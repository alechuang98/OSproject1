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
#include "my_psjf.h"
#define intmax 2147483647;
int cmp(const void *a, const void *b){
	if(((TSK *)a)->arr - ((TSK *)b)->arr > 0) return 1;
	else if(((TSK *)a)->arr - ((TSK *)b)->arr < 0) return -1;
	else{
		return ((TSK *)a)->rem - ((TSK *)b)->rem;
	}
}
void my_psjf(TSK *tsk,int n){
	TSK ntsk[1005];
	int appear[1005] = {0};
	for(int i = 0; i < n; i++){
		ntsk[i].arr = tsk[i].arr;
		ntsk[i].rem = tsk[i].rem;
		ntsk[i].pid = i;
	}
	int cnt = 0;
	int process_cnt = 0;
	int nxt = n;
	qsort(ntsk,n,sizeof(TSK),cmp);
	int now = ntsk[0].arr;
	int process[3000][2];
	for(int i = 1; i < n; i++){
		if(ntsk[i].arr != ntsk[0].arr){
			nxt = i;
			break;
		}
	}
	ntsk[n].arr = intmax;
	if(now != 0){
		process[process_cnt][0] = -1;
		process[process_cnt++][1] = now;
	}
	while(1){
		int min_index = -1, min_time = intmax;
		for(int i = 0; i < n; i++){
			if(ntsk[i].rem > 0){
				if(ntsk[i].arr < ntsk[nxt].arr){
					if(min_time > ntsk[i].rem){
						min_time = ntsk[i].rem;
						min_index = i;
					}
				}
			}
		}
		if(min_index == -1){
			process[process_cnt][0] = -1;
			process[process_cnt++][1] = ntsk[nxt].arr - now;
			for(int i = nxt + 1; i <= n; i++){
				if(ntsk[i].arr != ntsk[nxt].arr){
					nxt = i;
					break;
				}
			}
			continue;
		}
		if(now + min_time >= ntsk[nxt].arr){
			min_time = ntsk[nxt].arr - now;
			for(int i = nxt + 1; i <= n; i++){
				if(ntsk[i].arr != ntsk[nxt].arr){
					nxt = i;
					break;
				}
			}
		}
		if(!appear[min_index]){
			process[process_cnt][0] = ntsk[min_index].pid;
			process[process_cnt++][1] = -1;
			appear[min_index] = 1;
		}
		process[process_cnt][0] = ntsk[min_index].pid;
		process[process_cnt++][1] = min_time;
		ntsk[min_index].rem -= min_time;
		now += min_time;
		if(ntsk[min_index].rem == 0)
			cnt++;
		if(cnt == n)
			break;
	}
	int time = 0;
	for(int i = 0; i < process_cnt; i++){
		if(process[i][0] == -1){
			for(int t = 0 ; t < process[i][1] ; t++){
			volatile unsigned long k;
				for (k = 0; k < 1000000UL ; k++);
			}
		}
		else if(process[i][1] == -1)
			make(&tsk[process[i][0]]);
		else
			run(&tsk[process[i][0]],process[i][1]);
	}
	return;
}
