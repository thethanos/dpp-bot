FROM ubuntu:23.04

ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && \
    apt-get install --no-install-recommends -y libssl-dev zlib1g-dev libsodium-dev libopus-dev \
    cmake pkg-config g++-13 gcc-13 gdb git make tclsh libpq-dev && \
    apt-get clean && rm -rf /var/lib/apt/lists/*

ENV CC=/usr/bin/gcc-13
ENV CXX=/usr/bin/g++-13

# install spdlog https://github.com/gabime/spdlog.git
WORKDIR /usr/src/spdlog
COPY spdlog/ .
WORKDIR /usr/src/spdlog/build

RUN cmake .. -DCMAKE_INSTALL_PREFIX=/usr
RUN make -j 2
RUN make install

# install DPP https://github.com/brainboxdotcc/DPP.git
WORKDIR /usr/src/DPP
COPY DPP/ .
WORKDIR /usr/src/DPP/build

RUN cmake .. -DDPP_BUILD_TEST=OFF
RUN make -j 2
RUN make install

# install PostgreSQL https://github.com/jtv/libpqxx.git
WORKDIR /usr/src/libpqxx
COPY libpqxx/ .
WORKDIR /usr/src/libpqxx/build

RUN cmake .. -DCMAKE_INSTALL_PREFIX=/usr
RUN make -j 2
RUN make install

# install google test https://github.com/google/googletest.git
WORKDIR /usr/src/googletest
COPY googletest/ .
WORKDIR /usr/src/googletest/build

RUN cmake .. -DCMAKE_INSTALL_PREFIX=/usr
RUN make -j 2
RUN make install

# install redis
WORKDIR /usr/src/hiredis
COPY hiredis/ .
WORKDIR /usr/src/hiredis/build

RUN cmake .. -DCMAKE_INSTALL_PREFIX=/usr
RUN make -j 2
RUN make install