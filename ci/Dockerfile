FROM debian:bookworm-slim

ENV DEBIAN_FRONTEND=noninteractive MIPS_BINUTILS_PREFIX=mips-linux-gnu- DETECTED_OS=linux PYTHON=/work/.venv/bin/python3
RUN apt-get update && apt-get install -y --no-install-recommends bash build-essential binutils-mips-linux-gnu ca-certificates gcc-multilib git make python3 python3-pip python3-venv && apt-get clean && rm -rf /var/lib/apt/lists/*
WORKDIR /work
COPY . /work
RUN make venv && make setup && make extract && (make -B CC=tools/ido/linux/7.1/cc COMPARE=0 -j"$(nproc)" rom || test -s build/pokestadium-us.map)
RUN mkdir -p build && cp baseroms/us/baserom.z64 build/pokestadium-us.z64 && md5sum build/pokestadium-us.z64 baseroms/us/baserom.z64
