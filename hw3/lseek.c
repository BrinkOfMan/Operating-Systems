#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define MAXSTRING 100 /* maximum size for strings */

int main(int argc, char *argv[]){

    char *filename = argv[1];  /* name of file to be multiply opened */
    char *progname = argv[0];  /* name of this program */
    size_t getlinelen = 0;

    if (argc == 1){
        printf("usage:  %s filename \n", progname); 
        exit(1); 
    }

    int ret_val;
    printf("open(%s, O_RDONLY) returned ", filename);
    if ((ret_val = open(filename, O_RDONLY)) < 0){
        printf("error %d\n", ret_val);
        perror("try_open: ");
    }
  else{
    printf("file descriptor %d\n", ret_val);
  }

    printf("Enter commands using the following syntax:  (count < %d)\n",
	 MAXSTRING);

    printf("    read <fd> <count>\n    lseek <fd> <byte #>\n    quit\n");

    while (1) {
        char cmd[MAXSTRING];  /* command entered by user */
        int fd;  /* file descriptor entered by user */
        char buff[MAXSTRING];  /* a string buffer for I/O */
        int ret_val;  /* return value from a system call */

        printf("read lseek quit:  ");
        if ((ret_val = scanf("%s", cmd)) == EOF)
            exit(0);
        /* assert:  one word was read into cmd */

        /***** read command *****/
        if (!strcmp(cmd, "read")) {
            int count;  /* number of chars to read */

            scanf("%d %d", &fd, &count);
            if (count >= MAXSTRING) {
                printf("%d is too many characters -- try reading fewer than %d\n",
                count, MAXSTRING);
                continue;
            }
            /* count is small enough */
            if ((ret_val = read(fd, buff, count)) < 0){
                printf("Error code %d returned\n", ret_val);
            }
            else{
                buff[ret_val] = '\0';
                printf("%d bytes were read:  \"%s\"\n", ret_val, buff);
            }
        } 

        /***** lseek command *****/
        else if(!strcmp(cmd, "lseek")){
            int location; //Where to jump to in the file

            scanf("%d %d", &fd, &location);
            lseek(fd, location, SEEK_SET);
            printf("Position in file set to byte %d.\n");
        }

        /***** quit command *****/
        else if (!strcmp(cmd, "quit")){
            break;
        }
        else{
            printf("Unrecognized command %s\n", cmd);
        }
    }

    exit(0);
}