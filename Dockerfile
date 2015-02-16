FROM ubuntu:trusty
MAINTAINER Mark Stillwell <mark@stillwell.me>

ENV DEBIAN_FRONTEND noninteractive
RUN apt-get update && \
    apt-get -y install build-essential && \
    rm -rf /var/lib/apt/lists/* /var/cache/apt/*

WORKDIR /root

COPY . /root/workloadmodels
