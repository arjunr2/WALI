## Goals and Motivation of OS Syscall Interfaces for Wasm

<center> <h3> WebAssembly is not a niche platform -- it's a <b>software-defined ISA</b> and we should treat it like one!</h3> </center>

WALI (and system call interfaces in general) for WebAssembly aims to push lightweight virtualization down to the bottom of the software stack by 
virtualizing the *very bottom of an operating system's userspace*.
This has three major outcomes:

* **Software Reuse**: Modern software is underpinned by a plethora of
intricate software that has been deployed and optimized for decades. 
With WALI, Wasm ecosystems can immediately tap into this entire software ecosystem with minimal source code changes. 
These applications can now acquire  ISA portability, sandboxing, portability, CFI, and remote-code execution protection through a Wasm port.

* **Flexible and Modular Security Enforcement**: There is no silver bullet to security. 
The average user, average cloud ecosystem, and average automotive system all require security to various degrees of enforcement.
The goal should thus be: *how do we make it easy to accomodate and build these diverse abstraction?*. 
With WALI, we enables a clean layering approach to API design -- as the "bottom" layer, it provides feature-completeness and defers security enforcement occurs through higher-level APIs (e.g. WASI) **layered** over it. 
It is the goal of higher-level APIs to provide a safe and controlled interface to WALI (much like our operating systems of today).
By decoupling these requirements of feature completeness and security, we can accomodate for a wide variety of use-cases, while also allowing these higher-level APIs to be shipped as Wasm modules themselves!

* **One(or Few) Interfaces to Rule Them All (for VM Developers)**: Wasm engines are complex pieces of software, carefully weaving together high-performance JIT compilers, runtimes, garbage collectors, profiling/debugging tools, security, and more.
System interfaces are core yet non-trivial part of these engines, and maintaining many indepedent system interfaces for different domains is a huge burden of maintainability.
Instead, developers can now easily implement one or few system interfaces of interest, allowing focus on the core requirement of an engine -- **run bytecode fast**. 
Major parts of the engine can also be decoupled and virtualized as Wasm modules over WALI, keeping the core complexity of the trusted engine small.
