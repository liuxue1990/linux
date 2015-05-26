#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/pid.h>
#include <linux/slab.h>
#include <asm/pgtable.h>


#define MAX_FILE_PATH_LEN 30
#define PTE_TABLE_SIZE 10
/**
 * @brief helper definition for iterating vm list
 * @details [long description]
 */
#define list_for_each_vm(pos, head) \
	for (pos = (head); pos != NULL; pos = pos->vm_next)
/**
 * @brief helper function for calculating size of the memory address
 * @details [long description]
 */
#define calc_size(start, end) \
	(end - start) / 1024

#define printk_address(mesg, start, end) \
	printk(mesg" %8lx%8luKB ", start, calc_size(start, end))

#define printk_address_ln(mesg, start, end) \
	printk_address(mesg, start, end); \
	printk("\n")

pte_t* pte_table[PTE_TABLE_SIZE];
int cur_pte_table_entry = 0;

void print_vm_flag(unsigned long vm_flags){
	int i;
	for (i = 0; i < 32; ++i)
	{
		switch(vm_flags & ( 0x1 << i)){
			case 0x00000001:
				printk("VM_READ|");
				break;
			case 0x00000002:
				printk("VM_WRITE|");
				break;
			case 0x00000004:
				printk("VM_EXEC|");
				break;
			case 0x00000008:
				printk("VM_SHARED|");
				break;
			case 0x00000010:
				printk("VM_MAYREAD|");
				break;
			case 0x00000020:
				printk("VM_MAYWRITE|");
				break;
			case 0x00000040:
				printk("VM_MAYEXEC|");
				break;
			case 0x00000080:
				printk("VM_MAYSHARE|");
				break;
			case 0x00000100:
				printk("VM_GROWSDOWN|");
				break;
			case 0x00000200:
				printk("VM_GROWSUP|");
				break;
			case 0x00000400:
				printk("VM_PFNMAP|");
				break;
			case 0x00000800:
				printk("VM_DENYWRITE|");
				break;
			case 0x00001000:
				printk("VM_EXECUTABLE|");
				break;
			case 0x00002000:
				printk("VM_LOCKED|");
				break;
			case 0x00004000:
				printk("VM_IO|");
				break;
			case 0x00008000:
				printk("VM_SEQ_READ|");
				break;
			case 0x00010000:
				printk("VM_RAND_READ|");
				break;
			case 0x00020000:
				printk("VM_DONTCOPY|");
				break;
			case 0x00040000:
				printk("VM_DONTEXPAND|");
				break;
			case 0x00080000:
				printk("VM_RESERVED|");
				break;
			case 0x00100000:
				printk("VM_ACCOUNT|");
				break;
			case 0x00200000:
				printk("VM_NORESERVE|");
				break;
			case 0x00400000:
				printk("VM_HUGETLB|");
				break;
			case 0x00800000:
				printk("VM_NONLINEAR|");
				break;
			case 0x01000000:
				printk("VM_MAPPED_COPY|");
				break;
			case 0x02000000:
				printk("VM_INSERTPAGE|");
				break;
			case 0x04000000:
				printk("VM_NODUMP|");
				break;
			case 0x08000000:
				printk("VM_CAN_NONLINEAR|");
				break;
			case 0x10000000:
				printk("VM_MIXEDMAP|");
				break;
			case 0x20000000:
				printk("VM_SAO|");
				break;
			case 0x40000000:
				printk("VM_PFN_AT_MMAP|");
				break;
			case 0x80000000:
				printk("VM_MERGEABLE|");
				break;
			default:
				break;
		}
	}
	printk("\b\n");
}

void print_vm_file(struct file *vm_file){
	char* buf, *path;
	buf = (char*)kmalloc(MAX_FILE_PATH_LEN, GFP_KERNEL);
	if(buf == NULL){
		printk("ERROR kmalloc");
		return;
	}
	memset(buf, '\0', MAX_FILE_PATH_LEN);
	if (vm_file != NULL)
	{
		path = dentry_path_raw(vm_file->f_path.dentry, buf, MAX_FILE_PATH_LEN - 1);
	}else{
		path = "";
	}
	printk("%-20s ", path);
	kfree(buf);
}

void print_vm_area_struct(struct vm_area_struct *mmap){
	struct vm_area_struct *p;
	list_for_each_vm(p, mmap){
		printk_address("",p->vm_start, p->vm_end);
		// printk("%lu ", pgprot_val(p->vm_page_prot));
		// print_vm_file(p->vm_file);
		print_vm_flag(p->vm_flags);
	}
}

void print_mm_struct(struct mm_struct *mm){
	printk_address_ln("       code:", mm->start_code, mm->end_code);
	printk_address_ln("       data:", mm->start_data, mm->end_data);
	printk_address_ln("       heap:", mm->start_brk, mm->brk);
	printk_address_ln("      stack:", mm->start_stack, mm->start_stack);
	printk_address_ln("  arguments:", mm->arg_start, mm->arg_end);
	printk_address_ln("environment:", mm->env_start, mm->env_end);
}



