/*
Ethan Brinkman
written 9/10/2020
most recent update: 9/20/2020

The purpose of this code it to act as a standard computer shell,
allowing users to input commands as though they were in a shell similar to /bin/csh.
*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAXTOKS 10 //Max number of tokens to be held in **tok
enum status_value{EOF_OR_ERROR = 0, NORMAL = 1, TOO_MANY_TOKENS = 2}; //Keep track of strct status

struct strct {

    char **tok;
    int count;
    int status;

};

int read_name(struct strct *stc){

    char *buffer = NULL; //This took 5 hours to debug
    int ltr = 0; //Total letters accounted for
    int beginning = ltr; //Keeps track of the current buffer word beginning
    size_t buffersize = 0; //Size_t is required for getline() (it's an unsigned type)
    ssize_t characters; //Return value of getline()
    stc -> tok = (char **)malloc(MAXTOKS * sizeof(char *)); //Allocating 10 char *

    characters = getline(&buffer,&buffersize,stdin); //getline() returns # characters read from input

    if (characters == -1){ //If someone is able to input nothing (somehow), or inputs more than buffer allows

        stc -> status = EOF_OR_ERROR;
    }

    else{

        //Tested by hitting [ctrl + @] to create a nullbyte: program will count nullbytes.
       
        stc -> count = 0;

        while(stc -> count < MAXTOKS && buffer[ltr] != '\0' && buffer[ltr] != '\n'){ //Goes through each string up to max tokens or end of input

            while(buffer[ltr] != '\0' && isspace(buffer[ltr])){ //Test if multiple spaces OR a nullbyte before copying input
                 ltr++;
            }
            //Note that if there's a nullbyte and ONLY a nullbyte this loop will copy it into the first token

            beginning = ltr; //Update current buffer start position

            for(ltr; ltr < characters; ltr++){ //Increment ltr

                if(isspace(buffer[ltr]) || buffer[ltr] == '\0'){ //If current letter is a space, nullbyte, or enter

                    int N = ltr - beginning; //N represents length of current token

                    stc -> tok[stc -> count] = (char *)malloc((N + 1) * sizeof(char));
                    //This allocates the exact amount of necessary bytes for the upcoming string

                    for(int i = 0; i < N; i++){
                        stc -> tok[stc -> count][i] = buffer[beginning + i];
                        //Copy each character of this current buffer into the token
                    }

                    stc -> tok[stc -> count][N] = '\0'; //Add a nullbyte to the end of the word

                    break; //End this string's loop
                }
                //Increment if not a space, nullbyte, or newline
            }

            stc -> count++;
        }

        stc -> status = NORMAL;
    }

    if(stc -> count == MAXTOKS && buffer[ltr + 1] != '\0'){ //Check if any remaining tokens after the 10th

        stc -> tok[stc -> count] = NULL;

        free(stc -> tok[MAXTOKS - 1]); //Clear the current last token
        
        int N = 0; //Represents length of current token

        for(int i = beginning; buffer[i] != '\0'; i++){ //How many letters are left?
            N = i - beginning;
        }

        stc -> tok[MAXTOKS - 1]  = (char *)malloc((N + 1) * sizeof(char));
        //This allocates the exact amount of necessary bytes for the upcoming string

        for(int i = 0; i < N; i++){
            stc -> tok[MAXTOKS - 1][i] = buffer[beginning + i];
            //Copy each character of this current buffer into the final token
        }

        stc -> tok[MAXTOKS - 1][N] = '\0'; //Assign a nullbyte at the end of the final token

        stc -> status = TOO_MANY_TOKENS;
    }

    return (stc -> status); //0 for EOF, 1 for NORMAL, 2 for TOO_MANY_TOKENS

}

void deallocate(struct strct *stc){ //Freeing all memory, "resetting" strct to default states

    for(int i = 0; i < stc -> count - 1; i++){ 
            free(stc -> tok[i]); //Free each individual char * of tok
        }
    free(stc -> tok); //Free the whole char **

    stc -> count = 0;
    stc -> tok = NULL; //"Obliterate any association to a char **" - Rab 2020
}

int main()
{

    struct strct mystruct;
    char buffer[200]; //This will be used in the chdir and pwd commands

    while(1){

        printf("Please enter a command. Input \"help\" to see a list of acceptable inputs.\n");

        read_name(&mystruct);        

        if (mystruct.status == 0){
            printf("Error: end of file was encountered. Did you type any characters?\n");
        }  

        else if (mystruct.status != 1){
            printf("Error: too many words detected in input.\nAll words beyond word %d saved in final token of struct.\n", MAXTOKS);
        }

        mystruct.tok[mystruct.count] = NULL; //Last node (for execve) will be transformed into a null pointer

        char *env[] = {NULL}; //Stolen from the man page on execve


        //Command processing

        //No input

        if(mystruct.tok[0] == NULL) ; //Guard for if user accidentally hits enter before entering a command

        //Help: lists all valid inputs

        else if(!strcmp(mystruct.tok[0], "help") || !strcmp(mystruct.tok[0], "Help")){
            printf("\n");
            printf("Input \"ls\" to list files in current working directory.\n");
            printf("Input \"chdir <path>\" to change directories.\n");
            printf("Input \"pwd\" to print current working directory.\n");
            printf("Input \"clear\" to clear screen.\n");
            printf("Input \"exit\" to exit the shell.\n");
            printf("Input \"help\" to see this output.\n");
            printf("Any other input will be considered as a path to exec.\n");
        }

        //Ls: lists files in current working directory
        //NOTE: This isn't really a |new| command, as I just execve(/bin/ls) 

        else if(!strcmp(mystruct.tok[0], "ls") || !strcmp(mystruct.tok[0], "Ls")){
            
            pid_t forkStatus;

            if ((forkStatus = fork()) < 0){
                printf("fork() failed.\n");
                perror("Error in calling fork");
            }
            if (forkStatus != 0) { //Parent
                wait(NULL);
                printf("\n");
            }        
            else { //Child
                printf("\n");
                execve("/bin/ls", mystruct.tok, env);
                printf("Execve(ls) failed, Child will exit.\n");
                perror("Execve failed"); //Perror will be prepended to the error message.
                _exit(1);
            } 
        }

        //Chdir: changes current working directory

        else if(!strcmp(mystruct.tok[0], "chdir") || !strcmp(mystruct.tok[0], "Chdir")){
            if(mystruct.tok[1] == NULL || mystruct.tok[2] != NULL){
                printf("Usage: chdir <path>\n");
            }
            else{
                chdir(mystruct.tok[1]);
                getcwd(buffer, sizeof(buffer));
                printf("Changed to directory: %s\n", buffer);
            }
        }

        //Pwd: prints current working directory

        else if(!strcmp(mystruct.tok[0], "pwd") || !strcmp(mystruct.tok[0], "Pwd")){ 
            getcwd(buffer, sizeof(buffer));
            printf("%s\n", buffer);
        }

        //Clear: clears screen of all output

        else if(!strcmp(mystruct.tok[0], "clear") || !strcmp(mystruct.tok[0], "Clear")){ 
            system("clear");
        }

        //Exit: exits shell

        else if(!strcmp(mystruct.tok[0], "exit") || !strcmp(mystruct.tok[0], "Exit")){
            printf("\nExiting shell...\n");
            _exit(5);
        }

        //Execve: executes a program

        else{
            pid_t forkStatus;

            printf("\nCalling execve...\n");

            if ((forkStatus = fork()) < 0){
                printf("fork() failed.\n");
                perror("Error in calling fork");
            }
            if (forkStatus != 0) { //Parent
                printf("Parent (pid %d) is waiting.\n", getpid());
                int status;
                wait(&status);
                printf("Parent (pid %d) is exiting.\n", getpid());
            }        
            else { //Child
                printf("Child  (pid %d) is running, processing.\n", getpid());
                execve(mystruct.tok[0], mystruct.tok, env);
                printf("Execve failed, Child will exit.\n");
                perror("Execve failed"); //Perror will be prepended to the error message.
                _exit(1);
            } 
        }

        deallocate(&mystruct); //De-allocate tok array

    } //End of loop
    exit(0);
}