FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && \
    apt-get install -y gcc make python3 python3-pip && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY . .

RUN make

CMD ["make", "test"]