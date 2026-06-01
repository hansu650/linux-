#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include <sched.h>
#include <sys/types.h>

#define SAMPLE_COUNT 800  //设置800个抽样点
#define INCREAMENT 0.0025 //增量为0.0025=2/800
#define SAMPLE_TIME 25    //设置每个抽样点的时间片


double GetSysTime()       
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec*1000+tv.tv_usec*1.0/1000);
}

int GetCpuNums()
{
    return (int)sysconf(_SC_NPROCESSORS_ONLN);
}

void *thread_cpu1(void*arg);
void *thread_cpu2(void*arg);//线程函数2，原理同线程函数1

int main()//主函数
{
    //int cpu_num = 0;
    //cpu_num  = GetCpuNums();
    //printf("The number of cpu is %d\n", cpu_num);

    pthread_t t1;
    pthread_t t2;
    pthread_attr_t attr1;
    pthread_attr_t attr2;

    pthread_attr_init(&attr1);
    pthread_attr_init(&attr2);

    if (0!=pthread_create(&t1, &attr1, thread_cpu1, NULL))//创建线程1
    {
        printf("error:Create thread 1\n");
        return 0;
    }

    if (0!=pthread_create(&t2, &attr2, thread_cpu2, NULL))//创建线程2
    {
        printf("error:Create thread 2\n");
        return 0;
    }

    cpu_set_t cpu_info;
    CPU_ZERO(&cpu_info);
    CPU_SET(0, &cpu_info);
    if (0!=pthread_setaffinity_np(t1, sizeof(cpu_set_t), &cpu_info))
    {
        printf("error:Thread 1 set affinity failed.\n");
    }

    CPU_ZERO(&cpu_info);
    CPU_SET(1, &cpu_info);
    if (0!=pthread_setaffinity_np(t2, sizeof(cpu_set_t), &cpu_info))
    {
        printf("error:Thread 2 set affinity failed.\n");
    }

    pthread_join(t1, NULL);//释放线程t1
    pthread_join(t2, NULL);//释放线程t2
    return 0;
}

void *thread_cpu1(void*arg)          //线程函数1
{
    double busy_cache[SAMPLE_COUNT]; //CPU占用时间，缓存一个周期的计算结果
    double idle_cache[SAMPLE_COUNT]; //CPU空闲时间

    double radian = 0.0;             //增量从0开始，最大值到2=SAMPLE_COUNT*INCREAMENT

    double start_time;               //开始时间

    double temp;
    int i;
    //一个周期内为分段函数分为四段
    for(i=0; i<SAMPLE_COUNT/4; i++)
    {
        temp = (double)(-8.0*(radian-0.25)*(radian-0.25)+1);
        busy_cache[i] = (double)(SAMPLE_TIME*temp);
        idle_cache[i] = (double)(SAMPLE_TIME - busy_cache[i]);
        radian += INCREAMENT;
    }

    for(i=SAMPLE_COUNT/4; i<SAMPLE_COUNT/2; i++)
    {
        temp = (double)(-8.0*(radian-0.75)*(radian-0.75)+1);
        busy_cache[i] = (double)(SAMPLE_TIME*temp);
        idle_cache[i] = (double)(SAMPLE_TIME - busy_cache[i]);
        radian += INCREAMENT;
    }

    for(i= SAMPLE_COUNT/2; i < 3*SAMPLE_COUNT/4; i++)
    {
        temp = (double)(0.667*(radian-2)*(radian-2)-0.167);
        busy_cache[i] = (double)(SAMPLE_TIME*temp);
        idle_cache[i] = (double)(SAMPLE_TIME - busy_cache[i]);
        radian += INCREAMENT;
    }

    for(i= 3*SAMPLE_COUNT/4; i < SAMPLE_COUNT; i++)
    {
        temp = (double)(0.667*(radian-1)*(radian-1)-0.167);
        busy_cache[i] = (double)(SAMPLE_TIME*temp);
        idle_cache[i] = (double)(SAMPLE_TIME - busy_cache[i]);
        radian += INCREAMENT;
    }

    for(i = 0; ; i = (i + 1) % SAMPLE_COUNT)
    {
        start_time = GetSysTime();
        while((GetSysTime()-start_time) <= busy_cache[i]);
        usleep(idle_cache[i]*1000);
    }
    return NULL;
}

void *thread_cpu2(void*arg)//线程函数2，原理同线程函数1
{
    double busy_cache[SAMPLE_COUNT];
    double idle_cache[SAMPLE_COUNT];

    double radian = 0.0;

    double start_time;


    double temp;

	int i;
    for(i=0; i<SAMPLE_COUNT/4; i++)
    {

        temp = (double)(0.667*(radian-1)*(radian-1)-0.167);
        busy_cache[i] = (double)(SAMPLE_TIME*temp);
        idle_cache[i] = (double)(SAMPLE_TIME - busy_cache[i]);
        radian += INCREAMENT;
    }

    for(i= SAMPLE_COUNT/4; i < SAMPLE_COUNT/2; i++)
    {
        temp = (double)(0.667*(radian)*(radian)-0.167);
        busy_cache[i] = (double)(SAMPLE_TIME*temp);
        idle_cache[i] = (double)(SAMPLE_TIME - busy_cache[i]);
        radian += INCREAMENT;
    }

    for(i=SAMPLE_COUNT/2; i<3*SAMPLE_COUNT/4; i++)
    {
        temp = (double)(-8.0*(radian-1.25)*(radian-1.25)+1);
        busy_cache[i] = (double)(SAMPLE_TIME*temp);
        idle_cache[i] = (double)(SAMPLE_TIME - busy_cache[i]);
        radian += INCREAMENT;
    }

    for(i=3*SAMPLE_COUNT/4; i<SAMPLE_COUNT; i++)
    {
        temp = (double)(-8.0*(radian-1.75)*(radian-1.75)+1);
        busy_cache[i] = (double)(SAMPLE_TIME*temp);
        idle_cache[i] = (double)(SAMPLE_TIME - busy_cache[i]);
        radian += INCREAMENT;
    }

    for(i = 0; ; i = (i + 1) % SAMPLE_COUNT)
    {
        start_time = GetSysTime();
        while((GetSysTime()-start_time) <= busy_cache[i]);
        usleep(idle_cache[i]*1000);
    }
    return NULL;
}
