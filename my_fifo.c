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
#include "my_fifo.h"

typedef struct _id{
	int tsk_id, arr;
} tid;
typedef struct _seq{
	int tsk_id, rem;
} seq;

int cmp_fifo(const void *a ,const void *b){
	return (*(tid *)a).arr > (*(tid *)b).arr ? 1 : -1;
}

void my_fifo(TSK *tsk, int n){
	tid id[1005];
	for(int i = 0 ; i < n ; i ++){
		id[i].tsk_id = i;
		id[i].arr = tsk[i].arr;
	}
	qsort(id, n ,sizeof(id[0]), cmp_fifo);
	int t = 0, len = 0;
	seq lst[1005];

	for(int i = 0; i < n; i ++){
		int tsk_id = id[i].tsk_id;
		if(id[i].arr > t){
			lst[len ++] = (seq){-1, id[i].arr - t};
			t = id[i].arr;
		}
		if(t == id[i].arr) lst[len ++] = (seq){tsk_id, -1};
		lst[len ++] = (seq){tsk_id, tsk[tsk_id].rem};
		for(int j = i + 1, pre = t; j < n; j ++){
			if(t <= id[j].arr && id[j].arr < t + tsk[tsk_id].rem){
				lst[len - 1].rem = id[j].arr - pre;
				lst[len ++] = (seq){id[j].tsk_id, -1};
				lst[len ++] = (seq){tsk_id, t + tsk[tsk_id].rem - id[j].arr};
				pre = id[j].arr;
			}
		}
		t += tsk[tsk_id].rem;
	}
	
	for(int i = 0; i < len; i ++) printf("%d %d\n", lst[i].tsk_id, lst[i].rem);

	for(int i = 0; i < len; i ++){
		if(lst[i].tsk_id == -1){
			for(int t = 0 ; t < lst[i].rem; t++){
				volatile unsigned long k;
				for(k = 0; k < 1000000UL; k++);
			}
		} else if(lst[i].rem == -1){
			make(&tsk[lst[i].tsk_id]);
		} else if(lst[i].rem > 0){
			run(&tsk[lst[i].tsk_id], lst[i].rem);
		}		
	}
}

