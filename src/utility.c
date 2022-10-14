#include "utility.h"
#include "list.h"
#include <string.h>
#include <stdio.h>
#include "error.h"
#include <errno.h>
#include <stdlib.h>
#include <ctype.h>
#include "ins_values.h"

extern struct List instructionList;
extern struct List stringCharsList;
extern struct List pieceList;
extern struct List labelList;
extern unsigned char* memImage;
extern size_t memIdx;

// parse a string representing an integer and return it
int strToInt(const char in[], int len){
	if(len <= 0){
		return 0;
	}
	int base = 10; // decimal is default base
	if(in[0] == '0' && len > 1){
		// 0X is hex, 0B is bin
		if(in[1] == 'B' || in[1] == 'X'){
			base = (in[1] == 'B' ? 2 : 16);
			in += 2;
			len -= 2;
		}else{ // 0N is octal
			base = 8;
			in += 1;
			len -= 1;
		}
	}
	int value = 0;
	for(int idx = 0; idx < len; ++idx){
		value *= base;
		int charValue = base;
		char c = in[idx];
		if(c >= '0' && c <= '9'){
			charValue = c - '0';
		}else if(c >= 'A' && c <= 'F'){
			charValue = c - 'A' + 10;
		}
		// fail if digit not valid for base
		if(charValue >= base){
			return -1;
		}
		value += charValue;
	}
	return value;
}

// evaluate an expression from an array of pieces starting at p and store the result in res, return if it was successful
bool evalExpression(struct Piece p[], int* res){
	struct List valueList = listNew(sizeof(int), 10);
	int result = 0;
	for(; !IS_EXPR_END(p->type); ++p){
		switch(p->type){
			case PT_STRING:
				// is a label, find a matching name and use it if it is a defined label
				;
				bool found = false;
				for(struct Label* l = listBeg(labelList); l != listEnd(labelList); ++l){
					if(!strcmp(ESTR(l->name), ESTR(p->stridx))){
						if(l->type == LT_DEFINED){
							found = true;
							listAdd(&valueList, &l->value, 1);
						}
						break;
					}
				}
				if(!found){
					addErrorMessage("string \"%s\" did not match any defined labels", ESTR(p->stridx));
					return false;
				}
				break;
			case PT_INTEGER:
				listAdd(&valueList, &p->integer, 1);
				break;
			case PT_ADD:
			case PT_SUB:
			case PT_MUL:
			case PT_DIV:
				// do the math operation
				;
				int* v = listAt(valueList, 0);
				for(size_t a = 0; a < valueList.elementCount; ++a){
					switch(p->type){
						case PT_ADD:
							result += v[a];
							break;
						case PT_SUB:
							result -= v[a];
							break;
						case PT_MUL:
							result *= v[a];
							break;
						case PT_DIV:
							result /= v[a];
							break;
					}
				}
				valueList.elementCount = 0;
				break;
			default:
				addErrorMessage("piece type not good for evaluation sequence");
				return false;
		}
	}
	int* v = listAt(valueList, 0);
	for(size_t a = 0; a < valueList.elementCount; ++a){
		result += v[a];
	}
	*res = result;
	listZero(&valueList);
	return true;
}

int exprArrayLen(const struct Piece p[]){
	int ct = 1;
	while(!IS_EXPR_END(p->type)){
		++p;
		++ct;
	}
	if(ct == 0) ct = 1;
	return p->type == PT_LINE ? -ct : ct;
}

char* printExpr(struct Piece p[]){
	static char exprbuf[200] = {0};
	static char formatbuf[100] = {0};
	for(; !IS_EXPR_END(p->type); ++p){
		switch(p->type){
			case PT_STRING:
				strcat(exprbuf, ESTR(p->stridx));
				strcat(exprbuf, " ");
				break;
			case PT_INTEGER:
				sprintf(formatbuf, "%d ", p->integer);
				strcat(exprbuf, formatbuf);
				break;
			default:
				// symbol piece, puts the ascii character
				formatbuf[1] = 0;
				formatbuf[0] = p->type;
				strcat(exprbuf, formatbuf);
				strcat(exprbuf, " ");
				break;
		}
	}
	return exprbuf;
}

