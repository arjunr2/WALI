"""
types_abi.py - Source of truth for WALI ABI type definitions.

Defines the exact byte-level layout of all types used in WALI syscall interfaces.
All type sizes, alignments, and struct field offsets are either explicitly defined
or computed from C alignment rules. This file replaces the old WIT template files
(primitives.wit.template, complex.wit.template, records.wit.template) and type_system.py.

Usage:
    from types_abi import TypeSystem, C_PRIMITIVES, TYPE_ALIASES, ARRAY_TYPES, STRUCT_DEFS

    ts = TypeSystem.load()
    size = resolve_size("struct_stat")
"""

from __future__ import annotations
from dataclasses import dataclass, field
from typing import Dict, Set
import re
import logging

# =============================================================================
# Dataclass Definitions
# =============================================================================

@dataclass(frozen=True)
class Primitive:
    """A primitive type with known size and signedness."""
    size: int       # bytes: 1, 2, 4, or 8
    signed: bool

    @property
    def wit_name(self) -> str:
        """Compute WIT type name from size and signedness."""
        return f"{'s' if self.signed else 'u'}{self.size * 8}"


@dataclass(frozen=True)
class ArrayType:
    """A named fixed-size array type (e.g., sigset_t = uint8_t[128])."""
    element_type: str   # C_PRIMITIVES key
    count: int          # number of elements


@dataclass(frozen=True)
class Field:
    """A field in a struct definition."""
    name: str           # e.g., "st_dev", "pad0"
    type_name: str      # e.g., "dev_t", "uint32_t", "long[3]"
    offset: int | None = None   # Optional explicit byte offset override.
                                # If None: computed from C alignment rules.
                                # If set: used as-is (must not overlap previous field).


# =============================================================================
# Primitive Type Definitions (replaces primitives.wit.template)
# =============================================================================

C_PRIMITIVES: dict[str, Primitive] = {
    # Standard C types (64-bit Linux sizes)
    "int":                Primitive(4, True),
    "long":               Primitive(8, True),
    "long_long":          Primitive(8, True),
    "short":              Primitive(2, True),
    "reg":                Primitive(8, True),
    "unsigned":           Primitive(4, False),
    "unsigned_int":       Primitive(4, False),
    "unsigned_long":      Primitive(8, False),
    "unsigned_long_long": Primitive(8, False),
    "unsigned_short":     Primitive(2, False),
    "unsigned_reg":       Primitive(8, False),
    "syscall_result":     Primitive(8, True),
    # stdint types
    "uint8_t":            Primitive(1, False),
    "uint16_t":           Primitive(2, False),
    "uint32_t":           Primitive(4, False),
    "uint64_t":           Primitive(8, False),
    "int8_t":             Primitive(1, True),
    "int16_t":            Primitive(2, True),
    "int32_t":            Primitive(4, True),
    "int64_t":            Primitive(8, True),
    # Wasm32 specifics
    "ptr":                Primitive(4, True),    # all pointers are 4 bytes in wasm32
    "ptr_func":           Primitive(4, False),   # function pointer = table index
}


# =============================================================================
# Type Aliases (replaces complex.wit.template)
# =============================================================================

TYPE_ALIASES: dict[str, str] = {
    # Syscall argument types
    "off_t":        "long_long",
    "size_t":       "unsigned_int",
    "nfds_t":       "unsigned_long_long",
    "socklen_t":    "unsigned_int",
    "clockid_t":    "int",
    "mode_t":       "int",
    "uid_t":        "int",
    "pid_t":        "int",
    "gid_t":        "int",
    # Syscall param reference types
    "rlim_t":       "unsigned_long_long",
    "time_t":       "int64_t",
    "suseconds_t":  "int64_t",
    "sa_family_t":  "unsigned_short",
    "ino_t":        "uint64_t",
    "dev_t":        "uint64_t",
    "nlink_t":      "unsigned_reg",
    "blksize_t":    "long",
    "blkcnt_t":     "int64_t",
    "fsblkcnt_t":   "uint64_t",
    "fsfilcnt_t":   "uint64_t",
    "epoll_data_t": "int64_t",
}


