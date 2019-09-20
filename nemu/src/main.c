#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "nemu.h"
int init_monitor(int, char *[]);
void ui_mainloop(int);
uint32_t expr(char *, bool *);

uint32_t str2num(char *num) {
	int point = 0;
	uint32_t result = 0;
	while (num[point]) {
		result = result * 10 + num[point] - '0';
		point++;
	}	
	return result;
}

void value_test() {
	FILE *fp = fopen("tools/gen-expr/input", "r");
	assert(fp != NULL);

	char line[1024];
  char *line_read = line;
	fgets(line_read, 1024, fp);
	while (line_read != NULL) {
		char *num = strtok(line_read, " ");
    uint32_t key = str2num(num);

		char *EXPR = line_read + strlen(num) + 1;
		int point = 0;
		while (EXPR[point] != '\n')
			point++;
		EXPR[point] = '\0';

		bool flag = true;
		bool *success = &flag;
		uint32_t answer = expr(EXPR, success);
		printf("key = %d, answer = %d", key, answer);
		if (key == answer) 
			printf("  correct!\n");
		else
			printf("  wrong!\n");

		fgets(line_read, 1024, fp);
	}
	fclose(fp);
}

int main(int argc, char *argv[]) {
  /* Initialize the monitor. */
  int is_batch_mode = init_monitor(argc, argv);
	
	value_test();

  /* Receive commands from user. */
  ui_mainloop(is_batch_mode);

  return 0;
}
