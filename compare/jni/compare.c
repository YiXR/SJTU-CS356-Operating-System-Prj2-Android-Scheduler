/*About compare.c
*In this program, we are trying to compare the performance of different scheduling.
*By creating some threads and doing some calculating, we set the program to different scheduling and print the whole running time.
*Input like : ./compare 1 99 50 means we use SCHED_FIFO and we set the priority to 99, and we create 50 threads.
*Please push it to /data/misc, and it can be excuted by test_compare in groups.
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/sysinfo.h>
#include <time.h>
#include <sched.h>
#include <ctype.h>
#include <string.h>
#include <pthread.h>
#define COUNT 1000000
#define MILLION 1000000L
#define NANOSECOND 1000
#define SCHED_NORMAL 0
#define SCHED_WRR 6
#define SCHED_RR 2
#define SCHED_FIFO 1
char *Scheduling[] = {"SCHED_NORMAL","SCHED_FIFO","SCHED_RR","","","","SCHED_WRR"};

void test_func()
{
    int i = 0;
    unsigned long long result = 0;;

    for(i = 0; i<80000000 ;i++)
    {
        result += 2;
    }

}
void *test()
{
	int i = 0;
	for(i = 0;i<COUNT;i++)
    {
        test_func();
    }
}

int main(int argc,char* argv[])
{
    int i;
    struct sched_param param;
    int ret = 0;
    struct timeval timeStart, timeEnd; 

    double runTime=0;
    if(argc != 4)
    {
        printf("Error input\n");
        return -1;
    }


    int sched_policy = atoi(argv[1]);
    int sched_priority = atoi(argv[2]);
    int num_thread = atoi(argv[3]);

    param.sched_priority = sched_priority;
    if ( sched_setscheduler(getpid(), sched_policy, &param) == -1)
	{
		printf("%s\n","sched_setscheduler fail");
	}
   
    int scheduler = sched_getscheduler(getpid());

    printf("the scheduler of PID(%d) is %s, priority (%d)\n",
            getpid(),Scheduling[scheduler],sched_priority);

    gettimeofday(&timeStart, NULL );

    pthread_t thread[num_thread];
	for (i = 0; i < num_thread; ++i)
	{
		int n;
		n = pthread_create(&thread[i], NULL, test, NULL);
		if(n)
		{
			printf("ERROR; return code is %d\n",n);
			return EXIT_FAILURE;
		}
	}
	for (i = 0; i < num_thread; ++i)
	{
		pthread_join(thread[i], NULL);
	}
    gettimeofday( &timeEnd, NULL ); 
    runTime = (timeEnd.tv_sec - timeStart.tv_sec ) + (double)(timeEnd.tv_usec -timeStart.tv_usec)/1000000;
    printf("PID = %d; priority: %d; RUN TIME is %lf\n",getpid(),sched_priority,runTime);
    return 0;
}


