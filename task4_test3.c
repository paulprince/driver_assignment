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
	char name[20];
	int fd;
	char buf[4096];
};

struct my_struct arg_dev0_param1;
struct my_struct arg_dev0_param2;
struct my_struct arg_dev0_param3;

pthread_mutex_t console_mutex;

void *read_thread (void *arg)
{
	struct my_struct *arg1 = arg;
	int ret, len, len1, ret1;
	
	//printf("fd is %d\n", arg1->fd);
	ret = read(arg1->fd, arg1->buf, 4096);
	if(ret < 0){ perror("error in read\n");exit(1);}
	//printf("ret value is %d\n",ret );
	len1 = strlen(arg1->buf);
	arg1->buf[len1+1] = '\n';
	if(ret > 0)
	{/*TODO  print the dev name and param #*/
		len = strlen(arg1->name);
		ret1 = pthread_mutex_lock(&console_mutex);
		if(ret1 != 0) { perror("Error in read's mutex lock\n"); exit(1);}
	//	printf("inside mutex : name is ");
		write(STDOUT_FILENO, arg1->name, len);
		write(STDOUT_FILENO, arg1->buf, ret+1);
	//	printf("About to unlock mutex\n");
		ret = pthread_mutex_unlock(&console_mutex);
		if(ret != 0) { perror("Error in read's mutex unlock\n"); exit(1);}
	//	printf("unlocked mutex\n");
	}
	pthread_exit(NULL);
}

void *write_thread(void *arg)
{
	struct my_struct *arg1 = arg;
	int data, len;
	
	len = strlen(arg1->name);
	write (STDOUT_FILENO, arg1->name, len);
	read (STDIN_FILENO, arg1->buf, 4096);
	write (arg1->fd, arg1->buf, 4096);

	pthread_exit(NULL);
}

int main()
{
	int fd1, fd2, fd3, ret;
	pthread_mutexattr_t ma;
	pthread_attr_t tha;

	pthread_t dev0_param1_thid;
	pthread_t dev0_param2_thid;
	pthread_t dev0_param3_thid;
	pthread_t dev0_param1_write_thid;

	pthread_attr_init(&tha);
	pthread_mutexattr_init(&ma);
	pthread_mutexattr_setpshared(&ma, PTHREAD_PROCESS_PRIVATE);
	pthread_mutexattr_settype(&ma, PTHREAD_MUTEX_NORMAL);
	pthread_mutex_init(&console_mutex, &ma);

	/******dev0param1****/
	fd1 = open ("/sys/kernel/kset_device_paul/dev0/dev_param1", O_RDWR);
	if(fd1 < 0) { perror("Error opening param1 attribute\n"); exit(1);}
	//printf("param1 fd is %d\n", fd1);
	strcpy(arg_dev0_param1.name,  "dev 0 param 1\n");
	arg_dev0_param1.fd = fd1;

	ret = pthread_create(&dev0_param1_thid, &tha, read_thread, &arg_dev0_param1);
	if(ret > 0){ printf("error in creating thread \n"); exit(3);}

	/******dev0 param2*****/
	fd2 = open ("/sys/kernel/kset_device_paul/dev0/dev_param2", O_RDONLY);
	if(fd2 < 0){ perror("Error in opening param2 file \n");	exit(1);}
//	printf("param2 fd is %d\n", fd2);

	strcpy(arg_dev0_param2.name,  "dev 0 param 2\n");
	arg_dev0_param2.fd = fd2;
	
	ret = pthread_create(&dev0_param2_thid, &tha, read_thread, &arg_dev0_param2);
	if(ret > 0){ printf("error in creating thread \n"); exit(3);}
	
	/*******dev0 param3*****/
	fd3 = open ("/sys/kernel/kset_device_paul/dev0/dev_param3", O_RDONLY);
	if(fd3 < 0){perror("Error in opening param3 file \n"); exit(1);}
//	printf("param3 fd is %d\n", fd3);

	strcpy(arg_dev0_param3.name,  "dev 0 param 3\n");
	arg_dev0_param3.fd = fd3;
	
	ret = pthread_create(&dev0_param3_thid, &tha, read_thread, &arg_dev0_param3);
	if(ret > 0) { printf("error in creating thread \n"); exit(3);}
/*
	ret = pthread_create(&dev0_param1_write_thid,NULL,  write_thread, &arg_dev0_param1);
	if(ret > 0) { printf("error in creating thread \n"); exit(3);}
*/
	pthread_join(dev0_param1_thid, NULL);
	pthread_join(dev0_param2_thid, NULL);
	pthread_join(dev0_param3_thid, NULL);
//	pthread_join(dev0_param1_write_thid, NULL);

	exit(0);
}
