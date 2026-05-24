FROM golang:1.26-alpine AS build

WORKDIR /src

COPY go/ ./go

WORKDIR /src/go

RUN CGO_ENABLED=0 GOOS=linux GOARCH=amd64 go build -o telemetry-backend ./cmd/server

FROM alpine:3.18

WORKDIR /app

COPY --from=build /src/go/telemetry-backend .

RUN chmod +x /app/telemetry-backend

EXPOSE 3000

ENTRYPOINT ["./telemetry-backend"]