#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define BUF_SIZE 1024

int main(){
	int fd;
	char *str_hello = "Hello, Virtual Buffer";
	char buff[BUF_SIZE];

	printf("Device driver test\n");
	if((fd = open("/dev/virtual_buffer", O_RDWR)) > 0){
		write(fd, str_hello, strlen(str_hello) + 1);
		read(fd, buff, BUF_SIZE);
		printf("read from device: %s\n", buff);
		close(fd);
	}
	return 0;
}
