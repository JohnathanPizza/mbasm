#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <getopt.h>
#include "utility.h"
#include "list.h"
#include "types.h"
#include "ins_values.h"
#include "error.h"

#define BASE 0x8000
#define PROGRAM_NAME "mbasm"
#define EEPROM_IMAGE_SIZE 0x8000

struct List labelList;
struct List commandList;
struct List instructionList;
struct List pieceList;
struct List stringCharsList;
unsigned char* memImage;
size_t memIdx = 0;
const char* outputName = "out.mb";

struct{
	bool verbose;
	bool list;
} static programFlags = {0};

static void processArgs(int argc, char* argv[]);
static void printVerbose(void);

int main(int argc, char* argv[]){
	// little endian check
	unsigned n = 1;
	testError(((char*)&n)[0] != 1, "little endian check failed");

	// allocate output buffer for data
	testError(!(memImage = calloc(EEPROM_IMAGE_SIZE, 1)), "eeprom image buffer alloc fail (%d)", EEPROM_IMAGE_SIZE);
	
	// inits
	labelList = listNew(sizeof(struct Label), 50);
	commandList = listNew(sizeof(struct Command), 50);
	instructionList = listNew(sizeof(struct Instruction), 100);
	stringCharsList = listNew(1, 100);
	pieceList = listNew(sizeof(struct Piece), 100);

	processArgs(argc, argv);

	// address label, takes value of current offset in memory where instruction/command is started
	listAdd(&labelList, &(struct Label){.name = stringCharsList.elementCount, .type = LT_DEFINED}, 1);
	listAdd(&stringCharsList, "__A", strlen("__A") + 1);

	for(int a = optind; a < argc; ++a){
		int pl = pieceList.elementCount;
		createPiecesFromFile(argv[a]);
		int scanres = scanPieces(pl);
		if(scanres){
			addErrorMessage("from file \"%s\" on line %d", argv[a], scanres);
			printErrorsExit();
		}
	}

	// evaluate commands
	// make function
	struct List setCommands = listNew(sizeof(int) * 2, 50);
	int complete = 0;
	while(complete != commandList.elementCount){
		int change = complete;
		for(struct Command* c = listBeg(commandList); c != listEnd(commandList); ++c){
			int v;
			((struct Label*)listAt(labelList, 0))->value = 0;
			switch(c->id){
				case CID_DROP:
					// setting __A
					((struct Label*)listAt(labelList, 0))->value = c->drop.offset + BASE;
					if(evalExpression(listAt(pieceList, c->drop.expr), &v)){
						memImage[c->drop.offset] = v;
						complete++;
						c->id = CID_NULL;
					}
					break;
				case CID_CONST:
					if(evalExpression(listAt(pieceList, c->constant.expr), &v)){
						struct Piece* name = listAt(pieceList, c->constant.name);
						struct Label l = {.value = v, .type = LT_DEFINED, .name = name->stridx};
						listAdd(&labelList, &l, 1);
						complete++;
						c->id = CID_NULL;
					}
					break;
				case CID_ALLOC:
					if(evalExpression(listAt(pieceList, c->alloc.expr), &v)){
						struct Piece* name = listAt(pieceList, c->alloc.name);
						struct Label l = {.value = v, .type = LT_ALLOC, .name = name->stridx};
						listAdd(&labelList, &l, 1);
						complete++;
						c->id = CID_NULL;
					}
					break;
				case CID_SET:
					if(evalExpression(listAt(pieceList, c->set.addr), &v)){
						int v2;
						if(evalExpression(listAt(pieceList, c->set.value), &v2)){
							listAdd(&setCommands, &(int[2]){v, v2}, 1);
							//memImage[v % 0x8000] = v2;
							complete++;
							c->id = CID_NULL;
						}
					}
					break;
				case CID_NULL:
					break;
			}
		}
		testError(change == complete, "not all commands could be processed");
	}
	clearErrors();

	// adjust label values to be aligned at 0x8000 offset
	for(struct Label* l = listBeg(labelList); l != listEnd(labelList); ++l){
		static int allocAddr = 0x200;
		if(l->type == LT_UNDEFINED){
			l->value += BASE;
		}
		if(l->type == LT_ALLOC){
			int sz = l->value;
			l->value = allocAddr;
			allocAddr += sz;
		}
		l->type = LT_DEFINED;
	}

	// form instructions fully
	for(struct Instruction* i = listBeg(instructionList); i != listEnd(instructionList); ++i){
		// setting __A
		((struct Label*)listAt(labelList, 0))->value = i->offset + BASE;

		// eval expression if needed
		if(!i->expr){
			continue;
		}
		int v;
		// fail if expression not evaluated
		if(!evalExpression(listAt(pieceList, i->expr), &v)){
			addErrorMessage(printExpr(listAt(pieceList, i->expr)));
			addErrorMessage("failed to evaluate expression:");
			printErrorsExit();
		}
		i->value = v - i->value; // special for branch instructions
		if(i->size >= 2){
			memImage[i->offset + 1] = i->value;
		}
		if(i->size == 3){
			memImage[i->offset + 2] = i->value >> 8;
		}
	}

	// ensuring start and interrupt labels exists
	struct Label* startLabel = NULL, *intLabel = NULL;

	for(size_t a = 0; a < labelList.elementCount; ++a){
		struct Label* l = listAt(labelList, a);
		char* str = ESTR(l->name);
		if(!strcmp(str, "__START")){
			testError(startLabel, "multiple start labels found");
			startLabel = l;
		}else if(!strcmp(str, "__INTERRUPT")){
			testError(intLabel, "multiple interrupt labels found");
			intLabel = l;
		}
		for(size_t b = a + 1; b < labelList.elementCount; ++b){
			testError(!strcmp(str, ESTR(((struct Label*)listAt(labelList, b))->name)), "duplicate label \"%s\"", str);
		}
	}

	printf("START ADDR: %.4X INTERRUPT ADDR: %.4X\n", startLabel->value, intLabel->value);
	testError(!startLabel, "no start label");
	testError(!intLabel, "no interrupt label");
	memImage[0x7FFC] = startLabel->value;
	memImage[0x7FFD] = startLabel->value >> 8;
	memImage[0x7FFE] = intLabel->value;
	memImage[0x7FFF] = intLabel->value >> 8;

	// do set commands last over everything
	for(int* p = listBeg(setCommands); p != listEnd(setCommands); p += 2){
		memImage[p[0] % 0x8000] = p[1];
	}
	listZero(&setCommands);

	// write final output
	FILE* f = fopen(outputName, "wb");
	testError(!f, "%s fopen: %s", __func__, strerror(errno));
	testError(fwrite(memImage, 1, EEPROM_IMAGE_SIZE, f) != EEPROM_IMAGE_SIZE, "out file write failure");
	testError(fclose(f), "%s fclose: %s", __func__, strerror(errno));

	if(programFlags.verbose){
		printVerbose();
	}

	if(programFlags.list){
		for(size_t idx = 0; idx < EEPROM_IMAGE_SIZE; ++idx){
			printf("0x%.2X,", memImage[idx]);
		}
		puts("");
	}

	return EXIT_SUCCESS;
}





