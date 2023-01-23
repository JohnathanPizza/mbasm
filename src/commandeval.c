#include <stdio.h>
#include "utility.h"
#include "list.h"
#include "stringmanip.h"
#include "types.h"
#include "commandeval.h"
#include <string.h>

extern struct List setCommands;

static int nulleval(struct FileData*, struct Command*){
	return 0;
}

static int dropeval(struct FileData* f, struct Command* c){
	static int v;
	if(evalExpression(listAt(f->pieces, c->drop.expr), &v)){
		memImage[c->drop.offset] = v;
		c->id = CID_NULL;
		return 1;
	}
	return 0;
}

static int drop16eval(struct FileData* f, struct Command* c){
	static int v;
	if(evalExpression(listAt(f->pieces, c->drop16.expr), &v)){
		memImage[c->drop16.offset] = v;
		memImage[c->drop16.offset + 1] = v >> 8;
		c->id = CID_NULL;
		return 1;
	}
	return 0;
}

static int consteval(struct FileData* f, struct Command* c){
	static int v;
	if(evalExpression(listAt(f->pieces, c->constant.expr), &v)){
		struct Label l = {.value = v, .type = LT_DEFINED, .name = c->constant.name};
		listAdd(&f->labels, &l, 1);
		c->id = CID_NULL;
		return 1;
	}
	return 0;
}

static int alloceval(struct FileData* f, struct Command* c){
	static int v;
	if(evalExpression(listAt(f->pieces, c->alloc.expr), &v)){
		struct Label l = {.value = v, .type = LT_ALLOC, .name = c->alloc.name};
		listAdd(&f->labels, &l, 1);
		c->id = CID_NULL;
		return 1;
	}
	return 0;
}

static int labeleval(struct FileData* f, struct Command* c){
	struct Label l = {.value = c->label.addr + 0x8000, .type = LT_DEFINED, .name = c->label.name};
	listAdd(&f->labels, &l, 1);
	c->id = CID_NULL;
	return 1;
}

static int seteval(struct FileData* f, struct Command* c){
	static int v, v2;
	if(evalExpression(listAt(f->pieces, c->set.addr), &v)){
		if(evalExpression(listAt(f->pieces, c->set.value), &v2)){
			listAdd(&setCommands, &(int[2]){v, v2}, 1);
			c->id = CID_NULL;
			return 1;
		}
	}
	return 0;
}

static int stringeval(struct FileData* f, struct Command* c){
	struct Label l = {.value = c->string.offset + 0x8000, .type = LT_DEFINED, .name = c->string.name};
	listAdd(&f->labels, &l, 1);
	for(int idx = 0; idx <= strlen(stringAt(c->string.value)); ++idx){
		memImage[c->string.offset + idx] = stringAt(c->string.value)[idx];
	}
	c->id = CID_NULL;
	return 1;
}

int commandEval(struct FileData* f){
	static int (*evallist[])(struct FileData*, struct Command*) = {
		[CID_NULL] = nulleval,
		[CID_DROP] = dropeval,
		[CID_DROP16] = drop16eval,
		[CID_CONST] = consteval,
		[CID_ALLOC] = alloceval,
		[CID_STRING] = stringeval,
		[CID_LABEL] = labeleval,
		[CID_SET] = seteval
	};
	int ct = 0;
	for(int a = 0; a < f->commands.elementCount; ++a){
		struct Command* c = (struct Command*)f->commands.data + a;
		ct += evallist[c->id](f, c);
	}
	return ct;
}
