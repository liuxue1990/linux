#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <asm/uaccess.h>

#define BUFFER_SIZE 512

struct file* file_open(const char* path, int oflags, umode_t mode);
int file_read(struct file* file, unsigned long long offset, unsigned char* data, unsigned int size);
void file_close(struct file* file);

/**
 * @brief my own syscall for restore process from files
 * @details [long description]
 * 
 * @param restore the syscall name
 * @param path the path to the restored file
 */

SYSCALL_DEFINE1(restore, const char*, path){
	struct file* my_file;
	int ret_in;
	unsigned char buffer[BUFFER_SIZE];
    printk("The file is %s\n", path);
	my_file = file_open(path, O_RDONLY, 0);
	if(!my_file){
		pr_err("Cannot open file %s\n", path);
		return 1;
	}

	ret_in = file_read(my_file, 0, buffer, BUFFER_SIZE);
	if(ret_in <= 0){
		pr_err("read file error %d", ret_in);
		return 1;
	}

	printk("%s  %d\n", buffer, ret_in);
	file_close(my_file);
	return 0;
}


/**
 * @brief open a file
 * @details open a file in the kernel space just like the open syscall
 * 
 * @param path the path to the file
 * @param flags [description]
 * @param rights [description]
 * @return [description]
 */
struct file* file_open(const char* path, int oflags, umode_t mode) {
    struct file* filp = NULL;
    mm_segment_t oldfs;
    int err = 0;

    oldfs = get_fs();
    set_fs(get_ds());
    filp = filp_open(path, oflags, mode);
    set_fs(oldfs);
    if(IS_ERR(filp)) {
        err = PTR_ERR(filp);
        return NULL;
    }
    return filp;
}
/**
 * @brief close the file
 * @details close the file similar to close syscall
 * 
 * @param file [description]
 */
void file_close(struct file* file) {
    filp_close(file, NULL);
}

/**
 * @brief read data from a file
 * @details read the file similar to pread syscall
 * 
 * @param file [description]
 * @param long [description]
 * @param char [description]
 * @param int [description]
 * @return [description]
 */
int file_read(struct file* file, unsigned long long offset, unsigned char* data, unsigned int size) {
    mm_segment_t oldfs;
    int ret;

    oldfs = get_fs();
    set_fs(get_ds());

    ret = vfs_read(file, data, size, &offset);

    set_fs(oldfs);
    return ret;
}   
