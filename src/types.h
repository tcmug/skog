#ifndef JAMMY_TYPES_H
#define JAMMY_TYPES_H

#include <wchar.h>

#define TOKEN_MAX_LENGTH (512)
#define STACK_SIZE (512)

#define TYPE_REAL    (1)
#define TYPE_INTEGER (2)
#define TYPE_CHAR    (3)
#define TYPE_STRING  (4)

typedef struct t_jammy_stack_item {
    int type;
	union {
		double real;
		int integer;
		wchar_t character;
		wchar_t *string;
	};
} jammy_stack_item;

typedef struct t_jammy_stack {
    int top;
    jammy_stack_item data[STACK_SIZE];
} jammy_stack;


void stack_pop(jammy_stack *st);
void stack_push_real(jammy_stack *st, double number);
double stack_get_real(jammy_stack *st, int offset);
int stack_get_integer(jammy_stack *st, int offset);

#endif
