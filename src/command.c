#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "list.h"
#include "types.h"
#include "utility.h"
#include "error.h"
#include "stringmanip.h"

static struct FileData* currf;
static const char* formats[] = {
	[CID_LABEL] = ".LABEL STRING:LABEL NAME",
	[CID_CONST] = ".CONST STIRNG:CONSTANT NAME, EXPR:CONSTANT VALUE",
	[CID_DROP] = ".DROP EXPR:DROP VALUE",
	[CID_ALLOC] = ".ALLOC STRING:LABEL NAME, EXPR:ALLOC SIZE",
	[CID_STRING] = ".STRING STRING:STRING NAME, STRING:CONTENTS",
	[CID_SET] = ".SET EXPR:ADDRESS, EXPR:VALUE",
};

// these static functions check the formatting and create a command structure

// for LABEL command, creates an undefined label with name and relative location
static struct Command comLabel(struct Piece in[]){
	struct Command c = {.id = CID_NULL};

	if(exprArrayLen(in) != -2){
		addErrorMessage(formats[CID_LABEL]);
		addErrorMessage("first/final argument given incorrectly");
		return c;
	}
	if(in[0].type != PT_STRING){
		addErrorMessage(formats[CID_LABEL]);
		addErrorMessage("string expeceted for label name");
		return c;
	}

	c.id = CID_LABEL;
	c.label.addr = memIdx;
	c.label.name = in[0].stridx;
	return c;
}

// for CONST command, creates a defined label with name and value
static struct Command comConst(struct Piece in[]){
	struct Command c = {.id = CID_NULL};

	struct Piece* p = in;
	if(exprArrayLen(p) != 2){
		addErrorMessage(formats[CID_CONST]);
		addErrorMessage("first argument given incorrectly");
		return c;
	}
	if(p[0].type != PT_STRING){
		addErrorMessage("string expeceted for constant name");
		return c;
	}
	p += 2;
	if(exprArrayLen(p) > -2){
		addErrorMessage(formats[CID_CONST]);
		addErrorMessage("second/final argument given incorrectly");
		return c;
	}

	c.id = CID_CONST;
	c.constant.name = in[0].stridx;
	c.constant.expr = p - (struct Piece*)currf->pieces.data;
	return c;
}

// for DROP command, place a byte value at current relative position in memory
static struct Command comDrop(struct Piece in[]){
	struct Command c = {.id = CID_NULL};

	if(exprArrayLen(in) > -2){
		addErrorMessage(formats[CID_DROP]);
		addErrorMessage("first/final argument given incorrectly");
		return c;
	}
	c.id = CID_DROP;
	c.drop.expr = in - (struct Piece*)currf->pieces.data;
	c.drop.offset = memIdx++;
	return c;
}

// for ALLOC command, label takes the value of an incremental address given by assembler from a name and byte length
static struct Command comAlloc(struct Piece in[]){
	struct Command c = {.id = CID_NULL};

	struct Piece* p = in;
	if(exprArrayLen(p) != 2){
		addErrorMessage(formats[CID_ALLOC]);
		addErrorMessage("first argument given incorrectly");
		return c;
	}
	if(p[0].type != PT_STRING){
		addErrorMessage("string expeceted for alloc name");
		return c;
	}
	p += 2;
	if(exprArrayLen(p) > -2){
		addErrorMessage(formats[CID_ALLOC]);
		addErrorMessage("second/final argument given incorrectly");
		return c;
	}

	c.id = CID_ALLOC;
	c.alloc.name = in[0].stridx;
	c.alloc.expr = p - (struct Piece*)currf->pieces.data;
	return c;
}

// for STRING command, place ascii string of characters in memory at current relative position ending with nul char and create label with name
static struct Command comString(struct Piece in[]){
	struct Command c = {.id = CID_NULL};

	struct Piece* p = in;
	if(exprArrayLen(p) != 2){
		addErrorMessage(formats[CID_STRING]);
		addErrorMessage("first argument given incorrectly");
		return c;
	}
	if(p[0].type != PT_STRING){
		addErrorMessage("string expeceted for string name");
		return c;
	}
	p += 2;
	if(exprArrayLen(p) > -2){
		addErrorMessage(formats[CID_STRING]);
		addErrorMessage("second and final argument given incorrectly");
		return c;
	}
	if(p[0].type != PT_STRING){
		addErrorMessage("string expeceted for string value");
		return c;
	}

	c.id = CID_STRING;
	c.string.name = in[0].stridx;
	c.string.value = p[0].stridx;
	c.string.offset = memIdx;
	memIdx += strlen(stringAt(c.string.value)) + 1;
	return c;
}

// for SET command, set byte value at certain address from given value
static struct Command comSet(struct Piece in[]){
	struct Command c = {.id = CID_NULL};

	struct Piece* p = in;
	if(exprArrayLen(p) < 2){
		addErrorMessage(formats[CID_SET]);
		addErrorMessage("first argument given incorrectly");
		return c;
	}
	p += exprArrayLen(p);
	if(exprArrayLen(p) > -2){
		addErrorMessage(formats[CID_SET]);
		addErrorMessage("second and final argument given incorrectly");
		return c;
	}

	c.id = CID_SET;
	c.set.addr = in - (struct Piece*)currf->pieces.data;
	c.set.value = p - (struct Piece*)currf->pieces.data;
	return c;
}



// takes in pieces from a command line and chooses what function to call
bool commandHandler(struct Piece in[], struct FileData* f){
	if(in[0].type != PT_STRING){
		addErrorMessage("string expected for command name");
		return false;
	}

	// string literal used to call command and function pointer to it
	struct{
		const char* name;
		struct Command (*func)(struct Piece[]);
	} commandArray[] = {
		{"LABEL", comLabel},
		{"L", comLabel},
		{"CONST", comConst},
		{"C", comConst},
		{"STRING", comString},
		{"DROP", comDrop},
		{"ALLOC", comAlloc},
		{"SET", comSet},
	};
	currf = f;
	// attempt to find a matching command name and call command function
	for(int a = 0; a < sizeof(commandArray) / sizeof(commandArray[0]); ++a){
		if(!strcmp(commandArray[a].name, stringAt(in[0].stridx))){
			struct Command res = commandArray[a].func(in + 1);
			if(res.id == CID_NULL){
				addErrorMessage("error processing command \"%s\"", commandArray[a].name);
				return false;
			}
			listAdd(&currf->commands, &res, 1);
			return true;
		}
	}
	addErrorMessage("command name \"%s\" not recognized", stringAt(in[0].stridx));
	return false;
}
