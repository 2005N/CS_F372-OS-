#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(){
    int pfds[2];
    char buf[20];

    if(pipe(pfds)==-1){
        perror("error in creating pipe");
        exit(1);
    }

    printf("writing %d\n",pfds[1]);
    write(pfds[1],"test",5);
    printf("reading %d\n",pfds[0]);
    read(pfds[0],buf,5);
    printf("we have read: %s\n",buf);

    return 0;
}