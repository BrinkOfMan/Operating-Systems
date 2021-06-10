/*
Ethan Brinkman
Written 10/11/2020
Most recent update: 10/12/2020

The purpose of this code is to request user input of 2 files
Then, the program will append the contents of the 1st file onto the 2nd
*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>

//######################### COPYING CONTENTS FROM SHELL FOR CONSTANT INPUT #########################

#define MAXTOKS 2 //Max number of tokens to be held in **tok
enum status_value{EOF_OR_ERROR = 0, NORMAL = 1, TOO_MANY_TOKENS = 2}; //Keep track of strct status

struct name {

  char **tok;
  int count;
  int status;

};

int read_name(struct name *stc){

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

void deallocate(struct name *stc){ //Freeing all memory, "resetting" strct to default states

  for(int i = 0; i < stc -> count - 1; i++){ 
      free(stc -> tok[i]); //Free each individual char * of tok
  }
  free(stc -> tok); //Free the whole char **

  stc -> count = 0;
  stc -> tok = NULL; //"Obliterate any association to a char **" - Rab 2020
}

//##################################################################################################

int main()
{

  struct name input; //To be used for input
  FILE *pFileR, *pFileA; //File pointers to read and append to
  char *bufferF = NULL, *bufferI = NULL; //Buffers for file and input
  size_t buffLenF = 0, buffLenI = 0; //Buffer lengths for file and input 

  printf("\nPlease input the name of a file you wish to copy and the location to copy it to.\n");
  printf("Simple example: myfile.txt myfileappend.txt\n");
  printf("Example using paths: extra.txt ../hw7/myfile.txt\n");
  
  read_name(&input); //Grab input from user

  if (input.status == 0){
    printf("Error: end of file was encountered. Did you type any characters?\n");
    exit(1);
  }  

  else if (input.status != 1){
    printf("Error: too many words detected in input.\nAll words beyond word %d saved in final token of input.\n", MAXTOKS);
  }

  if ((pFileR = fopen(input.tok[0], "r")) == NULL){ //Open file for reading
    perror ("Error opening read file");
    exit(1);
  }
  if ((pFileA = fopen(input.tok[1], "a")) == NULL){ //Open file for appending
    perror ("Error opening write file");
    exit(1);
  }

  else {
    while (getline(&bufferF, &buffLenF, pFileR) >= 0){ //Read lines until end of file
      fputs(bufferF, pFileA); //Copy those lines (with nullbyte terminations added) into the new file.
    }
    fclose(pFileR); fclose(pFileA); //Close both files
  }

  deallocate(&input); //De-allocate input token array
  exit(0); //Successful exit status
}

