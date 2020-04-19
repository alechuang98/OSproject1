#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <error.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <sched.h>

int main(int argc, char *argv[]){
	struct sched_param param;
	int _fd = shm_open("exe_T", O_RDWR, S_IRWXU);
	if(_fd == -1){
		puts("shm GG");
		return 0;
	}
	int *_T = (int*)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, _fd, 0);
	int total = atoi(argv[2]);

	struct timespec st, ed;
	if(clock_gettime(CLOCK_REALTIME, &st) == -1) perror("[Child Error] start time gettime");
	printf("[start] %d %lu.%09lu\n", getpid(), st.tv_sec, st.tv_nsec);
	while(total > 0){
		total -= *_T;
#ifdef DEBUG
		printf("[%s] %d is running for %d unit of time\n", argv[1], getpid(), *_T);
		fflush(stdout);
#endif
		for(int t = 0; t < *_T; t ++){
			volatile unsigned long i;
			for (i = 0; i < 1000000UL; i ++);
		}
		if(total == 0){			
			if(clock_gettime(CLOCK_REALTIME, &ed) == -1) perror("[Child Error] end time gettime");
			char s[205];
			sprintf(s, "[Project1] %d %lu.%09lu, %lu.%09lu", getpid(), st.tv_sec, st.tv_nsec, ed.tv_sec, ed.tv_nsec);

			char sys_call[205] = {0};
			strcat(sys_call, "echo ");
			strcat(sys_call, s);
			strcat(sys_call, " > /dev/kmsg");
			system(sys_call);
			// if(syscall(333, s) == -1) puts("[Child Error] dmesg error");
		}
        param.sched_priority = 1;
		if(sched_setparam(0, &param) < 0) puts("[Child Error] set param error");
		sched_yield();
	}
	printf("[end] %d %lu.%09lu\n", getpid(), ed.tv_sec, ed.tv_nsec);
}
