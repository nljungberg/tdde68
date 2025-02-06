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
	struct thread *cur = thread_current();
	struct file *file = filesys_open(file);
	for (int i = 2; i < 128; i++) { 
		if (cur->fd_table[i] == NULL) {
			 cur->fd_table[i] = file;
			 return i;
		}		
	}
	return -1;
}

void close (int fd) {
	struct thread *cur = thread_current();
	if (fd > 128 || fd < 2) {
		return;
	}
	file_close(cur->fd_table[fd]);
}

int write (int fd, const void *buffer, unsigned size){
	struct thread *cur = thread_current();
	if (fd == 1) { // STDOUT
		putbuf(buffer, size);
		return size;
	} else if (fd > 1) {
		struct file *file = cur->fd_table[fd];
		int write_id = file_write(file, buffer);
		return write_id;
	}
	return -1;
}

int read (int fd, const void* buffer, unsigned size){
	if (fd == 0) { // stdin
		input_getc()
	}

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
