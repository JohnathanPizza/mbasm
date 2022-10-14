#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "list.h"
#include "types.h"
#include "utility.h"
#include "error.h"

extern unsigned char* memImage;
extern size_t memIdx;
extern struct List labelList;
extern struct List commandList;
extern struct List stringCharsList;
extern struct List pieceList;

// these static functions correspond to a callable command from code

// for LABEL command, creates an undefined label with name and relative location
static bool comLabel(struct Piece in[]){
	if(exprArrayLen(in) != -2){
		addErrorMessage("first and final argument given incorrectly - format: .LABEL STRING:LABEL NAME");
		return false;
	}
	if(in[0].type != PT_STRING){
		addErrorMessage("string expeceted for label name");
		return false;
	}

	struct Label l = {.type = LT_UNDEFINED, .value = memIdx, .name = in[0].stridx};
	listAdd(&labelList, &l, 1);
	return true;
}

// for CONST command, creates a defined label with name and value
static bool comConst(struct Piece in[]){
	struct Piece* p = in;
	if(exprArrayLen(p) != 2){
		addErrorMessage("first argument given incorrectly - format: .CONST STRING:CONSTANT NAME, EXPR:CONSTANT VALUE");
		return false;
	}
	if(p[0].type != PT_STRING){
		addErrorMessage("string expeceted for constant name");
		return false;
	}
	p += exprArrayLen(p);
	if(exprArrayLen(p) > -2){
		addErrorMessage("second and final argument given incorrectly - format: .CONST STRING:CONSTANT NAME, EXPR:CONSTANT VALUE");
		return false;
	}

	int v;
	if(!evalExpression(p, &v)){
		struct Command c = {.id = CID_CONST, .constant.name = in - (struct Piece*)pieceList.data, .constant.expr = p - (struct Piece*)pieceList.data};
		listAdd(&commandList, &c, 1);
	}else{
		struct Label l = {.type = LT_DEFINED, .value = v, .name = in[0].stridx};
		listAdd(&labelList, &l, 1);
	}
	return true;
}

// for DROP command, place a byte value at current relative position in memory
static bool comDrop(struct Piece in[]){
	if(exprArrayLen(in) > -2){
		addErrorMessage("first and final argument given incorrectly - format: .DROP EXPR:DROP VALUE");
		return false;
	}

	int v;
	if(!evalExpression(in, &v)){
		struct Command c = {.id = CID_DROP, .drop.offset = memIdx, .drop.expr = in - (struct Piece*)pieceList.data};
		memImage[memIdx++] = 0;
		listAdd(&commandList, &c, 1);
	}else{
		memImage[memIdx++] = v;
	}
	return true;
}

// for ALLOC command, label takes the value of an incremental address given by assembler from a name and byte length
static bool comAlloc(struct Piece in[]){
	struct Piece* p = in;
	if(exprArrayLen(p) != 2){
		addErrorMessage("first argument given incorrectly - format: .ALLOC STRING:LABEL NAME, EXPR:ALLOC SIZE");
		return false;
	}
	if(p[0].type != PT_STRING){
		addErrorMessage("string expeceted for alloc name");
		return false;
	}
	p += exprArrayLen(p);
	if(exprArrayLen(p) > -2){
		addErrorMessage("second and final argument given incorrectly - format: .ALLOC STRING:LABEL NAME, EXPR:ALLOC SIZE");
		return false;
	}
	
	int v;
	if(!evalExpression(p, &v)){
		struct Command c = {.id = CID_ALLOC, .alloc.name = in - (struct Piece*)pieceList.data, .alloc.expr = p - (struct Piece*)pieceList.data};
		listAdd(&commandList, &c, 1);
	}else{
		struct Label l = {.type = LT_ALLOC, .value = v, .name = in[0].stridx};
		listAdd(&labelList, &l, 1);
	}
	return true;
}

// for STRING command, place ascii string of characters in memory at current relative position ending with nul char and create label with name
static bool comString(struct Piece in[]){
	struct Piece* p = in;
	if(exprArrayLen(p) != 2){
		addErrorMessage("first argument given incorrectly - format: .STRING STRING:STRING NAME, STRING:CONTENTS");
		return false;
	}
	if(p[0].type != PT_STRING){
		addErrorMessage("string expeceted for string name");
		return false;
	}
	p += exprArrayLen(p);
	if(exprArrayLen(p) > -2){
		addErrorMessage("second and final argument given incorrectly - format: .STRING STRING:STRING NAME, STRING:CONTENTS");
		return false;
	}
	if(p[0].type != PT_STRING){
		addErrorMessage("string expeceted for string value");
		return false;
	}

	struct Label l = {.type = LT_UNDEFINED, .value = memIdx, .name = in[0].stridx};
	listAdd(&labelList, &l, 1);

	for(int a = 0; a < strlen(ESTR(p[0].stridx)) + 1; ++a){
		memImage[memIdx++] = (ESTR(in[1].stridx))[a];
	}
	memImage[memIdx++] = 0;
	return true;
}

// for SET command, set byte value at certain address from given value
static bool comSet(struct Piece in[]){
	struct Piece* p = in;
	if(exprArrayLen(p) < 2){
		addErrorMessage("first argument given incorrectly - format: .SET EXPR:ADDRESS, EXPR:VALUE");
		return false;
	}
	p += exprArrayLen(p);
	if(exprArrayLen(p) > -2){
		addErrorMessage("second and final argument given incorrectly - format: .SET EXPR:ADDRESS, EXPR:VALUE");
		return false;
	}

	int v;
	int addr;
	if(!evalExpression(in, &addr) || !evalExpression(p, &v)){
		struct Command c = {.id = CID_SET, .set.addr = in - (struct Piece*)pieceList.data, .set.value = p - (struct Piece*)pieceList.data};
		listAdd(&commandList, &c, 1);
	}else{
		memImage[addr % 0x8000] = v;
	}
	return true;
}



// takes in pieces from a command line and chooses what function to call
bool commandHandler(struct Piece in[]){
	if(in[0].type != PT_STRING){
		addErrorMessage("string expected for command name");
		return false;
	}

	// string literal used to call command and function pointer to it
	struct{
		const char* name;
		bool (*func)(struct Piece[]);
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

	// attempt to find a matching command name and call command function
	for(int a = 0; a < sizeof(commandArray) / sizeof(commandArray[0]); ++a){
		if(!strcmp(commandArray[a].name, ESTR(in[0].stridx))){
			if(!commandArray[a].func(in + 1)){
				addErrorMessage("error processing command \"%s\"", commandArray[a].name);
				return false;
			}
			return true;
		}
	}
	addErrorMessage("command name \"%s\" not recognized", ESTR(in[0].stridx));
	return false;
}
