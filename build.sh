#!/usr/bin/env bash
set -euo pipefail
g++ -std=c++17 -Isrc/libumlec src/libumlec/libumlec.cpp src/checker/checker.cpp -o checker
g++ -std=c++17 -Isrc/libumlec src/libumlec/libumlec.cpp src/analyzer/analyzer.cpp -o analyzer
echo "Built checker and analyzer."
