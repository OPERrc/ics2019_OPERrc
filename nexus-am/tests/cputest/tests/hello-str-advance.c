#include "trap.h"

char buf[128];

int main() {
	sprintf(buf, "%s", "Hello world!\n");
	nemu_assert(strcmp(buf, "Hello world!\n") == 0);

	sprintf(buf, "%d + %d = %d\n", 1, 1, 2);
	nemu_assert(strcmp(buf, "1 + 1 = 2\n") == 0);

	sprintf(buf, "%d + %d = %d, %s\n", 23333, 10000, 33333, "correct!");
	nemu_assert(strcmp(buf, "23333 + 10000 = 33333, correct!\n") == 0);

	sprintf(buf, "%s, %s, %s\n", "yes", "no", "fail");
	nemu_assert(strcmp(buf, "yes, no, fail\n") == 0);

	return 0;
}
