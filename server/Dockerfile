FROM ngrok/ngrok AS ngrok
FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    git \
    libcurl4-openssl-dev \
    libssl-dev \
    pkg-config \
    wget \
    unzip \
    libsasl2-dev \
    scons \
    libmongoc-1.0-0 \
    libmongoc-dev \
    libbson-1.0-0 \
    libbson-dev \
    ca-certificates \
    libboost-all-dev \
    libasio-dev \
    g++ \
    libstdc++-11-dev

WORKDIR /tmp/build

RUN git clone https://github.com/mongodb/mongo-c-driver.git \
    && cd mongo-c-driver \
    && git checkout 1.23.4 \
    && mkdir cmake-build && cd cmake-build \
    && cmake -DENABLE_AUTOMATIC_INIT_AND_CLEANUP=OFF -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local .. \
    && make -j$(nproc) && make install \
    && cd /tmp/build && rm -rf mongo-c-driver

RUN git clone https://github.com/mongodb/mongo-cxx-driver.git \
    && cd mongo-cxx-driver \
    && git checkout releases/v4.1 \
    && rm -rf build \
    && mkdir build && cd build \
    && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local .. \
    && make -j$(nproc) && make install \
    && cd /tmp/build && rm -rf mongo-cxx-driver

RUN git clone --recursive https://github.com/CrowCpp/crow.git \
    && cd crow \
    && mkdir build && cd build \
    && cmake -DCMAKE_BUILD_TYPE=Release -DCROW_ENABLE_SSL=ON -DUSE_SYSTEM_ASIO=ON -DCMAKE_INSTALL_PREFIX=/usr/local .. \
    && make -j$(nproc) && make install \
    && cd /tmp/build && rm -rf crow

RUN git clone https://github.com/nlohmann/json.git \
    && cd json \
    && git checkout v3.11.2 \
    && mkdir build && cd build \
    && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local .. \
    && make -j$(nproc) && make install \
    && cd /tmp/build && rm -rf json

RUN ldconfig

WORKDIR /app

COPY . .

RUN cmake -B build -S . && cmake --build build --config Release

COPY --from=ngrok /bin/ngrok /usr/local/bin/ngrok

CMD ["./build/server"]

EXPOSE 8080