// create a chain of pieces from input text and use the string "symbols" as symbol characters
void createPiecesFromFile(const char filename[]){
	static const char symbols[] = {
		PT_DOT,
		PT_EXPR_DELIM,
		PT_ADD,
		PT_SUB,
		PT_DIV,
		PT_MUL,
		PT_LITERAL,
		PT_LINE,
		';',
		0
	};

	FILE* file = fopen(filename, "r");
	testError(!file, "error opening file \"%s\": %s", filename, strerror(errno));

	char* line = NULL;
	size_t n = 0;
	while(getline(&line, &n, file) != -1){
		bool inString = false, inLit = false;
		char* c = line;
		char* stringPieceBegin = c;
		while(true){
			// get symbol if current char matches, or set line end if not \n and is ; or \0
			char symbol = 0;
			if(strchr(symbols, *c)){
				symbol = *c;
				if(*c == '\0' || *c == ';'){
					symbol = PT_LINE;
				}else if(*c == PT_LITERAL){
					inLit = true;
					inString = true;
					stringPieceBegin = c + 1;
				}
			}
			
			// literal goes until , or newline
			if(inLit && symbol != PT_LINE && symbol != PT_EXPR_DELIM){
				++c;
				continue;
			}

			// if start of new non ws area, begin new string
			// else if ws or symbol, end current string
			if(!inString && *c != ' ' && *c != '\t'){
				inString = true;
				stringPieceBegin = c;
			}else if(inString && (symbol || *c == ' ' || *c == '\t')){
				inString = false;
				int len = c - stringPieceBegin;

				if(!inLit){
					for(int a = 0; a < len; ++a){
						stringPieceBegin[a] = toupper(stringPieceBegin[a]);
					}
				}

				// add piece to piece chain
				int v;
				struct Piece p;
				if(!inLit && (v = strToInt(stringPieceBegin, len)) >= 0){
					p.type = PT_INTEGER;
					p.integer = v;
				}else{
					p.type = PT_STRING;
					p.stridx = stringCharsList.elementCount;
					listAdd(&stringCharsList, stringPieceBegin, len);
					// end string with nul char
					listAdd(&stringCharsList, &(char){0}, 1);
				}
				listAdd(&pieceList, &p, 1);
				inLit = false;
			}

			// add symbol if found and not a comment symbol and cancel string
			if(symbol){
				inString = false;
				struct Piece p = {.type = symbol};
				listAdd(&pieceList, &p, 1);
				// break on newline to loop to next line
				if(symbol == PT_LINE){
					break;
				}
			}
			++c;
		}
	}
	free(line);
	testError(fclose(file), "error closing file \"%s\": %s", filename, strerror(errno));
}

// 0 = good, else failed at that line
int scanPieces(int pieceStart){
	/* line processing
	 *
	 * command lines:
	 * 	.command_name [args...]
	 * 	begin with '.', must have a name, no limit to args passed
	 *
	 * instruction lines:
	 * 	ins [expr][,flags]
	 * 	N I X Y Z V, indirect immediate xoff yoff forcezp forceval
	 */

	errorLine = 1;
	for(struct Piece* p = listAt(pieceList, pieceStart); p != listEnd(pieceList); ++p){
		// a beginning PT_DOT is a command, a PT_STRING is and instruction, and not a PT_LINE is an error
		if(p->type == PT_DOT){
			// command line, send the line starting at 1 after PT_DOT to PT_LINE
			if(!commandHandler(++p)){
				addErrorMessage("command handler failure");
				return errorLine;
			}
			// go to end of line
			while(p->type != PT_LINE){
				++p;
			}
		}else if(p->type == PT_STRING){
			// instruction line
			struct Instruction i;
			
			if((p = getInsLine(p, &i)) == NULL){
				addErrorMessage("failed to form instruction from line");
				return errorLine;
			}
			listAdd(&instructionList, &i, 1);

			// add instruction to memory
			memImage[memIdx++] = i.opcode;
			if(i.size >= 2){
				memImage[memIdx++] = i.value;
			}
			if(i.size == 3){
				memImage[memIdx++] = i.value >> 8;
			}
		}else if(p->type != PT_LINE){
			addErrorMessage("line not recognized as a command or instruction: must start with an instruction name or \".\"");
			return errorLine;
		}
		++errorLine;
	}
	return 0;
}

