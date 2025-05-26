#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

int main(){
	pid_t child_pid=fork();
if(child_pid<0){
	perror("fork failed");
}
else if(child_pid==0){
	pid_t cc_pid=fork();
	if(cc_pid==0){
	printf("grand child's parent pid: %d\n",getppid());
	}else{
		wait(NULL);
		printf("child pid: %d\n",getpid());
	}

}else{
	wait(NULL);
	printf("parent's parent pid: %d\nparent pid: %d\n",getppid(),getpid());
}

return 0;
}