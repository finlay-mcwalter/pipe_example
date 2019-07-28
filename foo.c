
// create an anonymous pipe, fork a child, set the child's
// stdout to the pipe, and have the child printf - the parent
// can see it by reading on the pipe.
//
// Note the fflush, otherwise the printf might not actually be
// pushed out onto the pipe, particularly for a trivial example
// like this.
// 
// WFM 28 July 2019

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void) {
  int     pipefd[2];
  pid_t   childpid;
  
  if (pipe(pipefd) <0){
    perror("pipe");
    exit(EXIT_FAILURE);
  }
  
  if((childpid = fork()) <0) {
    perror("fork");
    exit(EXIT_FAILURE);
  }
  
  if(childpid == 0) {
    // child process
    dup2(pipefd[1], STDOUT_FILENO);
    close(pipefd[0]);
    close(pipefd[1]);
    
    for(unsigned i=0; i<5 ; i++){
      printf("greets from child %d", i);
      fflush(stdout);
      sleep(1);
    }

    close(STDOUT_FILENO); // explicitly close the pipe
    sleep(3); // for demo purposes, we do stuff for a while before we actually exit
    return EXIT_SUCCESS;
  }
  else {
    // parent process
    close(pipefd[1]);
    
    while(1){
      char buf[100];

      int count = read(pipefd[0], buf, sizeof(buf));
      if(count == 0) { // remote end disconnected
	printf("child has closed pipe - quitting\n");
	break; 
      }
      
      printf("received >%s<\n", buf);
    }
  }

  printf ("waiting for child process to finish\n");
  waitpid(childpid, NULL, 0);
  printf ("child has exited\n");
  return EXIT_SUCCESS;
}
