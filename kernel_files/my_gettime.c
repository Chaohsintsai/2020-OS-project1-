#include <linux/ktime.h>
#include <linux/timekeeping.h>

asmlinkage void sys_my_gettime(struct timespec *t) {
	getnstimeofday(t);
}
