
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include<sys/wait.h>

void func(int signum)
{
    wait(NULL);
}
int main()
{
pid_t p;
p = fork();
if (p==0) //child
{
printf("I am child having PID %d\n",getpid());
exit(0);
}
else //parent
{
sleep(1);
printf("I am parent having PID %d\n",getpid());
}
}