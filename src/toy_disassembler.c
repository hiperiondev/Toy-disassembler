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

#define SPC(n)  printf("%*s", n, "");
#define EP(x)   [x] = #x

const char *OP_STR[] = {
        EP(TOY_OP_EOF),                       //
        EP(TOY_OP_PASS),                      //
        EP(TOY_OP_ASSERT),                    //
        EP(TOY_OP_PRINT),                     //
        EP(TOY_OP_LITERAL),                   //
        EP(TOY_OP_LITERAL_LONG),              //
        EP(TOY_OP_LITERAL_RAW),               //
        EP(TOY_OP_NEGATE),                    //
        EP(TOY_OP_ADDITION),                  //
        EP(TOY_OP_SUBTRACTION),               //
        EP(TOY_OP_MULTIPLICATION),            //
        EP(TOY_OP_DIVISION),                  //
        EP(TOY_OP_MODULO),                    //
        EP(TOY_OP_GROUPING_BEGIN),            //
        EP(TOY_OP_GROUPING_END),              //
        EP(TOY_OP_SCOPE_BEGIN),               //
        EP(TOY_OP_SCOPE_END),                 //
        EP(TOY_OP_TYPE_DECL_removed),         //
        EP(TOY_OP_TYPE_DECL_LONG_removed),    //
        EP(TOY_OP_VAR_DECL),                  //
        EP(TOY_OP_VAR_DECL_LONG),             //
        EP(TOY_OP_FN_DECL),                   //
        EP(TOY_OP_FN_DECL_LONG),              //
        EP(TOY_OP_VAR_ASSIGN),                //
        EP(TOY_OP_VAR_ADDITION_ASSIGN),       //
        EP(TOY_OP_VAR_SUBTRACTION_ASSIGN),    //
        EP(TOY_OP_VAR_MULTIPLICATION_ASSIGN), //
        EP(TOY_OP_VAR_DIVISION_ASSIGN),       //
        EP(TOY_OP_VAR_MODULO_ASSIGN),         //
        EP(TOY_OP_TYPE_CAST),                 //
        EP(TOY_OP_TYPE_OF),                   //
        EP(TOY_OP_IMPORT),                    //
        EP(TOY_OP_EXPORT_removed),            //
        EP(TOY_OP_INDEX),                     //
        EP(TOY_OP_INDEX_ASSIGN),              //
        EP(TOY_OP_INDEX_ASSIGN_INTERMEDIATE), //
        EP(TOY_OP_DOT),                       //
        EP(TOY_OP_COMPARE_EQUAL),             //
        EP(TOY_OP_COMPARE_NOT_EQUAL),         //
        EP(TOY_OP_COMPARE_LESS),              //
        EP(TOY_OP_COMPARE_LESS_EQUAL),        //
        EP(TOY_OP_COMPARE_GREATER),           //
        EP(TOY_OP_COMPARE_GREATER_EQUAL),     //
        EP(TOY_OP_INVERT),                    //
        EP(TOY_OP_AND),                       //
        EP(TOY_OP_OR),                        //
        EP(TOY_OP_JUMP),                      //
        EP(TOY_OP_IF_FALSE_JUMP),             //
        EP(TOY_OP_FN_CALL),                   //
        EP(TOY_OP_FN_RETURN),                 //
        EP(TOY_OP_POP_STACK),                 //
        EP(TOY_OP_TERNARY),                   //
        EP(TOY_OP_FN_END),                    //
        };

