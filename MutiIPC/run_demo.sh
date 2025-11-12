#!/bin/bash
set -e
echo "Building..."
make
echo "Starting demo: ./main"
./main