# =============================================================================
# Named Array Types (replaces array type entries in complex.wit.template)
# =============================================================================

ARRAY_TYPES: dict[str, ArrayType] = {
    "sigset_t":  ArrayType("uint8_t",  128),
    "fsid_t":    ArrayType("int32_t",  2),
    "uts_str":   ArrayType("uint8_t",  65),
}


# =============================================================================
# Resolution Functions
# =============================================================================

# Forward declaration — populated by StructDef.__post_init__
STRUCT_DEFS: dict[str, StructDef] = {}

_INLINE_ARRAY_RE = re.compile(r'^(\S+?)\[(\d+)\]$')


def _parse_inline_array(type_name: str) -> tuple[int, str] | None:
    """Parse 'T[N]' inline array string. Returns (count, element_type) or None."""
    m = _INLINE_ARRAY_RE.match(type_name)
    if m:
        return (int(m.group(2)), m.group(1))
    return None


def resolve_primitive(type_name: str) -> Primitive:
    """Resolve any scalar type name to its underlying Primitive.

    Walks the alias chain: off_t -> long_long -> Primitive(8, True).
    Raises RuntimeError for array/struct types (no single primitive).
    """
    if type_name in C_PRIMITIVES:
        return C_PRIMITIVES[type_name]
    if type_name in TYPE_ALIASES:
        return resolve_primitive(TYPE_ALIASES[type_name])
    raise RuntimeError(f"Cannot resolve '{type_name}' to a primitive "
                       f"(not in C_PRIMITIVES or TYPE_ALIASES)")


def resolve_size(type_name: str) -> int:
    """Return the byte size of any type (primitive, alias, array, struct, inline array)."""
    if type_name in C_PRIMITIVES:
        return C_PRIMITIVES[type_name].size
    if type_name in TYPE_ALIASES:
        return resolve_size(TYPE_ALIASES[type_name])
    if type_name in ARRAY_TYPES:
        at = ARRAY_TYPES[type_name]
        return at.count * resolve_size(at.element_type)
    if type_name in STRUCT_DEFS:
        return STRUCT_DEFS[type_name].total_size
    arr = _parse_inline_array(type_name)
    if arr:
        count, elem = arr
        return count * resolve_size(elem)
    raise RuntimeError(f"Cannot resolve size for type '{type_name}'")


def resolve_alignment(type_name: str) -> int:
    """Return the alignment requirement (bytes) for any type."""
    if type_name in C_PRIMITIVES:
        return C_PRIMITIVES[type_name].size  # natural alignment
    if type_name in TYPE_ALIASES:
        return resolve_alignment(TYPE_ALIASES[type_name])
    if type_name in ARRAY_TYPES:
        return resolve_alignment(ARRAY_TYPES[type_name].element_type)
    if type_name in STRUCT_DEFS:
        return STRUCT_DEFS[type_name].alignment
    arr = _parse_inline_array(type_name)
    if arr:
        _, elem = arr
        return resolve_alignment(elem)
    raise RuntimeError(f"Cannot resolve alignment for type '{type_name}'")


def _align_up(offset: int, alignment: int) -> int:
    """Round offset up to the next multiple of alignment."""
    return (offset + alignment - 1) & ~(alignment - 1)


def compute_struct_layout(fields: list[Field]) -> tuple[list[int], list[int], int, int]:
    """Compute struct layout using C alignment rules.

    Returns (field_offsets, field_sizes, total_size, struct_alignment).
    Fields with explicit offsets override the computed position.
    """
    offsets: list[int] = []
    sizes: list[int] = []
    current_offset = 0
    max_alignment = 1

    for f in fields:
        f_size = resolve_size(f.type_name)
        f_align = resolve_alignment(f.type_name)
        max_alignment = max(max_alignment, f_align)

        if f.offset is not None:
            # Explicit offset: validate no overlap
            assert f.offset >= current_offset, \
                f"Field '{f.name}': explicit offset {f.offset} overlaps " \
                f"previous field end at {current_offset}"
            current_offset = f.offset
        else:
            # Computed offset: align up from current position
            current_offset = _align_up(current_offset, f_align)

        offsets.append(current_offset)
        sizes.append(f_size)
        current_offset += f_size

    total_size = _align_up(current_offset, max_alignment)
    return offsets, sizes, total_size, max_alignment


