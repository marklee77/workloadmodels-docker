FROM ubuntu:trusty
MAINTAINER Mark Stillwell <mark@stillwell.me>

ENV DEBIAN_FRONTEND noninteractive
RUN apt-get update && \
    apt-get -y install build-essential && \
    rm -rf /var/lib/apt/lists/* /var/cache/apt/*

COPY . /root/workloadmodels
WORKDIR /root/workloadmodels/models
RUN make clean && make && make install
WORKDIR /root
