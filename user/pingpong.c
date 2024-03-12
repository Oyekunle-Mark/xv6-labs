#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
	int p[2];

	pipe(p);

	if (fork() == 0) {
		int read_b = 'a';
		read(p[0], &read_b, 1);
		printf("%d: received ping\n", getpid());
		write(p[1], &read_b, 1);
		close(p[0]);
		close(p[1]);
		exit(0);
	} else {
		int sent = 'c';
		int read_b = '\0';

		write(p[1], &sent, 1);
		wait(0);
		read(p[1], &read_b, 1);
		printf("%d: received ping\n", getpid());
		close(p[0]);
		close(p[1]);
		exit(0);
	}
}	