// void print_pa(struct mm_struct *mm, unsigned long va){
// 	pgd_t *pgd = pgd_offset(mm, va);
// 	if (pgd_present(*pgd))
// 	{
// 		pmd_t *pmd = pmd_offset(pgd, va);
// 		if (pmd_present(*pmd))
// 		{
// 			pte_t *pte = pte_offset_map(pmd , va);
// 			if(pte_present(*pte)){
// 				printk("va 0x%lx -> pa 0x%lx \n", va, page_address(pte_page(*pte)));
// 			}
// 		}
// 	}
// }

// void print_pages(struct mm_struct *mm, long start, long end){
// 	// long va;
// 	// for (va = start; va < end; ++va)
// 	// {
// 	// 	print_pa(mm, va);
// 	// }
// 	print_pa(mm, start);
// 	printk("0x%lx\n", virt_to_phys(start));
// 	print_pa(mm, end);
// 	printk("0x%lx\n", virt_to_phys(end));
// }

void add_pte_table(pte_t* pte_table_entry){
	pte_table[cur_pte_table_entry] = pte_table_entry;
	cur_pte_table_entry++;
	if(cur_pte_table_entry == MAX_FILE_PATH_LEN){
		printk("pte table overflow!");
	}
}
int is_in_pte_table(pte_t* pte_table_entry){
	int i;
	for (i = 0; i <= cur_pte_table_entry; ++i)
	{
		if(pte_table[i] == pte_table_entry){
			return true;
		}
	}
	return false;
}
void print_pte_table(pte_t *pte_table_base, int pgd_index){
	pte_t *pte;
	int i, last_vaild_i = 0, is_in_range = 0, is_first = true;

	if(is_in_pte_table(pte_table_base)){
		return;
	}

	add_pte_table(pte_table_base);
	printk("PTE table Base address  %lx \nPGD index %x\n", *pte_val(pte_table_base), pgd_index);
	for (i = 0; i < PTRS_PER_PTE; ++i)
	{
		pte = pte_table_base + i;
		if (*pte_val(pte) != 0 && pte_present(*pte))
		{
			if(i - last_vaild_i == 1){
				is_in_range += 1;
			}else{
				if (is_first)
				{
					printk("PTE%4x --- ", i);
					is_in_range = 0;
					is_first = false;
				}else{
					printk("%4x (%d)\nPTE%4x --- ", last_vaild_i, is_in_range + 1, i);
					is_in_range = 0;
				}
			}
			last_vaild_i = i;
		}
	}
	printk("%4x (%d)\n", last_vaild_i, is_in_range + 1);
}

void print_pgd_table(struct mm_struct *mm){
	pgd_t *pgd;
	int i, last_vaild_i = 0, is_in_range = 0, is_first = true;
	printk("PGD table Base address  %lx \n", *pgd_val(mm->pgd));
	for (i = 0; i < PTRS_PER_PGD; ++i)
	{
		pgd = mm->pgd + i;
		if (*pgd_val(pgd) != 0)
		{
			if(i - last_vaild_i == 1){
				is_in_range += 1;
			}else{
				if (is_first)
				{
					printk("PGD%4x --- ", i);
					is_in_range = 0;
					is_first = false;
				}else{
					printk("%4x (%d)\nPGD%4x --- ", last_vaild_i, is_in_range + 1, i);
					is_in_range = 0;
				}
			}
			
			last_vaild_i = i;
		}
	}
	printk("%4x (%d)\n", last_vaild_i, is_in_range + 1);
}

void print_vm_area_details(struct mm_struct *mm){
	struct vm_area_struct *p;
	// long add;
	list_for_each_vm(p, mm->mmap){
		print_pte_table(*pgd_offset(mm, p->vm_start), pgd_index(p->vm_start));
		print_pte_table(*pgd_offset(mm, p->vm_end - 4), pgd_index(p->vm_end - 4));
	}
}

/**
 * @brief my own syscall for printing memory map
 * @details [long description]
 * 
 * @param p the syscall name
 * @param g the pid of process need to print the memory map
 */

SYSCALL_DEFINE1(print_map, long, my_pid){
	struct pid *my_task_pid;
	struct task_struct *my_task;
	struct mm_struct *my_mm;
	my_task_pid = find_vpid(my_pid);
	if(my_task_pid == NULL){
		printk("ERROR get my_task_pid");
		return 1;
	}
	my_task = get_pid_task(my_task_pid, PIDTYPE_PID);
	if(my_task == NULL){
		printk("ERROR get my_task");
		return 1;
	}
	my_mm = get_task_mm(my_task);
	if(my_mm == NULL){
		printk("ERROR get my_mm");
		return 1;
	}
	printk("%s[%d]\n", my_task->comm, my_task->pid);
	print_mm_struct(my_mm);
	// print_vm_area_struct(my_mm->mmap);
	print_pgd_table(my_mm);
	print_vm_area_details(my_mm);
	printk("My_pid is %lu\n", my_pid);
	return 0;
}
