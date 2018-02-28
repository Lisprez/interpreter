#include <parser.h>
#include <builtins.h>

#include "stack.h"

#include <memory.h>
#include <stdio.h>
//#include <stdlib.h>
#include <string.h>

extern int rand(void);

int s_put(interpreter *it)
{
	char *s = igetarg_string(it, 0);
	printf("%s", s);
	return 0;
}

int s_type(interpreter *it)
{
	variable *v = (variable *)it->stack[0];
	switch (v->valtype) {
	case STRING:
		puts("string");
		break;
	case INTEGER:
		puts("integer");
		break;
	case FLOAT:
		puts("float");
		break;
	case FUNC:
		puts("func");
		break;
	default:
		puts("unknown");
		break;
	}
	return 0;
}

int input(interpreter *it)
{
	variable *v = igetarg(it, 0);
	v->valtype = STRING;
	v->svalue = malloc(128);
	unsigned int unused;
	getline(&v->svalue, &unused, stdin);
	*strchr(v->svalue, '\n') = '\0';
	return 0;
}

int concat(interpreter *it)
{
	variable *v = igetarg(it, 0);
	char *s = igetarg_string(it, 1);
	char *new = malloc(strlen(v->svalue) + strlen(s) + 1);
	strcpy(new, v->svalue);
	strcpy(new + strlen(v->svalue), s);
	new[strlen(v->svalue) + strlen(s)] = 0;
	v->svalue = new;
	return 0;
}

int script_rand(interpreter *it)
{
	static variable v;
	v.valtype = INTEGER;
	unsigned int mod = igetarg_integer(it, 0);
	unsigned int val = rand();
	INT(&v) = val % mod;
	isetstr(&v);
	iret(it, &v);
	return 0;
}

int line(interpreter *it)
{
	(void)it;
	return 0;
}

int main(int argc, char **argv)
{
	interpreter interp;

	if (argc != 2) {
		printf("Usage: %s file\n", argv[0]);
		return -1;
	}

	FILE *fp = fopen(argv[1], "r");
	if (fp == 0) {
		printf("Could not open file: %s\n", argv[1]);
		return -1;
	}

	iinit(&interp);
	inew_cfunc(&interp, "print", s_put);
	inew_cfunc(&interp, "tp", s_type);
	inew_cfunc(&interp, "gets", input);
	inew_cfunc(&interp, "concat", concat);
	inew_cfunc(&interp, "rand", script_rand);
	inew_cfunc(&interp, "line", line);


	char *line = 0;
	unsigned int size;
	int result;
	while (getline(&line, &size, fp) != -1) {
		*strchr(line, '\n') = '\0';
		result = idoline(&interp, line);
		if (result != 0)
			printf("Error: %d\n", result);
	}

	fclose(fp);
	iend(&interp);
	return 0;
}
