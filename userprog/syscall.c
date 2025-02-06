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
	int syscall_nr = (f->esp);
	int *args = (int *) f->esp;
	switch (syscall_nr)
	{
	case SYS_WRITE:
		write(args[1], args[2], args[3]);
		/* code */
		break;
	case SYS_CLOSE:
		close(args[1]);
		/* code */
		break;
	case SYS_CREATE:
		create(args[1], args[2]);
		/* code */
		break;
	case SYS_HALT:	
		halt();
		/* code */
		break;
	case SYS_OPEN:
		open(args[1]);
		/* code */
		break;
	case SYS_READ:
		read(args[1], args[2], args[3]);
		/* code */
		break;
	case SYS_SLEEP:
		sleep(args[1]);
		/* code */
		break;
	case SYS_SEEK:
		seek(args[1], args[2]);
		/* code */
		break;
	case SYS_TELL:
		tell(args[1]);
		/* code */
		break;
	case SYS_REMOVE:
		remove(args[1]);
		/* code */
		break;
	case SYS_FILESIZE:
		filesize(args[1]);
		/* code */
		break;
	case SYS_EXIT:
		exit(args[1]);
		/* code */
		break;
	default:
		break;
	}
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
	if ( 1 < fd < 128) {
		return;
	}
	file_close(cur->fd_table[fd]);
}

int write (int fd, const void *buffer, unsigned size){
	struct thread *cur = thread_current();
	if (fd == 1) { // STDOUT
		putbuf(buffer, size);
		return size;
	} else if ( 1 < fd < 128)  {
		struct file *file = cur->fd_table[fd];
		int write_size = file_write(file, buffer);
		return write_size;
	}
	return -1;
}

int read (int fd, const void* buffer, unsigned size){

	for (unsigned i = 0; i < size; i++) {  //checks that the buffer is valid
			if(!is_user_vaddr(buffer + i)) {
				return -1;
			}
		}

	struct thread *cur = thread_current();
	if (fd == 0) { // stdin
		input_getc();
	} else if (1 < fd < 128) {
		struct file *file = cur->fd_table[fd];
		int bytes_read = file_read(file, buffer, size);
		return bytes_read;
	}
	return -1;

}

bool remove(const char *file_name){
	return filesys_remove(file_name);
}

int filesize(int fd){
	struct thread *cur = thread_current();
	if (1 < fd < 128) {
		struct file *file = cur->fd_table[fd];
		return file_length(file);
	}
	return -1;
}

void seek(int fd, unsigned position){
	struct thread *cur = thread_current();
	if (1 < fd < 128) {
		struct file *file = cur->fd_table[fd];
		file_seek(file, position);
	}
	return -1;

}

unsigned tell(int fd){
	struct thread *cur = thread_current();
	if (1 < fd < 128) {
		struct file *file = cur->fd_table[fd];
		return file_tell(file);
	}
	return -1;

}	

void exit(int status){
	struct thread *cur = thread_current();
	cur->fd_table;
	for (int i = 2; cur->fd_table[i] != NULL; i++) {
		close(i);
	}
	cur->status = status;
	thread_exit();
}