# =============================================================================
# Struct Definition (with auto-computed layout)
# =============================================================================

@dataclass
class StructDef:
    """A struct type with explicit fields and computed byte-level layout."""
    name: str
    fields: list[Field]
    expected_size: int | None = None

    # Computed in __post_init__
    field_offsets: list[int] = field(default_factory=list, init=False, repr=False)
    field_sizes: list[int] = field(default_factory=list, init=False, repr=False)
    total_size: int = field(default=0, init=False)
    alignment: int = field(default=1, init=False)

    def __post_init__(self):
        self.field_offsets, self.field_sizes, self.total_size, self.alignment = \
            compute_struct_layout(self.fields)
        if self.expected_size is not None:
            assert self.total_size == self.expected_size, \
                f"{self.name}: computed size {self.total_size} != expected {self.expected_size}"
        STRUCT_DEFS[self.name] = self


# =============================================================================
# Struct Definitions (replaces records.wit.template)
# ORDER MATTERS: referenced structs must be defined before their dependents.
# =============================================================================

_struct_defs_list = [
    # --- Leaf structs (no struct field dependencies) ---

    StructDef("struct_timespec", [
        Field("tv_sec",  "time_t"),
        Field("tv_nsec", "long"),
    ], expected_size=16),

    StructDef("struct_timeval", [
        Field("tv_sec",  "time_t"),
        Field("tv_usec", "suseconds_t"),
    ], expected_size=16),

    StructDef("struct_timezone", [
        Field("tz_minuteswest", "int"),
        Field("tz_dsttime",     "int"),
    ], expected_size=8),

    StructDef("stack_t", [
        Field("ss_sp",    "ptr"),
        Field("ss_flags", "int32_t"),
        Field("ss_size",  "size_t"),
    ]),

    StructDef("struct_dirent", [
        Field("d_ino",    "ino_t"),
        Field("d_off",    "off_t"),
        Field("d_reclen", "unsigned_short"),
        Field("d_type",   "uint8_t"),
        Field("d_name",   "uint8_t[256]"),
    ]),

    StructDef("struct_epoll_event", [
        Field("events", "uint32_t"),
        Field("data",   "epoll_data_t"),
    ]),

    StructDef("struct_iovec", [
        Field("iov_base", "ptr"),
        Field("iov_len",  "size_t"),
    ]),

    StructDef("struct_pollfd", [
        Field("fd",      "int32_t"),
        Field("events",  "short"),
        Field("revents", "short"),
    ]),

    StructDef("struct_rlimit", [
        Field("rlim_cur", "rlim_t"),
        Field("rlim_max", "rlim_t"),
    ], expected_size=16),

    StructDef("struct_sigaction", [
        Field("sa_handler",  "ptr_func"),
        Field("sa_mask",     "sigset_t"),
        Field("sa_flags",    "int32_t"),
        Field("sa_restorer", "ptr_func"),
    ]),

    StructDef("struct_sockaddr", [
        Field("sa_family", "sa_family_t"),
        Field("sa_data",   "uint8_t[14]"),
    ]),

    StructDef("struct_statfs", [
        Field("f_type",    "unsigned_long"),
        Field("f_bsize",   "unsigned_long"),
        Field("f_blocks",  "fsblkcnt_t"),
        Field("f_bfree",   "fsblkcnt_t"),
        Field("f_bavail",  "fsblkcnt_t"),
        Field("f_files",   "fsfilcnt_t"),
        Field("f_ffree",   "fsfilcnt_t"),
        Field("f_fsid",    "fsid_t"),
        Field("f_namelen", "unsigned_long"),
        Field("f_frsize",  "unsigned_long"),
        Field("f_flags",   "unsigned_long"),
        Field("f_spare",   "unsigned_long[4]"),
    ]),

    StructDef("stx_time", [
        Field("tv_sec", "int64_t"),
        Field("tv_nsec", "uint32_t"),
        Field("pad",     "int32_t"),
    ], expected_size=16),

    StructDef("struct_sysinfo", [
        Field("uptime",    "unsigned_long"),
        Field("loads",     "unsigned_long[3]"),
        Field("totalram",  "unsigned_long"),
        Field("freeram",   "unsigned_long"),
        Field("sharedram", "unsigned_long"),
        Field("bufferram", "unsigned_long"),
        Field("totalswap", "unsigned_long"),
        Field("freeswap",  "unsigned_long"),
        Field("procs",     "unsigned_short"),
        Field("pad",       "unsigned_short"),
        Field("totalhigh", "unsigned_long"),
        Field("freehigh",  "unsigned_long"),
        Field("mem_unit",  "uint32_t"),
        Field("reserved",  "uint8_t[256]"),
    ]),

    StructDef("struct_utimbuf", [
        Field("actime",  "time_t"),
        Field("modtime", "time_t"),
    ], expected_size=16),

    StructDef("struct_utsname", [
        Field("sysname",    "uts_str"),
        Field("nodename",   "uts_str"),
        Field("release",    "uts_str"),
        Field("version",    "uts_str"),
        Field("machine",    "uts_str"),
        Field("domainname", "uts_str"),
    ]),

    StructDef("fd_set", [
        Field("fds_bits", "uint8_t[128]"),
    ]),

    StructDef("cpu_set_t", [
        Field("bits", "uint8_t[128]"),
    ]),

    # --- Structs with struct field dependencies ---

    StructDef("struct_itimerval", [
        Field("it_interval", "struct_timeval"),
        Field("it_value",    "struct_timeval"),
    ]),

    StructDef("struct_msghdr", [
        Field("msg_name",       "ptr"),
        Field("msg_namelen",    "socklen_t"),
        Field("msg_iov",        "struct_iovec"),
        Field("msg_iovlen",     "int32_t"),
        Field("pad1",           "int32_t"),
        Field("msg_control",    "ptr"),
        Field("msg_controllen", "socklen_t"),
        Field("pad2",           "int32_t"),
        Field("msg_flags",      "int32_t"),
    ]),

    StructDef("struct_rusage", [
        Field("ru_utime",    "struct_timeval"),
        Field("ru_stime",    "struct_timeval"),
        Field("ru_maxrss",   "long"),
        Field("ru_ixrss",    "long"),
        Field("ru_idrss",    "long"),
        Field("ru_isrss",    "long"),
        Field("ru_minflt",   "long"),
        Field("ru_majflt",   "long"),
        Field("ru_nswap",    "long"),
        Field("ru_inblock",  "long"),
        Field("ru_oublock",  "long"),
        Field("ru_msgsng",   "long"),
        Field("ru_msgrcv",   "long"),
        Field("ru_nsignals", "long"),
        Field("ru_nvcsw",    "long"),
        Field("ru_nivcsw",   "long"),
        Field("reserved",    "long[16]"),
    ]),

    StructDef("struct_stat", [
        Field("st_dev",     "dev_t"),
        Field("st_ino",     "ino_t"),
        Field("st_nlink",   "nlink_t"),
        Field("st_mode",    "mode_t"),
        Field("st_uid",     "uid_t"),
        Field("st_gid",     "gid_t"),
        Field("pad0",       "uint32_t"),
        Field("st_rdev",    "dev_t"),
        Field("st_size",    "off_t"),
        Field("st_blksize", "blksize_t"),
        Field("st_blocks",  "blkcnt_t"),
        Field("st_atim",    "struct_timespec"),
        Field("st_mtim",    "struct_timespec"),
        Field("st_ctim",    "struct_timespec"),
        Field("unused",     "long[3]"),
    ], expected_size=144),

    StructDef("struct_statx", [
        Field("stx_mask",            "uint32_t"),
        Field("stx_blksize",         "uint32_t"),
        Field("stx_attributes",      "uint64_t"),
        Field("stx_nlink",           "uint32_t"),
        Field("stx_uid",             "uint32_t"),
        Field("stx_gid",             "uint32_t"),
        Field("stx_mode",            "uint16_t"),
        Field("pad1",                "uint16_t"),
        Field("stx_ino",             "uint64_t"),
        Field("stx_size",            "uint64_t"),
        Field("stx_blocks",          "uint64_t"),
        Field("stx_attributes_mask", "uint64_t"),
        Field("stx_atime",           "stx_time"),
        Field("stx_btime",           "stx_time"),
        Field("stx_ctime",           "stx_time"),
        Field("stx_mtime",           "stx_time"),
        Field("stx_rdev_major",      "uint32_t"),
        Field("stx_rdev_minor",      "uint32_t"),
        Field("stx_dev_major",       "uint32_t"),
        Field("stx_dev_minor",       "uint32_t"),
        Field("spare",               "uint64_t[14]"),
    ]),
]

