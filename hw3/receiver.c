/* Server-side use of Berkeley socket calls -- receive one message and print 
   Requires one command line arg:  
     1.  port number to use (on this machine). 
   RAB 3/12 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>

#define MAXBUFF 100

int main(int argc, char **argv) {
  char *prog = argv[0];
  int port;
  int serverd;  /* socket descriptor for receiving new connections */

  if (argc < 2) {
    printf("Usage:  %s port\n", prog);
    return 1;
  }
  port = atoi(argv[1]);

  if ((serverd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    printf("%s ", prog);
    perror("socket()");
    return 1;
  }
  
  struct sockaddr_in sa;
  sa.sin_family = AF_INET;
  sa.sin_port = htons(port);
  sa.sin_addr.s_addr = INADDR_ANY;

  if (bind(serverd, (struct sockaddr*) &sa, sizeof(sa)) < 0) {
    printf("%s ", prog);
    perror("bind()");
    return 1;
  }
  if (listen(serverd, 5) < 0) {
    printf("%s ", prog);
    perror("listen()");
    return 1;
  }

  
  int clientd;  /* socket descriptor for communicating with client */
  struct sockaddr_in ca;
  int size = sizeof(struct sockaddr);

  printf("Waiting for a incoming connection...\n");
  if ((clientd = accept(serverd, (struct sockaddr*) &ca, &size)) < 0) {
    printf("%s ", prog);
    perror("accept()");
    return 1;
  }

  char buff[MAXBUFF];  /* message buffer */
  int ret;  /* return value from a call */
  if ((ret = recv(clientd, buff, MAXBUFF-1, 0)) < 0) {
    printf("%s ", prog);
    perror("recv()");
    return 1;
  }

  buff[ret] = '\0';  // add terminating nullbyte to received array of char
  printf("Received message (%d chars):\n%s\n", ret, buff);

  if ((ret = close(clientd)) < 0) {
    printf("%s ", prog);
    perror("close(clientd)");
    return 1;
  }
  if ((ret = close(serverd)) < 0) {
    printf("%s ", prog);
    perror("close(serverd)");
    return 1;
  }
  return 0;
}
