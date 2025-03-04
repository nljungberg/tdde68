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
#include "userprog/pagedir.h"
#include "threads/vaddr.h"

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
pid_t syscall_exec(const char *cmd_line);
int syscall_wait(int pid);

static bool is_valid_user_ptr(const void *ptr) {
    return ptr != NULL && is_user_vaddr(ptr) && pagedir_get_page(thread_current()->pagedir, ptr) != NULL;
}
static bool valid_user_string (const char *str) {
    if (str == NULL) return false;
    for (;;) {
        if (!is_valid_user_ptr(str)) return false;  // check that page is present
        if (*str == '\0') return true;             // found terminator
        str++;
    }
}


static bool valid_user_buffer(const void *buffer, unsigned size){
	char *buf = (char *) buffer;
	char *start = (char*)buffer;
	char *end = start+size;
	if(!is_user_vaddr(start) || (size > 0 && !is_user_vaddr(end-1))) return false;
	char* page_start = pg_round_down(start);
	char* page_end = pg_round_up(end);
	for(char* page = page_start; page < page_end; page += PGSIZE){
		if(pagedir_get_page(thread_current()->pagedir, page) == NULL) return false;
	}
	return true;
}

static int
get_user (const uint8_t *uaddr)
{
  int result;
  asm ("movl $1f, %0\n"
       "1:\tmovzbl %1, %0\n"
       "movl %0, %0\n"
       "1:" : "=&a" (result) : "m" (*uaddr));
  return result;
}


int safe_get_int(const void *uaddr) {
  int value;
  for (size_t i = 0; i < sizeof(int); i++) {
    int byte = get_user((uint8_t *)uaddr + i);
    if (byte < 0)
      syscall_exit(-1);
    ((uint8_t *)&value)[i] = byte;
  }
  return value;
}


static void syscall_handler(struct intr_frame* f UNUSED)
{
	if (!valid_user_buffer(f->esp, 1 * sizeof(int)))
    	syscall_exit(-1);

	int *args = (int *) f->esp;

	int syscall_nr = args[0];


	switch (syscall_nr) {
		case SYS_WRITE:
			if (!valid_user_buffer(f->esp  ,4  * sizeof(int))) syscall_exit(-1);
			void *buf = (void*) args[2];
			unsigned size = args[3];

			if (!valid_user_buffer(buf, size)) syscall_exit(-1);

			
			f->eax = syscall_write(args[1], buf,size);
			/* code */
			break;

		case SYS_CLOSE:
			if(!is_valid_user_ptr(&args[1])) syscall_exit(-1);
			if (!valid_user_buffer(f->esp + 4, sizeof(int)))
    			syscall_exit(-1);
			int fds = safe_get_int(&args[1]);
			syscall_close(fds);
			/* code */
			break;
		case SYS_CREATE:
			if (!valid_user_buffer(f->esp, 3 * sizeof(int)))
    			syscall_exit(-1);
            const char *file_name_ = (const char*) args[1];
        	unsigned  size_ = args[2];
			if (!valid_user_string(file_name_)) {
            	syscall_exit(-1);
			}
			f->eax = syscall_create(file_name_, size_);
			/* code */
			break;

		case SYS_HALT:
			syscall_halt();
			/* code */
			break;

		case SYS_OPEN:
			
            const char *file = (const char*) args[1];
        	if (!valid_user_string(file)) {
            	syscall_exit(-1);
        	}
			f->eax = syscall_open(file);
			/* code */
			break;

		case SYS_READ:
            int fd = args[1];
			void *buffer = (void*) args[2];

			if (!valid_user_buffer(buffer, args[3])) {
				syscall_exit(-1);
			}
			f->eax = syscall_read(fd, buffer, args[3]);
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
            const char *file_name = (const char *) args[1];
			if (!valid_user_string(file_name)) {
				syscall_exit(-1);
			}
			f->eax = syscall_remove(file_name);
			/* code */
			break;

		case SYS_FILESIZE:
            if (!is_valid_user_ptr(&args[1])) {
                syscall_exit(-1);
            }
			f->eax = syscall_filesize(args[1]);
			/* code */
			break;
		case SYS_EXIT:
            if (!valid_user_buffer(&args[1], 2*sizeof(int))) {
                syscall_exit(-1);
            }
			syscall_exit(args[1]);
			/* code */
			break;
		case SYS_EXEC:
            if (!valid_user_buffer(f->esp, 2 * sizeof(int))) {
                syscall_exit(-1);
            }
            const char *cmd_line = (const char*) args[1];
			if (!valid_user_string(cmd_line)) {
				syscall_exit(-1);
			}
			f->eax = syscall_exec(cmd_line);
			/* code */
			break;
        case SYS_WAIT:
			if(!valid_user_buffer(f->esp, 2 *sizeof(int))) return syscall_exit(-1);
            f->eax = syscall_wait(args[1]);
			break;
		default:
			syscall_exit(-1);
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
		file_close(cur_file);
		return -1;
	}
	for (int i = 2; i < 130; i++) {
		if (cur->fd_table[i] == NULL) {
			 cur->fd_table[i] = cur_file;
			 return i;
		}
	}
	file_close(cur_file);
	return -1;
}

void syscall_close (int fd) {
	struct thread *cur = thread_current();
	if ( fd < 2 || fd >= 130) {
		syscall_exit(-1);
	}
	if (cur->fd_table[fd] != NULL) {
		file_close(cur->fd_table[fd]);
		cur->fd_table[fd] = NULL;
	} 
}

int syscall_write (int fd, const void *buffer, unsigned size){

	struct thread *cur = thread_current();
	if (fd == 1) { // STDOUT
		putbuf(buffer, size);
		return size;
	} else if (fd >= 2 && fd < 130)  {
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
	} else if (fd >= 2 && fd < 130) {
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
	if (fd >= 2 && fd < 130) {
		struct file *file = cur->fd_table[fd];
		if(file != NULL){
			return file_length(file);
		}
	}
	return -1;
}

void syscall_seek(int fd, unsigned position){
	struct thread *cur = thread_current();
	if (fd >= 2 && fd < 130) {
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
	unsigned tell = 0;
	if (fd >= 2 && fd < 130) {
		struct file *file = cur->fd_table[fd];
		if(file != NULL){
			tell = file_tell(file);
		}
	}
	return tell;
}

void syscall_exit(int status){
    struct thread *t = thread_current();
    if(t->pc != NULL){ // tells parent it is going kill itself now
		t->pc->exit_status = status;
    }
	printf("%s: exit(%d)\n", t->name, t->pc->exit_status);
    thread_exit();
}

pid_t syscall_exec(const char *cmd_line){
	pid_t pid = process_execute(cmd_line);
	return pid;
}

int syscall_wait(int pid){
	return process_wait(pid);
}


