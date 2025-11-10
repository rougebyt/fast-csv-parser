# Dockerfile
FROM ubuntu:22.04

# Avoid interactive prompts
ENV DEBIAN_FRONTEND=noninteractive

# Install build tools and Python
RUN apt-get update && \
    apt-get install -y \
        gcc \
        make \
        python3 \
        python3-pip \
    && rm -rf /var/lib/apt/lists/*

# Set Python3 as default
RUN ln -s /usr/bin/python3 /usr/bin/python

WORKDIR /app

# Copy source code
COPY . .

# Build the CLI
RUN make

# Default command: run example
CMD ["./csvparse", "examples/sample.csv"]