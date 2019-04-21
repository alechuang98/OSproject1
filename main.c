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
/* 
#include "my_fifo.h"
#include "my_rr.h"
#include "my_sjf.h"
#include "my_psjf.h"
*/

int main(){
	TSK tsk[1005];
	int n;
	char cmd[15];
	cpu_set_t _set;
	struct sched_param param;

	memset(tsk, 0, sizeof(tsk));
	_fd = shm_open("exe_T", O_CREAT | O_RDWR, S_IRWXU);
	if(_fd < 0) puts("main shm GG");
	ftruncate(_fd, sizeof(int));
	_T = (int*)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, _fd, 0);
	*_T = 0;
	CPU_ZERO(&_set);
	CPU_SET(0, &_set);
	if(sched_setaffinity(0, sizeof(cpu_set_t), &_set) == -1) puts("main set affinity GG");
	param.sched_priority = 2;
	if(sched_setscheduler(0, SCHED_FIFO, &param) == -1) puts("main set schedluer GG");
	if(sched_setparam(0, &param) == -1) puts("main set param GG");
	
	scanf("%s %d", cmd, &n);
	for(int i = 0; i < n; i ++){
		scanf("%s %d %d", tsk[i].name, &tsk[i].arr, &tsk[i].rem);
	}
	/*
	if(cmd[0] == 'F') my_fifo(tsk);
	if(cmd[0] == 'R') my_rr(tsk);
	if(cmd[0] == 'S') my_sjf(tsk);
	if(cmd[0] == 'P') my_psjf(tsk);
	*/
#ifdef TEST
	for(int i = 0; i < 2; i ++) make(&tsk[i]);
	run(&tsk[0], 20);
	run(&tsk[1], 50);
	run(&tsk[0], 80);
#endif
	for(int i = 0; i < n; i ++){
		clear(&tsk[i]);
		assert(tsk[i].rem == 0);
		printf("%s %d\n", tsk[i].name, tsk[i].pid);
	}
	shm_unlink("exe_T");
}
