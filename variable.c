#include "variable.h"
#include "parser.h"

#include <stdlib.h>
#include <memory.h>
#include <string.h>

extern int atoi(const char *);

extern char *str_undef;
extern char *str_func;

char *fixstring(char *s)
{
	char *n = malloc(strlen(s) + 1);
	int j = 0;
	for (int i = 0; s[i] != '\0'; i++, j++) {
		if (s[i] == '\\') {
			if (s[i + 1] == 'n')
				n[j] = '\n';
			i++;
		} else {
			n[j] = s[i];
		}
	}
	n[j] = '\0';
	return n;
}

variable *vmake(uint8_t fromc, uint8_t valtype, void *value)
{
	variable *v = (variable *)malloc(sizeof(variable));
	v->used = 0;
	v->fromc = fromc;
	v->valtype = valtype;
	v->value = 0;
	v->svalue = 0;
	switch (valtype) {
	case STRING:
		v->svalue = fixstring(value);
		free(value);
		break;
	case INTEGER:
		INT(v) = (int32_t)value;
		isetstr(v);
		break;
	case FUNC:
		v->value = (uint32_t)value;
		v->svalue = str_func;
		break;
	case EXPR:
		v->svalue = value;
		break;
	}
	return v;
}

variable *vmakef(float value)
{
	variable *v = (variable *)malloc(sizeof(variable));
	v->used = 0;
	v->fromc = 0;
	v->valtype = FLOAT;
	FLOAT(v) = value;
	fsetstr(v);
	return v;
}

void fsetstr(variable *f)
{
	if (f->svalue == 0 || f->svalue == str_undef)
		f->svalue = (char *)malloc(32);
	snprintf(f->svalue, 32, "%f", FLOAT(f));
}

void isetstr(variable *i)
{
	if (i->svalue == 0 || i->svalue == str_undef)
		i->svalue = (char *)malloc(32);
	snprintf(i->svalue, 32, "%d", (int)INT(i));
}

variable *itostring(variable *v)
{
	switch (v->valtype) {
	case INTEGER:
		v->valtype = STRING;
		isetstr(v);
		break;
	case FLOAT:
		v->valtype = STRING;
		fsetstr(v);
		break;
	}
	return v;
}

variable *itoint(variable *v)
{
	switch (v->valtype) {
	case STRING:
		v->valtype = INTEGER;
		INT(v) = atoi(v->svalue);
		isetstr(v);
		break;
	case FLOAT:
		v->valtype = INTEGER;
		INT(v) = (int32_t)FLOAT(v);
		isetstr(v);
		break;
	}
	return v;
}

variable *itofloat(variable *v)
{
	switch (v->valtype) {
	case STRING:
		v->valtype = FLOAT;
		FLOAT(v) = strtof(v->svalue, 0);
		fsetstr(v);
		break;
	case INTEGER:
		v->valtype = FLOAT;
		FLOAT(v) = (float)INT(v);
		fsetstr(v);
		break;
	}
	return v;
}

