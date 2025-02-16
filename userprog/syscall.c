#include "userprog/syscall.h"

#include "threads/interrupt.h"
#include "threads/thread.h"
#include "devices/timer.h"
#include "devices/shutdown.h"
#include "filesys/filesys.h"
#include "lib/user/syscall.h"
#include "lib/kernel/stdio.h"
#include "filesys/file.h"
#include "userprog/process.h"
#include "threads/palloc.h"
#include <stdio.h>
#include <syscall-nr.h> 

static void syscall_handler(struct intr_frame*);


void syscall_init(void)
{
	intr_register_int(0x30, 3, INTR_ON, syscall_handler, "syscall");

}


void syscall_sleep(int millis);
void syscall_halt(void);
bool syscall_create(const char *file, unsigned initial_size);
int syscall_open(const char *file);
void syscall_close (int fd);
int syscall_write (int fd, const void *buffer, unsigned size);
int syscall_read (int fd, void *buffer, unsigned size);
bool syscall_remove(const char *file_name);
int syscall_filesize(int fd);
void syscall_seek(int fd, unsigned position);
unsigned syscall_tell(int fd);
void syscall_exit(int status);
pid_t syscall_exec(const *cmd_line);

static void syscall_handler(struct intr_frame* f UNUSED)
{
	int syscall_nr = *((int *) f->esp);
	int *args = (int *) f->esp;
	switch (syscall_nr) {
		case SYS_WRITE: 
			f->eax = syscall_write(args[1], args[2], args[3]);
			/* code */
			break;
		
		case SYS_CLOSE:
			syscall_close(args[1]);
			/* code */
			break;
		case SYS_CREATE:
			f->eax = syscall_create(args[1], args[2]);
			/* code */
			break;
		
		case SYS_HALT:
			syscall_halt();
			/* code */
			break;
		
		case SYS_OPEN:
			f->eax = syscall_open(args[1]);
			/* code */
			break;
		
		case SYS_READ:
			f->eax = syscall_read(args[1], args[2], args[3]);
			/* code */
			break;
		
		case SYS_SLEEP:
			syscall_sleep(args[1]);
			/* code */
			break;
		
		case SYS_SEEK:
			syscall_seek(args[1], args[2]);
			/* code */
			break;
		
		case SYS_TELL:
			f->eax = syscall_tell(args[1]);
			/* code */
			break;
		
		case SYS_REMOVE:
			f->eax = syscall_remove(args[1]);
			/* code */
			break;
		
		case SYS_FILESIZE:
			f->eax = syscall_filesize(args[1]);
			/* code */
			break;
		case SYS_EXIT:
			syscall_exit(args[1]);
			/* code */
			break;
		case SYS_EXEC:
			f->eax = syscall_exec(args[1]);
			/* code */
			break;
        case SYS_WAIT:
            f->eax = syscall_wait(args[1]);
			break;
		default:
			break;
		}
}


void syscall_sleep(int millis){
	if(millis<0){
		return;
	}
	timer_msleep(millis);
	return;
}

void syscall_halt(void){
	shutdown_power_off();
}

bool syscall_create(const char *file, unsigned initial_size){
	return filesys_create(file, initial_size);
}

int syscall_open(const char *file){
	struct thread *cur = thread_current();
	struct file *cur_file = filesys_open(file);
	if (cur_file == NULL) {
		return -1;
	}
	for (int i = 2; i < 128; i++) { 
		if (cur->fd_table[i] == NULL) {
			 cur->fd_table[i] = cur_file;
			 return i;
		}		
	}
	return -1;
}

void syscall_close (int fd) {
	struct thread *cur = thread_current();
	if ( fd < 2 || fd >= 128) {
		return;
	}
	if (cur->fd_table[fd] != NULL) {
		file_close(cur->fd_table[fd]);
		cur->fd_table[fd] = NULL;
	}
	return;
}

int syscall_write (int fd, const void *buffer, unsigned size){
	
	struct thread *cur = thread_current();
	if (fd == 1) { // STDOUT
		putbuf(buffer, size);
		return size;
	} else if (fd >= 2 && fd < 128)  {
		struct file *file = cur->fd_table[fd];
		    if (file == NULL)
            return -1;
		int write_size = file_write(file, buffer, size);
		return write_size;
	}
	return -1;
}

int syscall_read (int fd, void *buffer, unsigned size){

	struct thread *cur = thread_current();
	if (fd == 0) { // stdin
		char *buf = (char *) buffer;
		for (unsigned i = 0; i < size; i++) {
			buf[i] = input_getc();
			if(buf[i] == '\r'){
				buf[i] = '\n';
			}
			putbuf(&buf[i], 1);
		}
    	return size;
	} else if (fd >= 2 && fd < 128) {
		if(cur->fd_table[fd] != NULL){
			struct file *file = cur->fd_table[fd];
			int bytes_read = file_read(file, buffer, size);
			return bytes_read;
		}
	}
	return -1;
}

bool syscall_remove(const char *file_name){
	return filesys_remove(file_name);
}

int syscall_filesize(int fd){
	struct thread *cur = thread_current();
	if (fd >= 2 && fd < 128) {
		struct file *file = cur->fd_table[fd];
		if(file != NULL){
			return file_length(file);
		}
	}
	return -1;
}

void syscall_seek(int fd, unsigned position){
	struct thread *cur = thread_current();
	if (fd >= 2 && fd < 128) {
		struct file *file = cur->fd_table[fd];
		if(file != NULL){
			unsigned file_len = file_length(file);
			if (position > file_len) {
				position = file_len;
			}
			file_seek(file, position);
		}
	}
	return; 

}

unsigned syscall_tell(int fd){
	struct thread *cur = thread_current();
	if (fd >= 2 && fd < 128) {
		struct file *file = cur->fd_table[fd];
		if(file != NULL){
			return file_tell(file);
		}
	}
	return 0;
}	

void syscall_exit(int status){
	struct thread *cur = thread_current();
    if (cur->own_status != NULL) {
    	cur->own_status->exit_satus = status;
    	cur->own_status->alive_count--; // child is now dead decrement alive count
    	sema_up(&cur->own_status->load_sema);
        if (cur->own_status->alive_count == 0) {
			free(cur->own_status);
		}
    	printf("%s: exit(%d)\n", cur->thread_name, status);
	}
	thread_exit();
}

pid_t syscall_exec(const *cmd_line){
	pid_t pid = process_execute(cmd_line);
	return pid;
}

int syscall_wait(int pid){
	struct thread *cur = thread_current();
    for (struct list_elem *e = list_begin(&cur->children); e != list_end(&cur->children); e = list_next(e)) {
		struct parent_child *cur_pc = list_entry(e, struct parent_child, elem);
        if (cur_pc->tid == pid) {
			sema_down(&cur_pc->load_sema);
			int status = cur_pc->exit_status;
			list_remove(e);
            cur_pc->alive_count--;
            if (cur_pc->alive_count == 0) {
				free(cur_pc);
			}
			return status;
		}
	}
	return -1;
}


