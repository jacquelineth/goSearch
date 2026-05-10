FROM golang:1.20-bullseye AS build

RUN apt-get update && apt-get install -y \
    libx11-dev libxext-dev libxcursor-dev libxrandr-dev \
    libxi-dev libxinerama-dev mesa-common-dev libgl1-mesa-dev xorg-dev

WORKDIR /app

COPY go.mod go.sum ./
RUN go mod download

COPY . .

RUN go build -o search ./cmd/search

FROM debian:stable-slim

WORKDIR /app
COPY --from=build /app/search .

CMD ["./search"]
