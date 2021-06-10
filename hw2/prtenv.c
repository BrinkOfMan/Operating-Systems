#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv, char **envp)
{
    int i = 0;
    while (envp[i] != NULL) {
        printf("%s\n", envp[i++]);
    }
    printf("\n");
    exit(argc); /* normal exit status would be 0 */
}