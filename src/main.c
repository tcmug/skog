
#include <wchar.h>
#include <stdlib.h>
#include <locale.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>


#include "types.h"
#include "structs.h"


jammy_compiler *new_jammy_compiler();
void free_jammy_compiler(jammy_compiler *comp);
void jammy_tokenize(jammy_compiler *jc, const wchar_t *str);

int main(int argc, const char *argv[]) {

	wchar_t query[100];
	setlocale(LC_ALL, "en_US.UTF-8");

	jammy_compiler *jc = new_jammy_compiler();
  	while (fgetws(query, 100, stdin)) {
 	 	jammy_tokenize(jc, query);
 	}
 	free_jammy_compiler(jc);

}
