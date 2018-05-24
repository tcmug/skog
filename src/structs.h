#ifndef JAMMY_STRUCTS_H
#define JAMMY_STRUCTS_H

#define TYPE_OPERAND  (10)
#define TYPE_VARIABLE (11)
#define TYPE_OPERATOR (20)

typedef struct t_details {
    int precedence;
    int left_associative;
    wchar_t data[TOKEN_MAX_LENGTH + 1];
} details;

typedef struct t_operand {
    int type;
    int precedence;
    details detail;
    wchar_t data[TOKEN_MAX_LENGTH + 1];
    const struct t_jammy_token *token;
} operand;

typedef struct t_operator {
    int type;
    int precedence;
    details detail;
    wchar_t data[TOKEN_MAX_LENGTH + 1];
    const struct t_jammy_token *token;
} operator;

typedef struct t_jammy_compiler {
    int operator_stack_top;
    operator operators[STACK_SIZE];
    int result_stack_top;
    operand result[STACK_SIZE];
} jammy_compiler;


#define JT_ANY    (0)
#define JT_SINGLE (1)
#define JT_WHOLE  (2)

typedef struct t_jammy_token {
    int type;
    int single;
    int precedence;
    const wchar_t *characters;
    int characters_length;
    void (*callback)(jammy_compiler *jc, const struct t_jammy_token *jt, const wchar_t *raw, int length);
    void (*operation)(jammy_stack *, operand *);
} jammy_token;

typedef void (*t_token_callback)(jammy_compiler *jc, const jammy_token *jt, const wchar_t *raw, int length);

#endif
