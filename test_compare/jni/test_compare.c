/*About test_compare.c
*Please put compare program into /data/misc.
*In this program, we are trying to execute ./data/misc/compare with 3 groups together.
*Each group has different number of threads.
*For each group, we have different scheduling with different priority.
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
#include <sys/wait.h>

int main()
{
    int i;
    long int begin_time;
    long int end_time;
    sleep(1);
    printf("\nGROUP 1 Thread = 10\n");
    system("./data/misc/compare 0 0 10");
    sleep(1);
    system("./data/misc/compare 1 1 10");
    sleep(1);
    system("./data/misc/compare 1 50 10");
    sleep(1);
    system("./data/misc/compare 1 99 10");
    sleep(1);
    system("./data/misc/compare 2 1 10");
    sleep(1);
    system("./data/misc/compare 2 50 10");
    sleep(1);
    system("./data/misc/compare 2 99 10");
    sleep(1);
    printf("GROUP 2 Thread = 500\n");
    system("./data/misc/compare 0 0 500");
    sleep(1);
    system("./data/misc/compare 1 1 500");
    sleep(1);
    system("./data/misc/compare 1 50 500");
    sleep(1);
    system("./data/misc/compare 1 99 500");
    sleep(1);
    system("./data/misc/compare 2 1 500");
    sleep(1);
    system("./data/misc/compare 2 50 500");
    sleep(1);
    system("./data/misc/compare 2 99 500");
    sleep(1);
    printf("GROUP 3 Thread = 1000\n");
    system("./data/misc/compare 0 0 1000");
    sleep(1);
    system("./data/misc/compare 1 1 1000");
    sleep(1);
    system("./data/misc/compare 1 50 1000");
    sleep(1);
    system("./data/misc/compare 1 99 1000");
    sleep(1);
    system("./data/misc/compare 2 1 1000");
    sleep(1);
    system("./data/misc/compare 2 50 1000");
    sleep(1);
    system("./data/misc/compare 2 99 1000");
}