assert len(STRUCT_DEFS) == len(_struct_defs_list), \
    f"Duplicate struct names detected: expected {len(_struct_defs_list)}, got {len(STRUCT_DEFS)}"


# =============================================================================
# Naming Helpers
# =============================================================================

def to_wit_name(name: str) -> str:
    """Convert underscore-style internal name to WIT hyphen-style."""
    return name.replace('_', '-')


def to_c_name(name: str) -> str:
    """Internal names are already C-compatible (underscored)."""
    return name


# =============================================================================
# WIT Type Name Resolution (for generators)
# =============================================================================

# Set of C_PRIMITIVES keys that are stdint types (emit as WIT primitives, not aliases)
_STDINT_NAMES = {k for k in C_PRIMITIVES if k.endswith('_t') and k[0] in 'iu'}


def field_type_to_wit(type_name: str) -> str:
    """Convert a source-of-truth field type name to its WIT representation.

    - TYPE_ALIASES/ARRAY_TYPES/STRUCT_DEFS: hyphenated alias name (e.g., time-t)
    - stdint C_PRIMITIVES (uint32_t etc.): WIT primitive (u32)
    - other C_PRIMITIVES (long, int): hyphenated alias name (long, int)
    - Inline T[N]: converted to Array[N, wit_T] for WIT tuple expansion
    """
    if type_name in TYPE_ALIASES:
        return to_wit_name(type_name)
    if type_name in ARRAY_TYPES:
        return to_wit_name(type_name)
    if type_name in STRUCT_DEFS:
        return to_wit_name(type_name)
    if type_name in C_PRIMITIVES:
        if type_name in _STDINT_NAMES:
            return C_PRIMITIVES[type_name].wit_name
        return to_wit_name(type_name)
    arr = _parse_inline_array(type_name)
    if arr:
        count, elem = arr
        wit_elem = field_type_to_wit(elem)
        return f"Array[{count}, {wit_elem}]"
    raise RuntimeError(f"Cannot convert type '{type_name}' to WIT name")


