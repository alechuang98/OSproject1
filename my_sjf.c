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
#include "my_sjf.h"

typedef struct my_process{
	char name[35];
	int arr, rem, pid, org_idx;
} TSK_copy;

int cmp_sjf(const void *a ,const void *b){
	if((*(TSK_copy *)a).arr == (*(TSK_copy *)b).arr) return (*(TSK_copy *)a).rem > (*(TSK_copy *)b).rem ? 1 : -1;
	else return (*(TSK_copy *)a).arr > (*(TSK_copy *)b).arr ? 1 : -1;
}

int cmp_sjf_find(const void *a ,const void *b){
	return (*(TSK_copy *)a).rem > (*(TSK_copy *)b).rem ? 1 : -1;
}

void my_sjf(TSK *tsk, int n){

	TSK_copy my_tsk[1005];
	for(int i = 0 ; i < n ; i++){
		strcpy(my_tsk[i].name, tsk[i].name);
		my_tsk[i].arr = tsk[i].arr;
		my_tsk[i].rem = tsk[i].rem;
		my_tsk[i].pid = tsk[i].pid;
		my_tsk[i].org_idx = i;
	}
	//make the process according to the arrival time
	qsort(my_tsk, n ,sizeof(my_tsk[0]), cmp_sjf);

	int t = 0, start, more = 0;
	int cmd_cnt = 0, cmd_type[10005][3];
	int maked[1005] = {0};
	for(int i = 0 ; i < n ; i++){
		if(i != 0 && more > 1){
			qsort(&my_tsk[i], more, sizeof(my_tsk[0]), cmp_sjf_find);
			more--;
		}	
		int pre_make = 0;
		more = 0;
		int wait_time = my_tsk[i].arr - t;
		if(wait_time < 0) wait_time = 0;
		else if(wait_time == 0){
			if(i == 0){
				cmd_type[cmd_cnt][0] = 1;
				cmd_type[cmd_cnt++][2] = my_tsk[i].org_idx;
				maked[my_tsk[i].org_idx] = 1;
			}
		}
		else{
			cmd_type[cmd_cnt][0] = 0;
			cmd_type[cmd_cnt++][1] = wait_time;
			cmd_type[cmd_cnt][0] = 1;
			cmd_type[cmd_cnt++][2] = my_tsk[i].org_idx;
		}
		start = t + wait_time;
		t += my_tsk[i].rem + wait_time;
		for(int j = i+1 ; j < n ; j++){
			if(my_tsk[j].arr <= t && my_tsk[j].arr >= start) pre_make++;
			else break;
		}
		for(int j = i+1 ; j < n ; j++){
			if(my_tsk[j].arr <= t) more++;
			else break;
		}
		for(int j = i+1 ; j < i+1+pre_make ; j++){
			if(maked[my_tsk[j].org_idx] == 0){
				int run_time = my_tsk[j].arr - start;
				start = my_tsk[j].arr;
				if(run_time != 0){
					cmd_type[cmd_cnt][0] = 2;
					cmd_type[cmd_cnt][1] = run_time;
					cmd_type[cmd_cnt++][2] = my_tsk[i].org_idx;
					my_tsk[i].rem -= run_time;
				}
				cmd_type[cmd_cnt][0] = 1;
				cmd_type[cmd_cnt++][2] = my_tsk[j].org_idx;
				maked[my_tsk[j].org_idx] = 1;
			}	
		}
		if(my_tsk[i].rem != 0){
			cmd_type[cmd_cnt][0] = 2;
			cmd_type[cmd_cnt][1] = my_tsk[i].rem;
			cmd_type[cmd_cnt++][2] = my_tsk[i].org_idx;
			my_tsk[i].rem = 0;
		}
	}

	
	for(int i = 0 ; i < cmd_cnt ; i++){
		if(cmd_type[i][0] == 0){
			for(int t = 0 ; t < cmd_type[i][1] ; t++){
				volatile unsigned long k;
				for (k = 0; k < 1000000UL ; k++);
			}
		}
		else if(cmd_type[i][0] == 1){
			make(&tsk[cmd_type[i][2]]);
			//printf("make %d\n",cmd_type[i][2]);
		}
		else{
			run(&tsk[cmd_type[i][2]], cmd_type[i][1]);
		}		
	}
	
}
