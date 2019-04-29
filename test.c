
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
	printf("%d\n", sched_get_priority_max(SCHED_FIFO));
}
