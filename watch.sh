#!/bin/bash
make
compile="g++ -pthread -fdiagnostics-color=always -Ih examples/main.cpp dist/libeasymt.a -o examples/main"
run="timeout 0.4 examples/main"
watch -ctn0.2 bash -c "cd '$PWD'; $compile && $run"
    
