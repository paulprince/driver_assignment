#include <sys/syscall.h>
#include <unistd.h>
#include <sys/types.h>
#include <linux/fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

struct my_struct
{
	int fd;
	char buf[4096];
};

struct my_struct arg_dev0_param1;
struct my_struct arg_dev0_param2;
struct my_struct arg_dev0_param3;

void *read_thread (void *arg)
{
	struct my_struct *arg1 = arg;
	int ret;
	
	//printf("fd is %d\n", arg1->fd);
	ret = read(arg1->fd, arg1->buf, 4096);
	if(ret > 0)
	{/*TODO  print the dev name and param #*/
		write(STDOUT_FILENO, arg1->buf, ret);
	}
}

int main()
{
	int fd1, fd2, fd3, ret;

	pthread_t dev0_param1_thid;
	pthread_t dev0_param2_thid;
	pthread_t dev0_param3_thid;

	fd1 = open ("/sys/kernel/kset_device_paul/dev0/dev_param1", O_RDONLY);
	if(fd1 < 0)
	{
		perror("Error opening param1 attribute\n");
		exit(1);
	}
	arg_dev0_param1.fd = fd1;
	ret = pthread_create(&dev0_param1_thid, NULL, read_thread, &arg_dev0_param1);

	fd2 = open ("/sys/kernel/kset_device_paul/dev0/dev_param2", O_RDONLY);
	if(fd2 < 0)
	{
		perror("Error in opening param2 file \n");
		exit(1);
	}
	arg_dev0_param2.fd = fd2;
	ret = pthread_create(&dev0_param2_thid, NULL, read_thread, &arg_dev0_param2);
	
	fd3 = open ("/sys/kernel/kset_device_paul/dev0/dev_param3", O_RDONLY);
	if(fd3 < 0)
	{
		perror("Error in opening param3 file \n");
		exit(1);
	}
	arg_dev0_param3.fd = fd2;
	ret = pthread_create(&dev0_param3_thid, NULL, read_thread, &arg_dev0_param3);
	




/*	while(1)
	{
		ret = read(fd1, buf, 10);
		if(ret ==0)
			break;
		printf("foo : # of chars returned is %d\n", ret);
//		printf("In bar : ");
		if(ret > 0)
		{
			write(STDOUT_FILENO, buf, ret);
		}
	}
*/
/*	lseek(fd1, 0, SEEK_SET);

	while(1)
	{
		ret = read(fd1, buf, 10);
		if(ret ==0)
			break;
		printf("foo : # of chars returned is %d\n", ret);
//		printf("In bar : ");
		if(ret > 0)
		{
			write(STDOUT_FILENO, buf, ret);
		}
	}

*/	
	
//	printf("\n");

/*
	while(1)
	{
		ret = read(fd2, buf, 10);
		if(ret ==0)
			break;
		printf("foo : # of chars returned is %d\n", ret);
//		printf("In foo : ");
		if(ret > 0)
		{
			write(STDOUT_FILENO, buf, ret);
		}
	}
//	printf("\n");

	while(1)
	{
		ret = read(fd3, buf, 10);
		if(ret ==0)
			break;
		printf("baz : # of chars returned is %d\n", ret);
//		printf("In baz : ");
		if(ret > 0)
		{
			write(STDOUT_FILENO, buf, ret);
		}
	}
//	printf("\n");

*/
	pthread_join(dev0_param1_thid, NULL);
	pthread_join(dev0_param2_thid, NULL);
	pthread_join(dev0_param3_thid, NULL);

	exit(0);
}

