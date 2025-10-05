FROM ubuntu:22.04

RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y \
    build-essential \
    sudo \
    gdb \
    make \
    cmake \
    git \
    e2fsprogs \
    dosfstools \
    xfsprogs \
    util-linux \
    mount \
    fuse \
 && apt-get clean && rm -rf /var/lib/apt/lists/*

RUN useradd -ms /bin/bash vscode
USER vscode
WORKDIR /workspace
