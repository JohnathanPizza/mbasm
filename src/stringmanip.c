#include "stringmanip.h"
#include "list.h"
#include <string.h>

extern struct List stringCharsList;
// parse a string representing an integer and return it
int strToInt(const char s[], int len){
	if(len <= 0){
		return 0;
	}
	int base = 10; // decimal is default base
	if(s[0] == '0' && len > 1){
		// 0X is hex, 0B is bin
		if(s[1] == 'B' || s[1] == 'X'){
			base = (s[1] == 'B' ? 2 : 16);
			s += 2;
			len -= 2;
		}else{ // 0N is octal
			base = 8;
			s += 1;
			len -= 1;
		}
	}
	int value = 0;
	for(int idx = 0; idx < len; ++idx){
		value *= base;
		int charValue = base;
		char c = s[idx];
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

char* stringAt(int i){
	int ct = 0;
	char* a = listBeg(stringCharsList);
	while(a < (char*)listEnd(stringCharsList)){
		if(ct == i){
			return a;
		}
		a += strlen(a) + 1;
		++ct;
	}
	return NULL;
}

int findString(char* c, int s){
	int idx = 0;
	for(char* a = listBeg(stringCharsList); a < (char*)listEnd(stringCharsList); a += strlen(a) + 1){
		if(!strncmp(a, c, s) && s == strlen(a)){
			return idx;
		}
		idx++;
	}
	return -(idx + 1);
}

int addString(char* c, int s){
	int idx = findString(c, s);
	if(idx < 0){
		static char n = 0;
		listAdd(&stringCharsList, c, s);
		listAdd(&stringCharsList, &n, 1);
		return -idx - 1;
	}else{
		return idx;
	}
}