# =============================================================================
# TypeSystem (backward-compatible interface for generators)
# =============================================================================

@dataclass
class TypeSystem:
    """Backward-compatible type system interface consumed by autogen generators.

    Provides the same basic_types, complex_types, combined_types, wit_primitive_set
    dictionaries as the old type_system.py, but built from the source-of-truth
    definitions above instead of parsing WIT template files.
    """
    basic_types: Dict[str, str] = field(default_factory=dict)
    complex_types: Dict[str, str] = field(default_factory=dict)
    combined_types: Dict[str, str] = field(default_factory=dict)
    wit_primitive_set: Set[str] = field(default_factory=set)

    @classmethod
    def load(cls) -> 'TypeSystem':
        """Build TypeSystem from source-of-truth definitions."""
        # basic_types: C type name (spaces) -> WIT primitive name
        # Skip stdint types — they weren't in the original and use underscores
        basic_types: Dict[str, str] = {}
        for c_name, prim in C_PRIMITIVES.items():
            if c_name in _STDINT_NAMES:
                continue
            key = c_name.replace('_', ' ')
            basic_types[key] = prim.wit_name

        # complex_types: alias name (underscores) -> resolved target
        # Target format must match old behavior:
        #   - If target resolves to a basic_types key (space-separated), use that
        #   - If target is a stdint type, use its WIT primitive name
        #   - If target is another alias, use underscore format
        #   - Array types emit as Array[N, T] notation with WIT element types
        complex_types: Dict[str, str] = {}
        for alias_name, target in TYPE_ALIASES.items():
            complex_types[alias_name] = _resolve_for_typesystem(target, basic_types)

        for arr_name, arr_type in ARRAY_TYPES.items():
            elem_wit = C_PRIMITIVES[arr_type.element_type].wit_name
            complex_types[arr_name] = f"Array[{arr_type.count}, {elem_wit}]"

        combined = {**basic_types, **complex_types}
        primitives_set = set(basic_types.values())

        logging.debug(f"Loaded Basic Types: {basic_types}")
        logging.debug(f"Loaded Complex Types: {complex_types}")

        return cls(basic_types, complex_types, combined, primitives_set)


