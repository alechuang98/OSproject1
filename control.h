#ifndef CONTROL_H
#define CONTROL_H

extern int _fd, *_T;
typedef struct _process{
	char name[35];
	int arr, rem, pid; // arrival time, remain time, process ID
} TSK;
void make(TSK *tsk);
void run(TSK *tsk, int run_time);
void clear(TSK *tsk);

#endif
