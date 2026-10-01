#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/syscall.h>

#define CLONE_NEGPID 0x00001000

int main(int argc, char **argv)
{
	char *stack;
	long pid;
	int sec = 30;   /* 默认 30 秒 */

	if (argc > 1)
		sec = atoi(argv[1]);
	if (sec <= 0)
		sec = 30;

	stack = malloc(65536);
	if (!stack)
		return 1;

	pid = syscall(SYS_clone,
		CLONE_NEGPID | SIGCHLD,
		stack + 65536,
		NULL, NULL, 0);

	if (pid == 0) {
		/* 子进程 */
		printf("child: pid=%d, alive %d s\n", getpid(), sec);
		sleep(sec);
		printf("child: exiting\n");
		_exit(0);
	}

	/* 父进程 */
	printf("child pid = %ld\n", pid);
	return 0;
}
