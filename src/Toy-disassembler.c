/*
 ============================================================================
 Name        : Toy-disassembler.c
 Author      : 
 Version     :
 Copyright   : Emiliano Augusto Gonzalez (egonzalez . hiperion @ gmail . com)
 Description : Toy language disassembler
 ============================================================================
 */

#include <stdlib.h>

#include "toy_disassembler.h"

int main(int argc, const char* argv[]) {
	toy_disassembler(argv[1]);

	return EXIT_SUCCESS;
}