const char *LIT_STR[] = {
        EP(TOY_LITERAL_NULL),                    //
        EP(TOY_LITERAL_BOOLEAN),                 //
        EP(TOY_LITERAL_INTEGER),                 //
        EP(TOY_LITERAL_FLOAT),                   //
        EP(TOY_LITERAL_STRING),                  //
        EP(TOY_LITERAL_ARRAY),                   //
        EP(TOY_LITERAL_DICTIONARY),              //
        EP(TOY_LITERAL_FUNCTION),                //
        EP(TOY_LITERAL_IDENTIFIER),              //
        EP(TOY_LITERAL_TYPE),                    //
        EP(TOY_LITERAL_OPAQUE),                  //
        EP(TOY_LITERAL_ANY),                     //
        EP(TOY_LITERAL_TYPE_INTERMEDIATE),       //
        EP(TOY_LITERAL_ARRAY_INTERMEDIATE),      //
        EP(TOY_LITERAL_DICTIONARY_INTERMEDIATE), //
        EP(TOY_LITERAL_FUNCTION_INTERMEDIATE),   //
        EP(TOY_LITERAL_FUNCTION_ARG_REST),       //
        EP(TOY_LITERAL_FUNCTION_NATIVE),         //
        EP(TOY_LITERAL_FUNCTION_HOOK),           //
        EP(TOY_LITERAL_INDEX_BLANK),             //
        };

enum TOY_ARG_TYPE {
    TOY_ARG_NONE,    //
    TOY_ARG_BYTE,    //
    TOY_ARG_WORD,    //
    TOY_ARG_INTEGER, //
    TOY_ARG_FLOAT,   //
    TOY_ARG_STRING   //
};

const uint8_t OP_ARGS[TOY_OP_END_OPCODES][2] = {
        { TOY_ARG_NONE, TOY_ARG_NONE }, // TOY_OP_EOF
        { TOY_ARG_NONE, TOY_ARG_NONE }, // TOY_OP_PASS
        { TOY_ARG_NONE, TOY_ARG_NONE }, // TOY_OP_ASSERT
        { TOY_ARG_NONE, TOY_ARG_NONE }, // TOY_OP_PRINT
        { TOY_ARG_BYTE, TOY_ARG_NONE }, // TOY_OP_LITERAL
        { TOY_ARG_WORD, TOY_ARG_NONE }, // TOY_OP_LITERAL_LONG
        { TOY_ARG_NONE, TOY_ARG_NONE }, // TOY_OP_LITERAL_RAW
        { TOY_ARG_NONE, TOY_ARG_NONE }, // TOY_OP_NEGATE
        { TOY_ARG_NONE, TOY_ARG_NONE }, // TOY_OP_ADDITION
        { TOY_ARG_NONE, TOY_ARG_NONE }, // TOY_OP_SUBTRACTION
        { TOY_ARG_NONE, TOY_ARG_NONE }, // TOY_OP_MULTIPLICATION
        { TOY_ARG_NONE, TOY_ARG_NONE }, // TOY_OP_DIVISION
        { TOY_ARG_NONE, TOY_ARG_NONE }, // TOY_OP_MODULO
        { TOY_ARG_NONE, TOY_ARG_NONE }, // TOY_OP_GROUPING_BEGIN
        { TOY_ARG_NONE, TOY_ARG_NONE }, // TOY_OP_GROUPING_END
        { TOY_ARG_NONE, TOY_ARG_NONE }, // TOY_OP_SCOPE_BEGIN
        { TOY_ARG_NONE, TOY_ARG_NONE }, // TOY_OP_SCOPE_END
        { TOY_ARG_NONE, TOY_ARG_NONE }, // TOY_OP_TYPE_DECL_removed
        { TOY_ARG_NONE, TOY_ARG_NONE }, // TOY_OP_TYPE_DECL_LONG_removed
        { TOY_ARG_BYTE, TOY_ARG_BYTE }, // TOY_OP_VAR_DECL
        { TOY_ARG_WORD, TOY_ARG_WORD }, // TOY_OP_VAR_DECL_LONG
        { TOY_ARG_BYTE, TOY_ARG_BYTE }, // TOY_OP_FN_DECL
        { TOY_ARG_WORD, TOY_ARG_WORD }, // TOY_OP_FN_DECL_LONG
        { TOY_ARG_NONE, TOY_ARG_NONE }, // TOY_OP_VAR_ASSIGN
        { TOY_ARG_NONE, TOY_ARG_NONE }, // TOY_OP_VAR_ADDITION_ASSIGN
        { TOY_ARG_NONE, TOY_ARG_NONE }, // TOY_OP_VAR_SUBTRACTION_ASSIGN
        { TOY_ARG_NONE, TOY_ARG_NONE }, // TOY_OP_VAR_MULTIPLICATION_ASSIGN
        { TOY_ARG_NONE, TOY_ARG_NONE }, // TOY_OP_VAR_DIVISION_ASSIGN
        { TOY_ARG_NONE, TOY_ARG_NONE }, // TOY_OP_VAR_MODULO_ASSIGN
        { TOY_ARG_NONE, TOY_ARG_NONE }, // TOY_OP_TYPE_CAST
        { TOY_ARG_NONE, TOY_ARG_NONE }, // TOY_OP_TYPE_OF
        { TOY_ARG_NONE, TOY_ARG_NONE }, // TOY_OP_IMPORT
        { TOY_ARG_NONE, TOY_ARG_NONE }, // TOY_OP_EXPORT_removed
        { TOY_ARG_NONE, TOY_ARG_NONE }, // TOY_OP_INDEX
        { TOY_ARG_BYTE, TOY_ARG_NONE }, // TOY_OP_INDEX_ASSIGN
        { TOY_ARG_NONE, TOY_ARG_NONE }, // TOY_OP_INDEX_ASSIGN_INTERMEDIATE
        { TOY_ARG_NONE, TOY_ARG_NONE }, // TOY_OP_DOT
        { TOY_ARG_NONE, TOY_ARG_NONE }, // TOY_OP_COMPARE_EQUAL
        { TOY_ARG_NONE, TOY_ARG_NONE }, // TOY_OP_COMPARE_NOT_EQUAL
        { TOY_ARG_NONE, TOY_ARG_NONE }, // TOY_OP_COMPARE_LESS
        { TOY_ARG_NONE, TOY_ARG_NONE }, // TOY_OP_COMPARE_LESS_EQUAL
        { TOY_ARG_NONE, TOY_ARG_NONE }, // TOY_OP_COMPARE_GREATER
        { TOY_ARG_NONE, TOY_ARG_NONE }, // TOY_OP_COMPARE_GREATER_EQUAL
        { TOY_ARG_NONE, TOY_ARG_NONE }, // TOY_OP_INVERT
        { TOY_ARG_WORD, TOY_ARG_NONE }, // TOY_OP_AND
        { TOY_ARG_WORD, TOY_ARG_NONE }, // TOY_OP_OR
        { TOY_ARG_WORD, TOY_ARG_NONE }, // TOY_OP_JUMP
        { TOY_ARG_WORD, TOY_ARG_NONE }, // TOY_OP_IF_FALSE_JUMP
        { TOY_ARG_NONE, TOY_ARG_NONE }, // TOY_OP_FN_CALL
        { TOY_ARG_NONE, TOY_ARG_NONE }, // TOY_OP_FN_RETURN
        { TOY_ARG_NONE, TOY_ARG_NONE }, // TOY_OP_POP_STACK
        { TOY_ARG_NONE, TOY_ARG_NONE }, // TOY_OP_TERNARY
};

