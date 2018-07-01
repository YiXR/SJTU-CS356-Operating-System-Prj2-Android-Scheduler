/*About test.c
*In this program, we are trying to change the scheduling for processtest app.
*When we choose SCHED_NORMAL and SCHED_WRR, the priority will be 0.
*When we successfully change the scheduling, we can see the printk information in the kernel.
*/
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include <sched.h>
#include <getopt.h>
#include <ctype.h>
#include <limits.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/syscall.h>

#define SCHED_NORMAL 0
#define SCHED_WRR 6
#define SCHED_RR 2
#define SCHED_FIFO 1

int sched_policy;
int pid;
char *Scheduling[] = {"SCHED_NORMAL","SCHED_FIFO","SCHED_RR","","","","SCHED_WRR"};
int main(int argc, char *argv[])
{
	struct sched_param param;
	printf("%s","Please input the Choice of Scheduling algorithms (0-NORMAL,1-FIFO,2-RR,6-WRR):");
	scanf("%d",&sched_policy);
	printf("Current scheduling algorithms is %s\n",Scheduling[sched_policy]);
	printf("%s","Please input the id of testprocess:");
	scanf("%d",&pid);
	int prev_sched = sched_getscheduler(pid);
	if(sched_policy == 0 || sched_policy == 6)
		param.sched_priority = 0;
	if(sched_policy == 1 || sched_policy == 2)
	{
		int pri;
		printf("%s","Set Process's priority (1-99): ");
		scanf("%d",&pri);
      	if(pri>99 || pri<1) 
      	{ 
            printf("%s\n","sched_setscheduler fail");
            return;
      	}
      param.sched_priority = pri; 
	}
	if ( sched_setscheduler(pid, sched_policy, &param) == -1)
	{
		printf("%s\n","sched_setscheduler fail");
	}
	else
	{
		printf("pre scheduler : %s\n", Scheduling[prev_sched]);
		printf("Cur scheduler : %s\n", Scheduling[sched_policy]);
	}
}

