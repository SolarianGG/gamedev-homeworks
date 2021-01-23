FROM ubuntu:latest

WORKDIR /homework
RUN apt update -y && apt install -y cmake ninja-build

RUN mdkir build && \
    cd build && \
    cmake -G Ninja .. && \
    cmake --build .
