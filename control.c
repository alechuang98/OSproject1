#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <sys/mman.h>
#include <wait.h>
#include <fcntl.h>
#include <errno.h>
#include <sched.h>
#include <assert.h>
#include "control.h"

int _fd = 0, *_T = 0;

void make(TSK *tsk){
	// printf("make %s\n", tsk -> name);
	fflush(stdout);
	tsk -> pid = fork();
	int PID = tsk -> pid;
	if(PID < 0) puts("fork GG");
	else if(PID == 0){
		fflush(stdout);
		char tim[105];
		printf("trigger child");
		sprintf(tim, "%d", tsk -> rem);
		execlp("./task", "./task", tsk -> name, tim, (char *)0);
	}
}

void run(TSK *tsk, int run_time){
	if(run_time > tsk -> rem){
		puts("run time > task time QQ");
		return;
	}
	tsk -> rem -= run_time;
	*_T = run_time;
	struct sched_param param;
	param.sched_priority = 3;
	// printf("set %s 's param\n", tsk -> name);
	if(sched_setparam(tsk -> pid, &param) == -1) puts("child set param GG");
}

void clear(TSK *tsk){
	waitpid(tsk -> pid, NULL, 0);
}
