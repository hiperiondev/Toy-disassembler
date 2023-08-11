/*
 * toy_disassembler.c
 *
 *  Created on: 10 ago. 2023
 *      Author: egonzalez
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "toy_opcodes.h"
#include "toy_literals.h"

#define EP(x) [x] = #x

const char *OP_STR[] = {
        EP(TOY_OP_EOF), //
        EP(TOY_OP_PASS), //
        EP(TOY_OP_ASSERT), //
        EP(TOY_OP_PRINT), //
        EP(TOY_OP_LITERAL), //
        EP(TOY_OP_LITERAL_LONG), //
        EP(TOY_OP_LITERAL_RAW), //
        EP(TOY_OP_NEGATE), //
        EP(TOY_OP_ADDITION), //
        EP(TOY_OP_SUBTRACTION), //
        EP(TOY_OP_MULTIPLICATION), //
        EP(TOY_OP_DIVISION), //
        EP(TOY_OP_MODULO), //
        EP(TOY_OP_GROUPING_BEGIN), //
        EP(TOY_OP_GROUPING_END), //
        EP(TOY_OP_SCOPE_BEGIN), //
        EP(TOY_OP_SCOPE_END), //
        EP(TOY_OP_TYPE_DECL_removed), //
        EP(TOY_OP_TYPE_DECL_LONG_removed), //
        EP(TOY_OP_VAR_DECL), //
        EP(TOY_OP_VAR_DECL_LONG), //
        EP(TOY_OP_FN_DECL), //
        EP(TOY_OP_FN_DECL_LONG), //
        EP(TOY_OP_VAR_ASSIGN), //
        EP(TOY_OP_VAR_ADDITION_ASSIGN), //
        EP(TOY_OP_VAR_SUBTRACTION_ASSIGN), //
        EP(TOY_OP_VAR_MULTIPLICATION_ASSIGN), //
        EP(TOY_OP_VAR_DIVISION_ASSIGN), //
        EP(TOY_OP_VAR_MODULO_ASSIGN), //
        EP(TOY_OP_TYPE_CAST), //
        EP(TOY_OP_TYPE_OF), //
        EP(TOY_OP_IMPORT), //
        EP(TOY_OP_EXPORT_removed), //
        EP(TOY_OP_INDEX), //
        EP(TOY_OP_INDEX_ASSIGN), //
        EP(TOY_OP_INDEX_ASSIGN_INTERMEDIATE), //
        EP(TOY_OP_DOT), //
        EP(TOY_OP_COMPARE_EQUAL), //
        EP(TOY_OP_COMPARE_NOT_EQUAL), //
        EP(TOY_OP_COMPARE_LESS), //
        EP(TOY_OP_COMPARE_LESS_EQUAL), //
        EP(TOY_OP_COMPARE_GREATER), //
        EP(TOY_OP_COMPARE_GREATER_EQUAL), //
        EP(TOY_OP_INVERT), //
        EP(TOY_OP_AND), //
        EP(TOY_OP_OR), //
        EP(TOY_OP_JUMP), //
        EP(TOY_OP_IF_FALSE_JUMP), //
        EP(TOY_OP_FN_CALL), //
        EP(TOY_OP_FN_RETURN), //
        EP(TOY_OP_POP_STACK), //
        EP(TOY_OP_TERNARY), //
        EP(TOY_OP_FN_END), //
};

const char *LIT_STR[] = {
        EP(TOY_LITERAL_NULL), //
        EP(TOY_LITERAL_BOOLEAN), //
        EP(TOY_LITERAL_INTEGER), //
        EP(TOY_LITERAL_FLOAT), //
        EP(TOY_LITERAL_STRING), //
        EP(TOY_LITERAL_ARRAY), //
        EP(TOY_LITERAL_DICTIONARY), //
        EP(TOY_LITERAL_FUNCTION), //
        EP(TOY_LITERAL_IDENTIFIER), //
        EP(TOY_LITERAL_TYPE), //
        EP(TOY_LITERAL_OPAQUE), //
        EP(TOY_LITERAL_ANY), //
        EP(TOY_LITERAL_TYPE_INTERMEDIATE), //
        EP(TOY_LITERAL_ARRAY_INTERMEDIATE), //
        EP(TOY_LITERAL_DICTIONARY_INTERMEDIATE), //
        EP(TOY_LITERAL_FUNCTION_INTERMEDIATE), //
        EP(TOY_LITERAL_FUNCTION_ARG_REST), //
        EP(TOY_LITERAL_FUNCTION_NATIVE), //
        EP(TOY_LITERAL_FUNCTION_HOOK), //
        EP(TOY_LITERAL_INDEX_BLANK), //
};

const uint8_t OP_ARGS[TOY_OP_END_OPCODES][3] = {
        { 0, 0, 0 }, // TOY_OP_EOF
        { 0, 0, 0 }, // TOY_OP_PASS
        { 0, 0, 0 }, // TOY_OP_ASSERT
        { 0, 0, 0 }, // TOY_OP_PRINT
        { 1, 0, 0 }, // TOY_OP_LITERAL
        { 2, 0, 0 }, // TOY_OP_LITERAL_LONG
        { 0, 0, 0 }, // TOY_OP_LITERAL_RAW
        { 0, 0, 0 }, // TOY_OP_NEGATE
        { 5, 0, 0 }, // TOY_OP_ADDITION
        { 0, 0, 0 }, // TOY_OP_SUBTRACTION
        { 0, 0, 0 }, // TOY_OP_MULTIPLICATION
        { 0, 0, 0 }, // TOY_OP_DIVISION
        { 0, 0, 0 }, // TOY_OP_MODULO
        { 0, 0, 0 }, // TOY_OP_GROUPING_BEGIN
        { 0, 0, 0 }, // TOY_OP_GROUPING_END
        { 0, 0, 0 }, // TOY_OP_SCOPE_BEGIN
        { 0, 0, 0 }, // TOY_OP_SCOPE_END
        { 0, 0, 0 }, // TOY_OP_TYPE_DECL_removed
        { 0, 0, 0 }, // TOY_OP_TYPE_DECL_LONG_removed
        { 1, 1, 5 }, // TOY_OP_VAR_DECL
        { 2, 2, 5 }, // TOY_OP_VAR_DECL_LONG
        { 1, 1, 0 }, // TOY_OP_FN_DECL
        { 2, 2, 0 }, // TOY_OP_FN_DECL_LONG
        { 0, 0, 0 }, // TOY_OP_VAR_ASSIGN
        { 0, 0, 0 }, // TOY_OP_VAR_ADDITION_ASSIGN
        { 0, 0, 0 }, // TOY_OP_VAR_SUBTRACTION_ASSIGN
        { 0, 0, 0 }, // TOY_OP_VAR_MULTIPLICATION_ASSIGN
        { 0, 0, 0 }, // TOY_OP_VAR_DIVISION_ASSIGN
        { 0, 0, 0 }, // TOY_OP_VAR_MODULO_ASSIGN
        { 0, 0, 0 }, // TOY_OP_TYPE_CAST
        { 0, 0, 0 }, // TOY_OP_TYPE_OF
        { 0, 0, 0 }, // TOY_OP_IMPORT
        { 0, 0, 0 }, // TOY_OP_EXPORT_removed
        { 0, 0, 0 }, // TOY_OP_INDEX
        { 1, 0, 0 }, // TOY_OP_INDEX_ASSIGN
        { 0, 0, 0 }, // TOY_OP_INDEX_ASSIGN_INTERMEDIATE
        { 0, 0, 0 }, // TOY_OP_DOT
        { 0, 0, 0 }, // TOY_OP_COMPARE_EQUAL
        { 0, 0, 0 }, // TOY_OP_COMPARE_NOT_EQUAL
        { 0, 0, 0 }, // TOY_OP_COMPARE_LESS
        { 0, 0, 0 }, // TOY_OP_COMPARE_LESS_EQUAL
        { 0, 0, 0 }, // TOY_OP_COMPARE_GREATER
        { 0, 0, 0 }, // TOY_OP_COMPARE_GREATER_EQUAL
        { 0, 0, 0 }, // TOY_OP_INVERT
        { 2, 0, 0 }, // TOY_OP_AND
        { 2, 0, 0 }, // TOY_OP_OR
        { 2, 0, 0 }, // TOY_OP_JUMP
        { 2, 0, 0 }, // TOY_OP_IF_FALSE_JUMP
        { 0, 0, 0 }, // TOY_OP_FN_CALL
        { 0, 0, 0 }, // TOY_OP_FN_RETURN
        { 0, 0, 0 }, // TOY_OP_POP_STACK
        { 0, 0, 0 }, // TOY_OP_TERNARY
        { 0, 0, 0 }, // TOY_OP_FN_END
};

typedef struct toy_program_s {
    uint8_t *program;
    uint32_t len;
    uint32_t pc;
} toy_program_t;

static void toy_print_opcode(uint8_t op);

static uint8_t readByte(const uint8_t *tb, uint32_t *count) {
    uint8_t ret = *(uint8_t*) (tb + *count);
    *count += 1;
    return ret;
}

static uint16_t readWord(const uint8_t *tb, uint32_t *count) {
    uint16_t ret = 0;
    memcpy(&ret, tb + *count, 2);
    *count += 2;
    return ret;
}

static int32_t readInt(const uint8_t *tb, uint32_t *count) {
    int ret = 0;
    memcpy(&ret, tb + *count, 4);
    *count += 4;
    return ret;
}

static float readFloat(const uint8_t *tb, uint32_t *count) {
    float ret = 0;
    memcpy(&ret, tb + *count, 4);
    *count += 4;
    return ret;
}

static const char* readString(const uint8_t *tb, uint32_t *count) {
    const unsigned char *ret = tb + *count;
    *count += strlen((char*) ret) + 1; //+1 for null character
    return (const char*) ret;
}

static void consumeByte(uint8_t byte, uint8_t *tb, uint32_t *count) {
    if (byte != tb[*count]) {
        printf("[internal] Failed to consume the correct byte (expected %u, found %u)\n", byte, tb[*count]);
        exit(1);
    }

    //printf("\n[ %05d ](%03d) ", *count, byte);
    //toy_print_opcode(byte);
    *count += 1;
}

///////////////////////////////////////////////////////////////////////////////

static void toy_disassembler_init(toy_program_t **prg) {
    (*prg) = malloc(sizeof(struct toy_program_s));
    (*prg)->len = 0;
    (*prg)->pc = 0;
}

static void toy_disassembler_deinit(toy_program_t **prg) {
    free((*prg)->program);
    free((*prg));
}

static uint8_t toy_load_file(const char *filename, toy_program_t **prg) {
    FILE *f;
    size_t fsize, bytes;
    uint32_t count = 0;
    uint8_t buf = 0;

    f = fopen(filename, "r");
    if (f == NULL) {
        printf("Not able to open the file.");
        return 1;
    }

    fseek(f, 0, SEEK_END);
    fsize = ftell(f);
    fseek(f, 0, SEEK_SET);
    (*prg)->program = malloc(fsize * sizeof(uint8_t));

    while ((bytes = fread(&buf, sizeof(uint8_t), 1, f)) == 1)
        (*prg)->program[count++] = buf;

    (*prg)->len = fsize;
    printf("File size: %zu\n", fsize);

    fclose(f);
    return 0;
}

static void toy_read_header(toy_program_t **prg) {
    const unsigned char major = readByte((*prg)->program, &((*prg)->pc));
    const unsigned char minor = readByte((*prg)->program, &((*prg)->pc));
    const unsigned char patch = readByte((*prg)->program, &((*prg)->pc));
    const char *build = readString((*prg)->program, &((*prg)->pc));
    printf("[Version: %d.%d.%d (%s)]\n", major, minor, patch, build);
}

static void toy_print_opcode(uint8_t op) {
    if (op == 255) {
        printf("SECTION_END");
        return;
    }

    if (op < TOY_OP_END_OPCODES)
        printf((OP_STR[op] + 7));
    else
        printf("(OP UNKNOWN [%c])", op);
}

///////////////////////////////////////////////////////////////////////////////

void toy_disassemble_section(toy_program_t **prg, uint32_t pc, uint32_t len) {
    uint8_t opcode;
    while (pc < len) {
        opcode = (*prg)->program[pc];
        printf("\n[ %05d ](%03d) ", pc++, opcode);
        toy_print_opcode(opcode);

        if (opcode > TOY_OP_END_OPCODES)
            continue;

        switch (OP_ARGS[opcode][0]) {
            case 0:
                break;
            case 1:
                printf(" byte(%d)", readByte((*prg)->program, &pc));
                break;
            case 2:
                printf(" word(%d)", readWord((*prg)->program, &pc));
                break;
            case 3:
                printf(" int(%d)", readInt((*prg)->program, &pc));
                break;
            case 4:
                printf(" float(%f)", readFloat((*prg)->program, &pc));
                break;
            case 5:
                printf(" string(%s)", readString((*prg)->program, &pc));
                break;
            default:
                printf("ERROR, unknown argument type\n");
                exit(1);
        }

        switch (OP_ARGS[opcode][1]) {
            case 0:
                break;
            case 1:
                printf(" byte(%d)", readByte((*prg)->program, &pc));
                break;
            case 2:
                printf(" word(%d)", readWord((*prg)->program, &pc));
                break;
            case 3:
                printf(" int(%d)", readInt((*prg)->program, &pc));
                break;
            case 4:
                printf(" float(%f)", readFloat((*prg)->program, &pc));
                break;
            case 5:
                printf(" string(%s)", readString((*prg)->program, &pc));
                break;
            default:
                printf("ERROR, unknown argument type\n");
                exit(1);
        }

        switch (OP_ARGS[opcode][2]) {
            case 0:
                break;
            case 1:
                printf(" byte(%d)", readByte((*prg)->program, &pc));
                break;
            case 2:
                printf(" word(%d)", readWord((*prg)->program, &pc));
                break;
            case 3:
                printf(" int(%d)", readInt((*prg)->program, &pc));
                break;
            case 4:
                printf(" float(%f)", readFloat((*prg)->program, &pc));
                break;
            case 5:
                printf(" string(%s)", readString((*prg)->program, &pc));
                break;
            default:
                printf("ERROR, unknown argument type\n");
                exit(1);
        }
    }
}

#define LIT_ADD(a, b, c) \
    b[c] = a; \
    ++c;

static void toy_read_interpreter_sections(toy_program_t **prg, uint32_t *pc) {
    uint32_t literal_count = 0;
    uint8_t literal_type[65536];

    const unsigned short literalCount = readWord((*prg)->program, pc);

    printf("\n-- Reading %d literals --\n", literalCount);

    for (int i = 0; i < literalCount; i++) {
        const unsigned char literalType = readByte((*prg)->program, pc);

        switch (literalType) {
            case TOY_LITERAL_NULL:
                LIT_ADD(TOY_LITERAL_NULL, literal_type, literal_count);
                printf("  (null)\n");
                break;

            case TOY_LITERAL_BOOLEAN: {
                const bool b = readByte((*prg)->program, pc);
                LIT_ADD(TOY_LITERAL_BOOLEAN, literal_type, literal_count);
                printf("  (boolean %s)\n", b ? "true" : "false");
            }
                break;

            case TOY_LITERAL_INTEGER: {
                const int d = readInt((*prg)->program, pc);
                LIT_ADD(TOY_LITERAL_INTEGER, literal_type, literal_count);
                printf("  (integer %d)\n", d);
            }
                break;

            case TOY_LITERAL_FLOAT: {
                const float f = readFloat((*prg)->program, pc);
                LIT_ADD(TOY_LITERAL_FLOAT, literal_type, literal_count);
                printf("  (float %f)\n", f);
            }
                break;

            case TOY_LITERAL_STRING: {
                const char *s = readString((*prg)->program, pc);
                LIT_ADD(TOY_LITERAL_STRING, literal_type, literal_count);
                printf("  (string \"%s\")\n", s);
            }
                break;

            case TOY_LITERAL_ARRAY_INTERMEDIATE:
            case TOY_LITERAL_ARRAY: {
                unsigned short length = readWord((*prg)->program, pc);
                printf("  (array ");
                for (int i = 0; i < length; i++) {
                    int index = readWord((*prg)->program, pc);
                    printf("%d, ", index);
                    LIT_ADD(TOY_LITERAL_NULL, literal_type, literal_count);
                }
                printf(")\n");
                LIT_ADD(TOY_LITERAL_ARRAY, literal_type, literal_count);
            }
                break;

            case TOY_LITERAL_DICTIONARY_INTERMEDIATE:
            case TOY_LITERAL_DICTIONARY: {
                unsigned short length = readWord((*prg)->program, pc);
                printf("  (dictionary ");
                for (int i = 0; i < length / 2; i++) {
                    int key = readWord((*prg)->program, pc);
                    int val = readWord((*prg)->program, pc);
                    printf("(key: %d, val:%d) ", key, val);
                }
                printf(")\n");
                LIT_ADD(TOY_LITERAL_DICTIONARY, literal_type, literal_count);
            }
                break;

            case TOY_LITERAL_FUNCTION: {
                unsigned short index = readWord((*prg)->program, pc);
                LIT_ADD(TOY_LITERAL_FUNCTION_INTERMEDIATE, literal_type, literal_count);
                printf("  (function index: %d)\n", index);
            }
                break;

            case TOY_LITERAL_IDENTIFIER: {
                const char *str = readString((*prg)->program, pc);
                LIT_ADD(TOY_LITERAL_IDENTIFIER, literal_type, literal_count);
                printf("  (identifier %s)\n", str);
            }
                break;

            case TOY_LITERAL_TYPE:
            case TOY_LITERAL_TYPE_INTERMEDIATE: {
                uint8_t literalType = readByte((*prg)->program, pc);
                uint8_t constant = readByte((*prg)->program, pc);
                printf("  (type %s: constant = %d)\n", (LIT_STR[literalType] + 12), constant);
                if (literalType == TOY_LITERAL_ARRAY) {
                    uint16_t vt = readWord((*prg)->program, pc);
                    printf("    -- LITERAL_ARRAY: %d\n", vt);
                }

                if (literalType == TOY_LITERAL_DICTIONARY) {
                    uint8_t kt = readWord((*prg)->program, pc);
                    uint8_t vt = readWord((*prg)->program, pc);
                    printf("    -- LITERAL_DICTIONARY: %d, %d\n", kt, vt);
                }
                LIT_ADD(literalType, literal_type, literal_count);
            }
                break;

            case TOY_LITERAL_INDEX_BLANK:
                LIT_ADD(TOY_LITERAL_INDEX_BLANK, literal_type, literal_count);
                printf("(blank)\n");
                break;
        }
    }

    consumeByte(TOY_OP_SECTION_END, (*prg)->program, pc);

    int functionCount = readWord((*prg)->program, pc);
    int functionSize = readWord((*prg)->program, pc);
    printf("\n  (fun count: %d, size: %d)\n", functionCount, functionSize);

    for (int i = 0; i < literal_count; i++) {
        if (literal_type[i] == TOY_LITERAL_FUNCTION_INTERMEDIATE) {
            size_t size = (size_t) readWord((*prg)->program, pc);
            printf("    (size: %zu)\n", size);
            if ((*prg)->program[*pc + size - 1] != TOY_OP_FN_END) {
                printf("ERROR: Failed to find function end");
                exit(1);
            }

            printf("[ start fun >>");
            uint32_t fpc = (*prg)->pc;
            toy_read_interpreter_sections(prg, &fpc);;
            printf("<< end fun ]\n");

            (*prg)->pc += size;
        }
    }

    consumeByte(TOY_OP_SECTION_END, (*prg)->program, pc);

    printf("-------------------------\n");
}

///////////////////////////////////////////////////////////////////////////////
void toy_disassembler(const char *filename) {
    toy_program_t *prg;
    //uint8_t opcode;

    toy_disassembler_init(&prg);
    if (toy_load_file(filename, &prg))
        exit(1);

    toy_read_header(&prg);

    consumeByte(TOY_OP_SECTION_END, prg->program, &(prg->pc));

    toy_read_interpreter_sections(&prg, &(prg->pc));

    toy_disassemble_section(&prg, prg->pc, prg->len);

    printf("\n\n");
    toy_disassembler_deinit(&prg);
}
