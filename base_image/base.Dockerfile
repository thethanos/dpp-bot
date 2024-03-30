FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && \
    apt-get install --no-install-recommends -y libssl-dev zlib1g-dev libsodium-dev libopus-dev \
    cmake pkg-config g++-12 gcc-12 gdb git make && \
    apt-get clean && rm -rf /var/lib/apt/lists/*

WORKDIR /usr/src/DPP

COPY DPP/ .

WORKDIR /usr/src/DPP/build

ENV CXX=/usr/bin/g++-12
RUN cmake .. -DDPP_BUILD_TEST=OFF
RUN make -j 6
RUN make install