typedef struct toy_program_s {
    uint8_t *program;
    uint32_t len;
    uint32_t pc;
} toy_program_t;

typedef struct toy_func_op_s {
    uint32_t start;
    uint32_t end;
} toy_func_op_t;

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

static char* readString(const uint8_t *tb, uint32_t *count) {
    const unsigned char *ret = tb + *count;
    *count += strlen((char*) ret) + 1; //+1 for null character
    return (char*) ret;
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
    printf("\nFile: %s, size: %zu\n", filename, fsize);

    fclose(f);
    return 0;
}

static void toy_read_header(toy_program_t **prg) {
    const unsigned char major = readByte((*prg)->program, &((*prg)->pc));
    const unsigned char minor = readByte((*prg)->program, &((*prg)->pc));
    const unsigned char patch = readByte((*prg)->program, &((*prg)->pc));
    const char *build = readString((*prg)->program, &((*prg)->pc));
    printf("[Header: Version: %d.%d.%d (%s)]\n", major, minor, patch, build);
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

#define S_OP(n) \
		switch (OP_ARGS[opcode][n]) { \
		    case TOY_ARG_NONE: \
		    break; \
		    case TOY_ARG_BYTE: \
		        uint = readByte((*prg)->program, &pc); \
		        printf(" b(%d)", uint); \
		    break; \
		    case TOY_ARG_WORD: \
		        uint = readWord((*prg)->program, &pc);\
		        printf(" w(%d)", uint); \
		    break; \
		    case TOY_ARG_INTEGER: \
		        intg = readInt((*prg)->program, &pc); \
		        printf(" i(%d)", intg); \
		    break; \
		    case TOY_ARG_FLOAT: \
		        flt = readFloat((*prg)->program, &pc); \
		        printf(" f(%f)", flt); \
		    break; \
		    case TOY_ARG_STRING: \
		        str = readString((*prg)->program, &pc); \
		        printf(" s(%s)", str); \
		    break; \
		    default: \
		        printf("ERROR, unknown argument type\n"); \
		        exit(1); \
		}

void toy_disassemble_section(toy_program_t **prg, uint32_t pc, uint32_t len, uint8_t spaces) {
    uint8_t opcode;
    uint32_t uint;
    int32_t intg;
    float flt;
    char *str;

    while (pc < len) {
        opcode = (*prg)->program[pc];
        printf("\n");
        SPC(spaces);
        printf("| [ %05d ](%03d) ", pc++, opcode);
        toy_print_opcode(opcode);

        if (opcode > TOY_OP_END_OPCODES)
            continue;

        S_OP(0);
        S_OP(1);
    }
}

#define LIT_ADD(a, b, c)  b[c] = a;  ++c;
static void toy_read_interpreter_sections(toy_program_t **prg, uint32_t *pc, uint8_t spaces) {
    uint32_t literal_count = 0;
    uint8_t literal_type[65536];

    const unsigned short literalCount = readWord((*prg)->program, pc);

    printf("\n");
    SPC(spaces);
    printf("| ( Reading %d literals )\n", literalCount);

    for (int i = 0; i < literalCount; i++) {
        const unsigned char literalType = readByte((*prg)->program, pc);

        switch (literalType) {
            case TOY_LITERAL_NULL:
                LIT_ADD(TOY_LITERAL_NULL, literal_type, literal_count);
                SPC(spaces);
                printf("  | ( null )\n");
                break;

            case TOY_LITERAL_BOOLEAN: {
                const bool b = readByte((*prg)->program, pc);
                LIT_ADD(TOY_LITERAL_BOOLEAN, literal_type, literal_count);
                SPC(spaces);
                printf("  | ( boolean %s )\n", b ? "true" : "false");
            }
                break;

            case TOY_LITERAL_INTEGER: {
                const int d = readInt((*prg)->program, pc);
                LIT_ADD(TOY_LITERAL_INTEGER, literal_type, literal_count);
                SPC(spaces);
                printf("  | ( integer %d )\n", d);
            }
                break;

            case TOY_LITERAL_FLOAT: {
                const float f = readFloat((*prg)->program, pc);
                LIT_ADD(TOY_LITERAL_FLOAT, literal_type, literal_count);
                SPC(spaces);
                printf("  | ( float %f )\n", f);
            }
                break;

            case TOY_LITERAL_STRING: {
                const char *s = readString((*prg)->program, pc);
                LIT_ADD(TOY_LITERAL_STRING, literal_type, literal_count);
                SPC(spaces);
                printf("  | ( string \"%s\" )\n", s);
            }
                break;

            case TOY_LITERAL_ARRAY_INTERMEDIATE:
            case TOY_LITERAL_ARRAY: {
                unsigned short length = readWord((*prg)->program, pc);
                SPC(spaces);
                printf("  | ( array ");
                for (int i = 0; i < length; i++) {
                    int index = readWord((*prg)->program, pc);
                    printf("%d ", index);
                    LIT_ADD(TOY_LITERAL_NULL, literal_type, literal_count);
                }
                printf(")\n");
                LIT_ADD(TOY_LITERAL_ARRAY, literal_type, literal_count);
            }
                break;

            case TOY_LITERAL_DICTIONARY_INTERMEDIATE:
            case TOY_LITERAL_DICTIONARY: {
                unsigned short length = readWord((*prg)->program, pc);
                SPC(spaces);
                printf("  | ( dictionary ");
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
                SPC(spaces);
                printf("  | ( function index: %d )\n", index);
            }
                break;

            case TOY_LITERAL_IDENTIFIER: {
                const char *str = readString((*prg)->program, pc);
                LIT_ADD(TOY_LITERAL_IDENTIFIER, literal_type, literal_count);
                SPC(spaces);
                printf("  | ( identifier %s )\n", str);
            }
                break;

            case TOY_LITERAL_TYPE:
            case TOY_LITERAL_TYPE_INTERMEDIATE: {
                uint8_t literalType = readByte((*prg)->program, pc);
                uint8_t constant = readByte((*prg)->program, pc);
                SPC(spaces);
                printf("  | ( type %s: %d)\n", (LIT_STR[literalType] + 12), constant);
                if (literalType == TOY_LITERAL_ARRAY) {
                    uint16_t vt = readWord((*prg)->program, pc);
                    SPC(spaces);
                    printf("    ( array: %d\n)", vt);
                }

                if (literalType == TOY_LITERAL_DICTIONARY) {
                    uint8_t kt = readWord((*prg)->program, pc);
                    uint8_t vt = readWord((*prg)->program, pc);
                    SPC(spaces);
                    printf("    | ( dictionary: [%d, %d] )\n", kt, vt);
                }
                LIT_ADD(literalType, literal_type, literal_count);
            }
                break;

            case TOY_LITERAL_INDEX_BLANK:
                LIT_ADD(TOY_LITERAL_INDEX_BLANK, literal_type, literal_count);
                SPC(spaces);
                printf("    | ( blank )\n");
                break;
        }
    }

    consumeByte(TOY_OP_SECTION_END, (*prg)->program, pc);

    int functionCount = readWord((*prg)->program, pc);
    int functionSize = readWord((*prg)->program, pc);

    if (functionCount) {
        SPC(spaces);
        printf("  | \n");
        SPC(spaces);
        printf("  | ( fun count: %d, total size: %d )\n", functionCount, functionSize);

        uint32_t fcnt = 0;
        for (int i = 0; i < literal_count; i++) {
            if (literal_type[i] == TOY_LITERAL_FUNCTION_INTERMEDIATE) {
                size_t size = (size_t) readWord((*prg)->program, pc);

                uint32_t fpc_start = *pc;
                uint32_t fpc_end = *pc + size - 1;

                SPC(spaces);
                printf("    | \n");
                SPC(spaces);
                printf("    | ( fun %d [ start: %d, end: %d ] )", fcnt, fpc_start, fpc_end);
                if ((*prg)->program[*pc + size - 1] != TOY_OP_FN_END) {
                    printf("\nERROR: Failed to find function end\n");
                    exit(1);
                }

                toy_read_interpreter_sections(prg, &fpc_start, spaces + 4);
                SPC(spaces);
                printf("    |\n");
                SPC(spaces + 4);
                printf("| -- FUNCTION CODE --");
                toy_disassemble_section(prg, fpc_start, fpc_end, spaces + 4);
                printf("\n");

                fcnt++;
                *pc += size;
            }
        }
    }

    consumeByte(TOY_OP_SECTION_END, (*prg)->program, pc);
}

///////////////////////////////////////////////////////////////////////////////

void toy_disassembler(const char *filename) {
    toy_program_t *prg;

    toy_disassembler_init(&prg);
    if (toy_load_file(filename, &prg))
        exit(1);

    toy_read_header(&prg);

    consumeByte(TOY_OP_SECTION_END, prg->program, &(prg->pc));

    printf("\n---- [LITERALS] ----");
    toy_read_interpreter_sections(&prg, &(prg->pc), 0);
    printf("-- [END LITERALS] --\n");

    printf("\n---- [PROGRAM] ----");
    toy_disassemble_section(&prg, prg->pc, prg->len, 0);
    printf("\n-- [END PROGRAM] --");

    printf("\n\n");
    toy_disassembler_deinit(&prg);
}
