
#include <wchar.h>
#include <stdlib.h>
#include <locale.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

// compiler -> emit -> code
// executer -> stack () -> callbacks

#include "types.h"
#include "structs.h"


void jammy_pop_operator(jammy_compiler *jc) {
	assert(jc->operator_stack_top > 0);
	// Never push "(" to the stack.
	if (jc->operators[jc->operator_stack_top - 1].data[0] != L'(') {
		memcpy(
			&jc->result[jc->result_stack_top++],
			&jc->operators[jc->operator_stack_top - 1],
			sizeof(operator)
		);
	}
	jc->operator_stack_top--;
}


void token_operator(jammy_compiler *jc, const jammy_token *jt, const wchar_t *raw, int length) {

	if (length > TOKEN_MAX_LENGTH) {
		wprintf(L"Token is too long\n");
		return;
	}

	// Pop operator tokens which have higher precedence.
	while (jc->operator_stack_top > 0
		&& jc->operators[jc->operator_stack_top - 1].precedence >= jt->precedence) {
		jammy_pop_operator(jc);
	}

	// Push to stack
	memcpy(jc->operators[jc->operator_stack_top].data, raw, length * sizeof(wchar_t));
	jc->operators[jc->operator_stack_top].type = TYPE_OPERATOR;
	jc->operators[jc->operator_stack_top].data[length] = 0;
	jc->operators[jc->operator_stack_top].precedence = jt->precedence;
	jc->operators[jc->operator_stack_top].token = jt;

	jc->operator_stack_top++;
}



void token_popback(jammy_compiler *jc, const jammy_token *jt, const wchar_t *raw, int length) {

	if (length > TOKEN_MAX_LENGTH) {
		wprintf(L"Token is too long\n");
		return;
	}

	while (jc->operator_stack_top > 0
		&& wcscmp(jc->operators[jc->operator_stack_top - 1].data, L"(") != 0) {
		jammy_pop_operator(jc);
	}

}


void token_operand(jammy_compiler *jc, const jammy_token *jt, const wchar_t *raw, int length) {
	if (length > TOKEN_MAX_LENGTH) {
		wprintf(L"Token is too long\n");
		return;
	}
	memcpy(jc->result[jc->result_stack_top].data, raw, length * sizeof(wchar_t));
	jc->result[jc->result_stack_top].type = TYPE_OPERAND;
	jc->result[jc->result_stack_top].data[length] = 0;
	jc->result[jc->result_stack_top].token = jt;
	jc->result_stack_top++;
}


jammy_stack js;

void jammy_dump(jammy_compiler *jc) {
	int i;
	wchar_t *end;
	js.top = 0;
	for (i = 0; i < jc->result_stack_top; i++) {
		if (jc->result[i].token->operation) {
			jc->result[i].token->operation(&js, &jc->result[i]);
		}
		else {
			wprintf(L"no op %ls %i\n", jc->result[i].data, (int)jc->result[i].token->precedence);
		}
	}

	wprintf(L"RESULT: %3.3f\n", stack_get_real(&js, -1));

	jc->operator_stack_top = 0;
	jc->result_stack_top = 0;
}


void operation_div(jammy_stack *st, operand *o) {
	double real1 = stack_get_real(st, -1);
	double real2 = stack_get_real(st, -2);
	stack_pop(st);
	stack_pop(st);
	stack_push_real(st, real2 / real1);
}

void operation_mul(jammy_stack *st, operand *o) {
	double real1 = stack_get_real(st, -1);
	double real2 = stack_get_real(st, -2);
	stack_pop(st);
	stack_pop(st);
	stack_push_real(st, real2 * real1);
}
void operation_add(jammy_stack *st, operand *o) {
	double real1 = stack_get_real(st, -1);
	double real2 = stack_get_real(st, -2);
	stack_pop(st);
	stack_pop(st);
	stack_push_real(st, real2 + real1);
}

void operation_sub(jammy_stack *st, operand *o) {
	double real1 = stack_get_real(st, -1);
	double real2 = stack_get_real(st, -2);
	stack_pop(st);
	stack_pop(st);
	stack_push_real(st, real2 - real1);
}

void operation_assign(jammy_stack *st, operand *o) {
	wprintf(L"ASS\n");
}

void operation_push(jammy_stack *st, operand *o) {
	wchar_t *end;
	double real = wcstod(o->data, &end);
	stack_push_real(st, real);
}

#define JT(t, jt, pr, str, token_callback, op_callback) \
	{ (t), (jt), (pr), (str), (sizeof(str)), (token_callback), (op_callback)}

jammy_token tokens[] = {
	JT( 0,    JT_ANY,    0,  L" \t\n\r",   0, 0),
	JT( 1,    JT_SINGLE, 20, L"/",         token_operator, operation_div),
	JT( 2,    JT_SINGLE, 20, L"*",         token_operator, operation_mul),
	JT( 3,    JT_SINGLE, 10, L"+",         token_operator, operation_add),
	JT( 4,    JT_SINGLE, 10, L"-",         token_operator, operation_sub),
	JT( 5,    JT_SINGLE, 30, L"(",         token_operator, 0),
	JT( 6,    JT_SINGLE, 30, L")",         token_popback,  0),
	JT( 7,    JT_SINGLE, 0,  L"=",         token_operator, operation_assign),
	JT( 1000, JT_ANY,    0,  0,            token_operand,  operation_push),
};


jammy_token token_eol = {
	-1, 0, 0, 0
};

jammy_token token_single = {
	1001, 0, 0, 0
};


jammy_token *determine_token_type(const wchar_t *at) {
	int i = 0;
	if (*at == 0) {
		return &token_eol;
	}
	while (tokens[i].characters != 0) {
		if (wcschr(tokens[i].characters, *at)) {
			return &tokens[i];
		}
		i++;
	}
	return &tokens[i];
}




void jammy_tokenize(jammy_compiler *jc, const wchar_t *str) {

	jammy_token *current = &tokens[0];
	jammy_token *scanned_type = &tokens[0];

	const wchar_t *start = str;
	const wchar_t *at = str;

	// Scan for tokens.
	while (current->type != -1) {
		scanned_type = determine_token_type(at);
		if (current->type != scanned_type->type || current->single) {
			// If the token type has a callback, call it.
			if (current->callback) {
				current->callback(jc, current, start, at - start);
			}
			start = at;
			current = scanned_type;
		}
		at++;
	}

	// Pop remaining operators to the result stack.
	while (jc->operator_stack_top > 0) {
		jammy_pop_operator(jc);
	}

	jammy_dump(jc);

}



jammy_compiler *new_jammy_compiler() {
	jammy_compiler *comp;
	comp = (jammy_compiler *)malloc(sizeof(jammy_compiler));
	comp->operator_stack_top = 0;
	comp->result_stack_top = 0;
	return comp;
}

void free_jammy_compiler(jammy_compiler *comp) {
	free(comp);
}

