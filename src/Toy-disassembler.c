/*
 ============================================================================
 Name        : Toy-disassembler.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>

#include "toy_disassembler.h"

int main(int argc, const char* argv[]) {
	toy_disassembler(argv[1]);

	return EXIT_SUCCESS;
}
