#include "filesys/file.h"

#include "filesys/inode.h"
#include "threads/malloc.h"
#include "threads/synch.h"

#include <debug.h>

/* An open file. */
struct file {
	struct inode* inode; /* File's inode. */
	off_t pos;				/* Current position. */
	struct lock cursor_lock;

};

/* Opens a file for the given INODE, of which it takes ownership,
	and returns the new file.  Returns a null pointer if an
	allocation fails or if INODE is null. */
struct file* file_open(struct inode* inode)
{
	struct file* file = calloc(1, sizeof *file);
	if (inode != NULL && file != NULL) {
		lock_init(&file->cursor_lock);
		file->inode = inode;
		file->pos = 0;
		return file;
	}
	else {
		inode_close(inode);
		free(file);
		return NULL;
	}
}

/* Opens and returns a new file for the same inode as FILE.
	Returns a null pointer if unsuccessful. */
struct file* file_reopen(struct file* file)
{
	return file_open(inode_reopen(file->inode));
}

/* Closes FILE. */
void file_close(struct file* file)
{
	if (file != NULL) {
		inode_close(file->inode);
		free(file);
	}
}

/* Returns the inode encapsulated by FILE. */
struct inode* file_get_inode(struct file* file)
{
	return file->inode;
}

/* Reads SIZE bytes from FILE into BUFFER,
	starting at the file's current position.
	Returns the number of bytes actually read,
	which may be less than SIZE if end of file is reached.
	Advances FILE's position by the number of bytes read. */
off_t file_read(struct file* file, void* buffer, off_t size)
{
	off_t bytes_read = inode_read_at(file->inode, buffer, size, file->pos);
	file->pos += bytes_read;
	return bytes_read;
}

/* Reads SIZE bytes from FILE into BUFFER,
	starting at offset FILE_OFS in the file.
	Returns the number of bytes actually read,
	which may be less than SIZE if end of file is reached.
	The file's current position is unaffected. */
off_t file_read_at(struct file* file, void* buffer, off_t size, off_t file_ofs)
{
	return inode_read_at(file->inode, buffer, size, file_ofs);
}

/* Writes SIZE bytes from BUFFER into FILE,
	starting at the file's current position.
	Returns the number of bytes actually written,
	which may be less than SIZE if end of file is reached.
	(Normally we'd grow the file in that case, but file growth is
	not yet implemented.)
	Advances FILE's position by the number of bytes read. */
off_t file_write(struct file* file, const void* buffer, off_t size)
{
	off_t bytes_written = inode_write_at(file->inode, buffer, size, file->pos);
	file->pos += bytes_written;
	return bytes_written;
}

/* Writes SIZE bytes from BUFFER into FILE,
	starting at offset FILE_OFS in the file.
	Returns the number of bytes actually written,
	which may be less than SIZE if end of file is reached.
	(Normally we'd grow the file in that case, but file growth is
	not yet implemented.)
	The file's current position is unaffected. */
off_t file_write_at(struct file* file, const void* buffer, off_t size, off_t file_ofs)
{
	return inode_write_at(file->inode, buffer, size, file_ofs);
}

/* Returns the size of FILE in bytes. */
off_t file_length(struct file* file)
{
	ASSERT(file != NULL);
	return inode_length(file->inode);
}

/* Sets the current position in FILE to NEW_POS bytes from the
	start of the file. */
void file_seek(struct file* file, off_t new_pos)
{
	ASSERT(file != NULL);
	ASSERT(new_pos >= 0);
		//lock_acquire(&file->cursor_lock);

	file->pos = new_pos;
		//lock_release(&file->cursor_lock);

	
}

/* Returns the current position in FILE as a byte offset from the
	start of the file. */
off_t file_tell(struct file* file)
{
	ASSERT(file != NULL);
	//lock_acquire(&file->cursor_lock);
	off_t pos = file->pos;
	//lock_release(&file->cursor_lock);
	return pos;


}
