#include <unistd.h>
#include <sys/types.h>
#include <linux/fcntl.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
	int fd1, fd2, fd3, ret;
char buf[10];
	fd1 = open ("/sys/kernel/kset_device_paul/dev0/dev_param1", O_RDONLY);
	if(fd1 < 0)
	{
		perror("Error opening foo attribute\n");
		exit(1);
	}

/*	fd2 = open ("/sys/kernel/Paul_kobject/foo", O_RDONLY);
	if(fd2 < 0)
	{
		perror("Error in opening foo file \n");
		exit(1);
	}
	
	fd3 = open ("/sys/kernel/Paul_kobject/baz", O_RDONLY);
	if(fd3 < 0)
	{
		perror("Error in opening baz file \n");
		exit(1);
	}
*/
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

	lseek(fd1, 0, SEEK_SET);

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
	exit(0);
}

