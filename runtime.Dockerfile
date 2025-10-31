FROM ubuntu:noble

ENV DEBIAN_FRONTEND=noninteractive

SHELL ["/bin/bash", "-c"]

COPY wasm-micro-runtime /runtime/wasm-micro-runtime
COPY misc /runtime/misc
COPY toolchains /runtime/toolchains
COPY Makefile /runtime/

RUN apt-get update && apt-get install -y make cmake \
        ninja-build gcc wabt libstdc++-12-dev g++ lld \
        build-essential ccache \
        binfmt-support python3

RUN cd /runtime && make iwasm && \
        cd misc && source gen_iwasm_wrapper.sh

RUN env &> /runtime/.walienv

ENTRYPOINT ["/runtime/misc/iwasm-wrapper"]
