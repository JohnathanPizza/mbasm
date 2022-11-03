#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <getopt.h>
#include "types.h"
#include "utility.h"
#include "list.h"
#include "ins_values.h"
#include "error.h"
#include "stringmanip.h"
#include "commandeval.h"

#define BASE 0x8000
#define EEPROM_IMAGE_SIZE 0x8000

static const char* outputName = "out.mb";
struct List stringCharsList;
unsigned char* memImage;
size_t memIdx = 0;
struct FileData* filesArray;

struct List setCommands;
struct{
	bool verbose;
	bool list;
} static programFlags = {0};

static void processArgs(int argc, char* argv[]);
static void printVerbose(void);

// get each file
// per file:
// get pieces
// analyze pieces
// make arrays of found labels, constants, and unsolved expressions
// compare labels and constants to find dupes
// eval expressions and sub in
// assemble instructions and add to image

int fssize;

int main(int argc, char* argv[]){
	// little endian check
	unsigned n = 1;
	testError(((char*)&n)[0] != 1, "little endian check failed");

	// allocate output buffer for data
	testError((memImage = calloc(EEPROM_IMAGE_SIZE, 1)) == NULL, "eeprom image buffer alloc fail (%d bytes)", EEPROM_IMAGE_SIZE);
	stringCharsList = listNew(1, 1000);

	processArgs(argc, argv);
	// argc total
	// index of optind is element number optind + 1
	// argc - (optind + 1) - 1 = argc - optind
	fssize = argc - optind;
	filesArray = malloc(sizeof(struct FileData) * fssize);
	for(int a = 0; a < argc - optind; ++a){
		filesArray[a] = newFileData(argv[a + optind]);
		createPieces(filesArray + a);
		int errorLine = scanPieces(filesArray + a);
		if(errorLine){
			addErrorMessage("from file \"%s\" on line %d", filesArray[a].name, errorLine);
			printErrorsExit();
		}
	}

	// evaluate commands
	setCommands = listNew(sizeof(int) * 2, 50);
	for(int fn = 0; fn < fssize; ++fn){
		struct FileData* cf = filesArray + fn;
		int complete = 0;
		while(complete != cf->commands.elementCount){
			int res = commandEval(cf);
			testError(res == 0, "commands entered deadlock state, can't continue evaluation");
			complete += res;
		}
	}
	clearErrors();

	// this looks really bad but it isnt
	// for each label from each file, check its string index with every other label from every file
	// error if the string indexes match, duplicate label
	// also adjust label values depending on type and find __START and __INTERRUPT
	struct Label* startLabel = NULL, *intLabel = NULL;

	for(int z = 0; z < fssize; ++z){
		for(int a = 0; a < filesArray[z].labels.elementCount; ++a){
			int csi = ((struct Label*)listAt(filesArray[z].labels, a))->name;
			for(int b = 0; b < fssize; ++b){
				for(int c = 0; c < filesArray[b].labels.elementCount; ++c){
					int i = ((struct Label*)listAt(filesArray[b].labels, c))->name;
					if(i == csi && !(z == b && a == c)){
						simpleError("duplicate label name found \"%s\"\nfrom files \"%s\" and \"%s\"", stringAt(i), filesArray[z].name, filesArray[b].name);
					}
				}
			}
			// adjust label values to be aligned at 0x8000 offset
			struct Label* l = listAt(filesArray[z].labels, a);
			static int allocAddr = 0x200;
			if(l->type == LT_UNDEFINED){
					l->value += BASE;
			}else if(l->type == LT_ALLOC){
				int sz = l->value;
				l->value = allocAddr;
				allocAddr += sz;
			}
			l->type = LT_DEFINED;
			for(struct Label* l = listBeg(filesArray[z].labels); l != listEnd(filesArray[z].labels); ++l){
				char* str = stringAt(l->name);
				if(!strcmp(str, "__START")){
					//testError(startLabel, "multiple start labels found");
					startLabel = l;
				}else if(!strcmp(str, "__INTERRUPT")){
					//testError(intLabel, "multiple interrupt labels found");
					intLabel = l;
				}
			}
		}
	}

	// form instructions fully
	for(int z = 0; z < fssize; ++z){
		for(struct Instruction* i = listBeg(filesArray[z].instructions); i != listEnd(filesArray[z].instructions); ++i){
			// eval expression if needed
			if(!i->expr){
				continue;
			}
			int v;
			// fail if expression not evaluated
			if(!evalExpression(listAt(filesArray[z].pieces, i->expr), &v)){
				addErrorMessage(printExpr(listAt(filesArray[z].pieces, i->expr)));
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
	}


	testError(!startLabel, "no start label");
	testError(!intLabel, "no interrupt label");
	printf("START ADDR: %.4X INTERRUPT ADDR: %.4X\n", startLabel->value, intLabel->value);
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
	/*printf("%zu instructions created\n", instructionList.elementCount);
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
		printf("%s  %X\n", stringAt(lp->name), lp->value);
	}*/
}

static void processArgs(int argc, char* argv[]){
	static const char* helpMessage =
		"mbasm [options] [infiles]\n"
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
