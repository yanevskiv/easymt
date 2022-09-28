#!/bin/bash
make
compile="g++ -pthread -fdiagnostics-color=always -Ih examples/main.cpp -Llib -leasymt -o examples/main"
run="timeout 0.4 examples/main"
watch -ctn0.1 bash -c "cd '$PWD'; $compile && $run"
    
