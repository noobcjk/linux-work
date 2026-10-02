#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define NEGPID_IOCTL_SET _IOW('n', 1, long)

int main(int argc, char **argv)
{
	long neg_pid = -281474976710656L;
	int fd;

	if (argc > 1)
		neg_pid = strtoll(argv[1], NULL, 0);
	if (neg_pid >= 0)
		neg_pid = -281474976710656L;

	fd = open("/dev/negpid", O_RDWR);
	if (fd < 0) {
		perror("open");
		return 1;
	}

	if (ioctl(fd, NEGPID_IOCTL_SET, &neg_pid) < 0) {
		perror("ioctl");
		close(fd);
		return 1;
	}

	printf("created negpid %ld\n", neg_pid);
	close(fd);
	return 0;
}
