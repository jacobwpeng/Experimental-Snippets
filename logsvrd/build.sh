#!/bin/bash
mkdir -p build/ logs/ data/ && cd build/ && cmake .. -DCMAKE_BUILD_TYPE=debug && make
