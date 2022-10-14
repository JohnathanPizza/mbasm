#include "error.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

struct List errorList = {.allocStep = 100, .elementSize = sizeof(char)};
int errorLine = 0;
static int msgCount = 0; // current number of messages ready to print

void addErrorMessage(const char* s, ...){
	static char errbuf[200] = {0};
	va_list arg;
	va_start(arg, s);
	if(vsnprintf(errbuf, sizeof(errbuf), s, arg) < 0){
		simpleError("vsnprintf fail");
	}
	listAdd(&errorList, errbuf, strlen(errbuf));
	listAdd(&errorList, "\n", 1);
	++msgCount;
	va_end(arg);
}

void simpleError(const char* s, ...){
	va_list a;
	va_start(a, s);
	vfprintf(stderr, s, a);
	fputc('\n', stderr);
	va_end(a);
	exit(EXIT_FAILURE);
}

void printErrorsExit(void){
	char* pos = listBeg(errorList);
	while(msgCount > 0){
		for(int idx = 0; idx < msgCount - 1; ++idx){
			pos = strchr(pos, '\n');
			if(!pos){
				exit(EXIT_FAILURE);
			}
			++pos;
		}
		if(strchr(pos, '\n')){
			*strchr(pos, '\n') = 0;
		}else{
			exit(EXIT_FAILURE);
		}
		fprintf(stderr, "%s\n", pos);
		pos = listBeg(errorList);
		--msgCount;
	}
	exit(EXIT_FAILURE);
}

void clearErrors(void){
	msgCount = 0;
	listZero(&errorList);
}
