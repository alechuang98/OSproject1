all: main.c control.c my_fifo.c my_sjf.c my_psjf.c my_rr.c task.c
	gcc -o main main.c control.c my_fifo.c my_sjf.c my_psjf.c my_rr.c -lrt -std=c11
	gcc -o task task.c -lrt -DDEBUG -std=c11

clear:
	rm -f main task
