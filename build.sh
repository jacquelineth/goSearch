#!/usr/bin/env bash
set -e
mkdir -p bin
go build -o bin/search ./cmd/search
if go build -o bin/searchui ./cmd/searchui; then
  printf "Built bin/search and bin/searchui\n"
else
  printf "Built bin/search; GUI build failed. Install system libraries required by Fyne/GLFW and rerun build.sh.\n"
fi