// process an instruction line starting at piece pidx and put the reults into out, returning an index to the piece last scanned (end of line piece)
struct Piece* getInsLine(struct Piece p[], struct Instruction* out){
	struct Instruction ins = {.expr = 0, .offset = memIdx, .size = 1};

	// find the instruction name, ex: name of STA 0X8009 is STA
	enum InstructionName insName = IN_NULL;
	for(int idx = 0; insNameStrings[idx]; ++idx){
		if(!strcmp(ESTR(p->stridx), insNameStrings[idx])){
			insName = idx; // enum identifer has value of index of string of enum identifier ([IN_AND] = "AND")
			break;
		}
	}
	if(insName == IN_NULL){
		addErrorMessage("instruction not recognized: %s", ESTR(p->stridx));
		return NULL;
	}

	char insMode[3] = {0};
	bool forceValue = false;
	++p;

	// if line ends or there is a expression delim, then there was no expression, so no value, so no mode flags
	// INC ,X will not work as expected (but syntax is ok) and be the acc mode
	if(IS_EXPR_END(p->type)){
		goto INS_NO_VAL;
	}

	// set after to default indicate there is a value - just for implied instructions
	insMode[0] = 'V';

	// early expression evaluation, can fail and retry later, but this step determines if zeropage ins or not
	int v;
	if(evalExpression(p, &v)){
		ins.value = v;
		if(v <= 0xFF && v >= 0){
			insMode[0] = 'Z';
		}
	}else{
		clearErrors();
		ins.expr = p - (struct Piece*)listBeg(pieceList);
	}

	// skip to after expr
	while(p->type != PT_LINE && p->type != PT_EXPR_DELIM){
		++p;
	}
	if(p->type == PT_EXPR_DELIM){
		++p;
	}

	// find mode flags if any
	int addidx = 1;
	while(p->type != PT_LINE){
		if(p->type == PT_STRING){
			char* s = ESTR(p->stridx);
			for(int idx = 0; s[idx]; ++idx){
				if(strchr("XYIN", s[idx])){
					if(addidx == 3){
						addErrorMessage("too many mode flags: %s", s);
						return NULL;
					}
					insMode[addidx++] = s[idx];
				}else if(strchr("ZV", s[idx])){
					insMode[0] = s[idx];
					forceValue = true;
				}else{
					addErrorMessage("mode flags not recognized: %s", s);
					return NULL;
				}
			}
		}else{
			addErrorMessage("mode flags not string type");
			return NULL;
		}
		++p;
	}

INS_NO_VAL:
	;
	// determine addressing mode from mode flags found
	// if not forced, both value modes are tried
	enum AddressingMode addrMode = AM_NULL;
	for(int a = 0; a < 16; ++a){
		if(opcodes[insName][a] && !memcmp(insMode, flagsList[a], 3)){
			addrMode = a;
			break;
		}
	}
	if(addrMode == AM_NULL && !forceValue){
		// no match and try a different value flag
		insMode[0] = (insMode[0] == 'Z' ? 'V' : 'Z');
		for(int a = 0; a < 16; ++a){
			if(opcodes[insName][a] && !memcmp(insMode, flagsList[a], 3)){
				addrMode = a;
				break;
			}
		}
	}
	if(addrMode == AM_NULL){
		if(insMode[0] == 0) insMode[0] = ' ';
		if(insMode[1] == 0) insMode[1] = ' ';
		if(insMode[2] == 0) insMode[2] = ' ';
		addErrorMessage("could not determine addressing mode from flags: %c%c%c", insMode[0], insMode[1], insMode[2]);
		return NULL;
	}

	// for calculating branch offsets, POS + VAL = TPOS -> VAL = TPOS - POS
	if(addrMode == AM_PCR){
		if(!ins.expr){
			ins.value -= memIdx + 2;
		}else{
			ins.value = memIdx + 2;
		}
	}
	ins.opcode = opcodes[insName][addrMode] - 1;
	if(insMode[0] == 'Z'){
		ins.size = 2;
	}else if(insMode[0] == 'V'){
		ins.size = 3;
	}

	*out = ins;
	return p;
}