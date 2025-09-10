#!/bin/bash
set -e

echo "Running Shared Memory Tests..."
g++ -pthread back-end/shared-memory/shared_memory.cpp tests/test_shared_memory.cpp -lgtest -lgtest_main -o tests/test_shared_memory.out
./tests/test_shared_memory.out

echo "Running Local Sockets Tests..."
g++ -pthread back-end/local-sockets/local_sockets.cpp tests/test_local_sockets.cpp -lgtest -lgtest_main -o tests/test_local_sockets.out
./tests/test_local_sockets.out

echo "Running Anonymous Pipes Tests..."
g++ -pthread back-end/anonymous-pipes/anonymous_pipes.cpp tests/test_anonymous_pipes.cpp -lgtest -lgtest_main -o tests/test_anonymous_pipes.out
./tests/test_anonymous_pipes.out

echo "All tests have passed!"
