#ifndef OPCODES_H
#define OPCODES_H

#define HALT 0x00
#define PRINT 0x01
#define LOAD_CONST 0x02
#define ADD 0x03
#define SUB 0x04
#define MUL 0x05
#define DIV 0x06
#define STORE_VAR 0x07
#define LOAD_VAR 0x08
#define CALL 0x09
#define RETURN 0x0A
#define JUMP 0x0B
#define JUMP_IF_TRUE 0x0C
#define JUMP_IF_FALSE 0x0D
#define EQUAL 0x0E
#define NOT_EQUAL 0x0F
#define GREATER 0x10
#define LESS 0x11
#define GREATER_EQUAL 0x12
#define LESS_EQUAL 0x13
#define LOAD_ARG 0x14
#define DISCARD 0x15
#define PUSH_NIL 0x16
#define NEGATE 0x17
#define NOT 0x18
#endif 
