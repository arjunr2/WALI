# WALI-compiled Applications

This folder contains all applications and libraries ported to WALI. To build applications:
```shell
git submodule update --init .
make
```

**NOTE**: Some builds (e.g. bash) generate and execute intermediate WALI binaries in their
build process. To support this, register it as a miscellaneous binary format. 
