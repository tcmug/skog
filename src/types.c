
#include "types.h"

#include <assert.h>


void stack_pop(jammy_stack *st) {
	assert(st->top > 0);
	st->top--;
}

void stack_push_real(jammy_stack *st, double number) {
	assert(st->top < STACK_SIZE);
	st->data[st->top].type = TYPE_REAL;
	st->data[st->top].real = number;
	st->top++;
}

double stack_get_real(jammy_stack *st, int offset) {
	assert(st->data[st->top + offset].type == TYPE_REAL);
	return st->data[st->top + offset].real;
}

int stack_get_integer(jammy_stack *st, int offset) {
	assert(st->data[st->top + offset].type == TYPE_INTEGER);
	return st->data[st->top + offset].real;
}
