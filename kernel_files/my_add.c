#include <linux/kernel.h>
#include <linux/linkage.h>

asmlinkage int sys_my_add(char *s){
    printk("%s\n",s);
    return 1;
}