static void printVerbose(void){
	printf("%zu instructions created\n", instructionList.elementCount);
	printf("instructions:\nOP   ADDR   VALUE\n");
	for(struct Instruction* i = listBeg(instructionList); i != listEnd(instructionList); ++i){
		printf("%.2X   %.4X   ", i->opcode, i->offset);
		if(i->size == 2){
			printf("%.2X\n", i->value & 0xFF);
		}else if(i->size == 3){
			printf("%.4X\n", i->value);
		}else{
			puts("----");
		}
	}
	printf("%zu labels created\nlabels:\nNAME  VALUE\n", labelList.elementCount);
	for(struct Label* lp = listBeg(labelList); lp != listEnd(labelList); ++lp){
		printf("%s  %X\n", ESTR(lp->name), lp->value);
	}
}

static void processArgs(int argc, char* argv[]){
	static const char* helpMessage =
		PROGRAM_NAME " [options] [infiles]\n"
		"options:\n"
		"-v / --version, print information about symbols and data\n"
		"-h / --help, print this info\n"
		"-o name / --out name, set the name of the output file - default is \"out.mb\"\n"
		"-l / --list, print a list of comma separated hex values of the code\n";

	static struct option longOptions[] = {
		{.name = "verbose", .has_arg = 0, .flag = NULL, .val = 'v'},
		{.name = "help", .has_arg = 0, .flag = NULL, .val = 'h'},
		{.name = "out", .has_arg = 1, .flag = NULL, .val = 'o'},
		{.name = "list", .has_arg = 0, .flag = NULL, .val = 'l'},
		{0, 0, 0, 0},
	};
	
	// quit if no arguments at all
	if(argc == 1){
		printf("%s", helpMessage);
		exit(EXIT_SUCCESS);
	}

	// go through args
	int o;
	while((o = getopt_long(argc, argv, "lvho:", longOptions, NULL)) != -1){
		switch(o){
			case 'v':
				programFlags.verbose = true;
				break;
			case 'o':
				outputName = optarg;
				break;
			case 'l':
				programFlags.list = true;
				break;
			case 'h':
			default:
				printf("%s", helpMessage);
				exit(EXIT_SUCCESS);
		}
	}

	// exit if no infiles / only args given
	if(optind == argc){
		exit(EXIT_SUCCESS);
	}
}
