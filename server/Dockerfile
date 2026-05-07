FROM debian:bookworm-slim AS builder

RUN apt-get update && apt-get install -y \
    gcc \
    libc6-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY include/ ./include/
COPY src/ ./src/

RUN gcc src/main.c src/monitor.c src/network.c src/info.c src/config.c -o monitor_server -lpthread

FROM debian:bookworm-slim

WORKDIR /app

COPY --from=builder /app/monitor_server .

EXPOSE 5000

CMD ["./monitor_server"]