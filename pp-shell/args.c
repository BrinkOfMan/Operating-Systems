/*
Ethan Brinkman
written 9/10/2020
most recent update: 9/20/2020

This code is supposed to be used by shell.c.
it will repeat the main arguments it was given.
*/

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]){

    printf("\nPrinting main() arguments:\n\n");

    for(int i = 0; i < argc; i++){
        printf("argv[%d] = %s\n", i, argv[i]);
    }

    printf("\n");

    return(3);
}