#ifndef MYRR_H
#define MYRR_H
typedef struct rr_process {
	TSK tsk;
	int id, ll, rr;
} TSK_rr;
int rr_cmp(const void *p1, const void *p2);
void my_rr(TSK *tsk, int n);

#endif
