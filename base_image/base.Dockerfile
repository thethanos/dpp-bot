FROM ubuntu:23.04

ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && \
    apt-get install --no-install-recommends -y libssl-dev zlib1g-dev libsodium-dev libopus-dev \
    cmake pkg-config g++-13 gcc-13 gdb git make tclsh && \
    apt-get clean && rm -rf /var/lib/apt/lists/*

ENV CC=/usr/bin/gcc-13
ENV CXX=/usr/bin/g++-13

WORKDIR /usr/src/spdlog

COPY spdlog/ .

WORKDIR /usr/src/spdlog/build

RUN cmake .. -DCMAKE_INSTALL_PREFIX=/usr
RUN make -j 6
RUN make install

WORKDIR /usr/src/DPP

COPY DPP/ .

WORKDIR /usr/src/DPP/build

RUN cmake .. -DDPP_BUILD_TEST=OFF
RUN make -j 6
RUN make install

WORKDIR /usr/src/sqlite

COPY sqlite/ .

WORKDIR /usr/src/sqlite/build

RUN ../configure --prefix=/usr
RUN make -j 6
RUN make install