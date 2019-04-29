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
#include <fcntl.h>
#include <errno.h>
#include <sched.h>
#include <assert.h>
#include "control.h"
#include "my_rr.h"

int rr_cmp(const void *p1, const void *p2) {
	if ( ((TSK_rr*)p1)->tsk.arr < ((TSK_rr*)p2)->tsk.arr ) return -1;
	else if ( ((TSK_rr*)p1)->tsk.arr == ((TSK_rr*)p2)->tsk.arr ) {
		if ( ((TSK_rr*)p1)->id < ((TSK_rr*)p2)->id ) return -1;
		else if ( ((TSK_rr*)p1)->id == ((TSK_rr*)p2)->id ) return 0;
		else return 1;
	}
	else return 1;
}
void my_rr(TSK *tsk, int n) {
	TSK_rr buf[1005];
	for (int i = 0; i < n; i++) {
		buf[i].tsk = tsk[i];
		buf[i].id = i;
		buf[i].ll = buf[i].rr = -1;
	}
	qsort(buf, n, sizeof(TSK_rr), rr_cmp);
	int t = 0;
	int ptr = 0, st = -1, ed = -1;
	while (ptr != n || st != -1) {
		if (st == -1) {
			int tt = buf[ptr].tsk.arr - t;
			t = buf[ptr].tsk.arr;
			for (int i = 0; i < tt; i++) {
				volatile unsigned long k;
				for (k = 0; k < 1000000UL; k++);
			}
			make(&tsk[buf[ptr].id]);
			st = ed = ptr;
			buf[ptr].ll = buf[ptr].rr = -1;
			ptr++;
		}
		int p = st;
		while (p != -1) {
			int la = tsk[buf[p].id].rem;
			if (la > 500) la = 500;
			while (ptr != n && t + la >= tsk[buf[ptr].id].arr) {
				int used = tsk[buf[ptr].id].arr - t;
				if (used) {
					run(&tsk[buf[p].id], used);
					la -= used;
					t += used;
				}
				make(&tsk[buf[ptr].id]);
				buf[ed].rr = ptr;
				buf[ptr].ll = ed;
				ed = ptr;
				buf[ptr].rr = -1;
				ptr++;
			}
			if (la) { 
				run(&tsk[buf[p].id], la);
				t += la;
			}
			if (!tsk[buf[p].id].rem) {
				if (st == ed) {
					st = ed = -1;
				} else if (p == st) {
					st = buf[p].rr;
					buf[st].ll = -1;
				} else if (p == ed) {
					ed = buf[p].ll;
					buf[ed].rr = -1;
				} else {
					buf[buf[p].ll].rr = buf[p].rr;
					buf[buf[p].rr].ll = buf[p].ll;
				}
			}
			p = buf[p].rr;
		}
	}
}
