#include <stdio.h>
#include <stdlib.h>

void main(){

    int n;

    printf("Please enter an integer: ");

    scanf("%d", &n);

    printf("N \t | N*N\n--------------\n");

    for (int i = 1; i <= n; i+=1){
        printf("%d\t | %d\n", i, i*i);
    }
    

}