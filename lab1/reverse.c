#include <stdio.h>
#include <stdlib.h> // for exit()

int main(int argc, char **argv, char **envp)
{
	int i = argc; // index into array argv[]
	while (i--) {
		printf("%s ", argv[i]);
	}
	printf("\n");

	exit(argc); // normal exit status would be 0
}

