# Webassembly Linux Interface (WALI)

This repo serves to document design decisions, requirements, and specifications for the WALI.

## Overview
We create a custom modified [musl libc](https://github.com/arjunr2/wali-musl) and produce a baseline
implementation in [WAMR](https://github.com/SilverLineFramework/wasm-micro-runtime/tree/wali)

## Resources
[Syscall Information Table](https://docs.google.com/spreadsheets/d/1__2NqMqGLHdjFFYonkF49IkGgfv62TJCpZuXqhXwnlc/edit?usp=sharing)

This paper (https://cseweb.ucsd.edu/~dstefan/pubs/johnson:2022:wave.pdf) and its related work section, especially the bit labeled "Modeling and verifying system interfaces"

