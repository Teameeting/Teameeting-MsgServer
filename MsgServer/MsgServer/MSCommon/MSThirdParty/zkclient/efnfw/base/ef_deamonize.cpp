#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>



namespace ef{
void daemonize(){
	// clear file creation mask
	umask(0);

	// fork child
	pid_t pid = fork();

	if (pid < 0) {
		exit(-1);
	} else if (pid > 0) {
		exit(0);
	}

	setsid();
	close(0);
	close(1);
	close(2);

	int fd0 = open("/dev/null", O_RDWR);
	int fd1 = dup(fd0);
	int fd2 = dup(fd0);
	
	
}

};
