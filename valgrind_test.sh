#! /bin/bash

valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose Release/Toy_disassembler -a function-within-function-bugfix.tb 
