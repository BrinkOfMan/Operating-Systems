#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define MAXTOKS 15 //15 strings to be held
enum status_value{EOF_OR_ERROR = 0, NORMAL = 1, TOO_MANY_TOKENS = 2};

struct strct {

    char **tok;
    int count;
    int status;

};

int read_name(struct strct *stc){

    char *buffer;
    int ltr = 0; //Total letters accounted for
    int beginning = ltr; //Keeps track of the current buffer word beginning
    size_t buffersize; //Size_t is required for getline() (it's an unsigned type)
    size_t characters; //Declaring for later
    stc -> tok = (char **)malloc(MAXTOKS * sizeof(char *)); //Allocating 15 strings (I think)

    printf("Type something: ");

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

                    int N = ltr - beginning;

                    stc -> tok[stc -> count] = (char *)malloc((N + 1) * sizeof(char));
                    //This allocates the exact amount of necessary bytes for the upcoming string

                    for(int i = 0; i < N; i++){
                        stc -> tok[stc -> count][i] = buffer[beginning + i];
                        //Copy each character of this current buffer into the token
                    }

                    stc -> tok[stc -> count][ltr] = '\0'; //Add a nullbyte to the end of the word

                    break; //End this string's loop
                }
                //Increment if not a space, nullbyte, or newline
            }

            stc -> count++;
        }

        stc -> status = NORMAL;
    }

    if(stc -> count == MAXTOKS && buffer[ltr + 1] != '\0'){ //Check if any remaining tokens after the 15th

        free(stc -> tok[MAXTOKS - 1]); //Clear the current last token

        int N = 0;

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

    free(buffer);
    return (stc -> status); //0 for error, 1 for normal

}

int main()
{

    struct strct mystruct;

    printf("Return value of %d from read_name() call\n", read_name(&mystruct));

    if (mystruct.status == 0){
        printf("Error: end of file was encountered. Did you type any characters?\n");
    }

    else if (mystruct.status == 1){
        printf("You typed: ");
        for(int i = 0; i < mystruct.count; i++){
            printf("%s ", mystruct.tok[i]);
        }
        printf("\n");
    }

    else{
        printf("Error: too many words detected in input.\nAll words beyond word %d saved in final token of struct.\n", MAXTOKS);

        printf("You typed: ");
        for(int i = 0; i < mystruct.count; i++){
            printf("%s ", mystruct.tok[i]);
        }
        printf("\n");
    }

    for(int i = 0; i < MAXTOKS; i++){ //Freeing all memory
        free(mystruct.tok[i]);
    }

    return(0);
}