// this file contains type declarations for data structures used throughout the program

#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "list.h"

struct FileData{
	const char* name;
	struct List pieces;
	struct List labels;
	struct List instructions;
	struct List commands;
};

// part of every piece struct, indentifies what data is in the union of each piece
enum PieceType{
	PT_NONE,
	PT_STRING,
	PT_INTEGER,
	PT_LINE = '\n',
	PT_DOT = '.',
	PT_EXPR_DELIM = ',',
	PT_ADD = '+',
	PT_SUB = '-',
	PT_LITERAL = '"',
	PT_DIV = '/',
	PT_MUL = '*'
};

// most basic unit of information for processing
// can represent a symbol or value (strings and numbers)
struct Piece{
	union{	// each piece uses only one of the union members
		size_t stridx;		// index of string start in array of characters (usually from global stringCharsList)
		int32_t integer;	// integer value
	};
	enum PieceType type;		// type of the piece
};

// holds data about any label, which is a string associated with value and represents locations in code or constants
struct Label{
	int32_t value;		// integer value of the label, can represent constants or offsets or other
	// can this be the enum and not uint8?
	uint8_t type;		// type of the label (defined, undefined, etc.)
	size_t name;		// index of identifier string start in array of characters
};

// information on all parts of 1 complete instruction
// all member sizes are accurate to what is on the 6502
struct Instruction{
	uint8_t size;		// byte length of ins
	uint8_t opcode;		// opcode value
	int32_t value;		// value of ins expression
	uint16_t offset;	// byte offset from beggining of instructions
	size_t expr;		// index of start of expression for evaluation
};

// change name maybe...
// enum for identifying what command occupies the union
enum CID{
	CID_DROP,	// place a value at address in memory where this command occurs
	CID_CONST,	// create a label with a custom value
	CID_ALLOC,	// create a label with an assembler defined value that is a memory block n bytes wide
	CID_SET,	// set a memory location to have a value, evaluated after all instructions placed and after all other commands
	CID_LABEL,
	CID_STRING,
	CID_NULL	// none
};

// holds data required to evaluate a command
// union holds structures for each command for indeterminate values
// used when a command cant be evaluated at first and needs to be done later
struct Command{
	enum CID id; // identifies what structure is in the union
	union{
		struct{ // drop command
			uint16_t offset;	// address offset to place the value
			size_t expr;		// index into pieceList for start of expression for the value
		} drop;

		struct{ // constant command
			size_t name;		// index into characterStringList for name of constant label
			size_t expr;		// index into pieceList for start of expression for value
		} constant;
		
		struct{ // allocate command
			size_t name;		// index into characterStringList for name of allocated label
			size_t expr;		// index into pieceList for start of expression for value
		} alloc;

		struct{ // set command
			size_t addr;		// index into pieceList for expression of address
			size_t value;		// same but expression of value
		} set;

		struct{ // label command
			size_t addr;
			size_t name;
		} label;

		struct{
			size_t name;
			size_t value;
			size_t offset;
		} string;
	};
};


// label types for telling the state of a label
enum LabelType{
	LT_DEFINED,	// label was given an explicit value
	LT_UNDEFINED,	// label is relative and linker gives it a value
	LT_ALLOC,	// label contains address of allocated ram
};

#endif
