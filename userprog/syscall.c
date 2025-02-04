#include "userprog/syscall.h"

#include "threads/interrupt.h"
#include "threads/thread.h"
#include "devices/timer.h"
#include "devices/shutdown.h"
#include "filesys/filesys.h"

#include <stdio.h>
#include <syscall-nr.h> 

static void syscall_handler(struct intr_frame*);

void syscall_init(void)
{
	intr_register_int(0x30, 3, INTR_ON, syscall_handler, "syscall");

}

static void syscall_handler(struct intr_frame* f UNUSED)
{
	printf("system call!\n");
	thread_exit();
}


void sleep(int millis){
	if(millis<0){
		return;
	}
	timer_msleep(millis);
	return;
}

void halt(void){
	shutdown_power_off();
}

bool create(const char *file, unsigned initial_size){
	return filesys_create(file, initial_size);
}

int open(const char *file){
	int fds[128] = thread_current()->fd;
	struct file* file = filesys_open(file);
	if(fid != -1){
		append(fds, fid);
	}
	return fid;
}

void close (int fd){

}

int write (int fd, const void *buffer, unsigned size){

}

int read (int fd, const void* buffer, unsigned size){

}

bool remove(const char *file_name){
	return filesys_remove(file_name);
}

int filesize(int fd){

}

void seek(int fd){

}

unsigned tell(int fd){

}

void exit(int status){
	
}