def _resolve_for_typesystem(target: str, basic_types: dict) -> str:
    """Resolve a TYPE_ALIASES target to a TypeSystem-compatible value.

    Produces the same values as the old regex-based parser from type_system.py.
    """
    # If target (with underscores -> spaces) is a basic_types key, use space form
    space_target = target.replace('_', ' ')
    if space_target in basic_types:
        return space_target

    # If target is a stdint C_PRIMITIVES entry, return its WIT primitive name
    if target in C_PRIMITIVES and target in _STDINT_NAMES:
        return C_PRIMITIVES[target].wit_name

    # Otherwise keep as-is (e.g., another alias name in underscore format)
    return target


# =============================================================================
# Validation
# =============================================================================

def validate():
    """Validate all type references resolve and print struct layouts."""
    errors = []

    # Check TYPE_ALIASES targets resolve
    for name, target in TYPE_ALIASES.items():
        try:
            resolve_size(target)
        except RuntimeError as e:
            errors.append(f"TYPE_ALIAS '{name}' -> '{target}': {e}")

    # Check ARRAY_TYPES element types resolve
    for name, at in ARRAY_TYPES.items():
        try:
            resolve_size(at.element_type)
        except RuntimeError as e:
            errors.append(f"ARRAY_TYPE '{name}' element '{at.element_type}': {e}")

    # Check all struct field types resolve
    for sd in STRUCT_DEFS.values():
        for f in sd.fields:
            try:
                resolve_size(f.type_name)
            except RuntimeError as e:
                errors.append(f"STRUCT '{sd.name}' field '{f.name}' type '{f.type_name}': {e}")

    if errors:
        for e in errors:
            logging.error(e)
        raise RuntimeError(f"{len(errors)} type resolution errors found")

    logging.info("All type references validated successfully.")


def print_struct_layouts():
    """Print computed struct layouts for verification."""
    for sd in STRUCT_DEFS.values():
        print(f"\n{sd.name} (size={sd.total_size}, align={sd.alignment}):")
        for f, offset, size in zip(sd.fields, sd.field_offsets, sd.field_sizes):
            marker = " [explicit]" if f.offset is not None else ""
            print(f"  +{offset:>4}  {f.name:<24} {f.type_name:<28} ({size} bytes){marker}")
        print(f"  total: {sd.total_size} bytes")


if __name__ == '__main__':
    import sys
    logging.basicConfig(level=logging.INFO, format='%(levelname)s: %(message)s')

    try:
        validate()
    except RuntimeError:
        sys.exit(1)

    print_struct_layouts()

    # Also verify TypeSystem loads correctly
    ts = TypeSystem.load()
    print(f"\nTypeSystem loaded: {len(ts.basic_types)} basic, "
          f"{len(ts.complex_types)} complex, {len(ts.wit_primitive_set)} primitives")
