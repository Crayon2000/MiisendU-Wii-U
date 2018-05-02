#include <dynamic_libs/os_defs.h>
#include <dynamic_libs/os_functions.h>

static void KernelCopyData(unsigned int addr, unsigned int src, unsigned int len){
    /*
     * Setup a DBAT access with cache inhibited to write through and read directly from memory
     */
    unsigned int dbatu0, dbatl0, dbatu1, dbatl1;
    // save the original DBAT value
    asm volatile("mfdbatu %0, 0" : "=r" (dbatu0));
    asm volatile("mfdbatl %0, 0" : "=r" (dbatl0));
    asm volatile("mfdbatu %0, 1" : "=r" (dbatu1));
    asm volatile("mfdbatl %0, 1" : "=r" (dbatl1));

    unsigned int target_dbatu0 = 0;
    unsigned int target_dbatl0 = 0;
    unsigned int target_dbatu1 = 0;
    unsigned int target_dbatl1 = 0;

    unsigned char *dst_p = (unsigned char*)addr;
    unsigned char *src_p = (unsigned char*)src;

    // we only need DBAT modification for addresses out of our own DBAT range
    // as our own DBAT is available everywhere for user and supervisor
    // since our own DBAT is on DBAT5 position we don't collide here
    if(addr < 0x00800000 || addr >= 0x01000000)
    {
        target_dbatu0 = (addr & 0x00F00000) | 0xC0000000 | 0x1F;
        target_dbatl0 = (addr & 0xFFF00000) | 0x32;
        asm volatile("mtdbatu 0, %0" : : "r" (target_dbatu0));
        asm volatile("mtdbatl 0, %0" : : "r" (target_dbatl0));
        dst_p = (unsigned char*)((addr & 0xFFFFFF) | 0xC0000000);
    }
    if(src < 0x00800000 || src >= 0x01000000)
    {
        target_dbatu1 = (src & 0x00F00000) | 0xB0000000 | 0x1F;
        target_dbatl1 = (src & 0xFFF00000) | 0x32;

        asm volatile("mtdbatu 1, %0" : : "r" (target_dbatu1));
        asm volatile("mtdbatl 1, %0" : : "r" (target_dbatl1));
        src_p = (unsigned char*)((src & 0xFFFFFF) | 0xB0000000);
    }

    asm volatile("eieio; isync");

    unsigned int i;
    for(i = 0; i < len; i++)
    {
        // if we are on the edge to next chunk
        if((target_dbatu0 != 0) && (((unsigned int)dst_p & 0x00F00000) != (target_dbatu0 & 0x00F00000)))
        {
            target_dbatu0 = ((addr + i) & 0x00F00000) | 0xC0000000 | 0x1F;
            target_dbatl0 = ((addr + i) & 0xFFF00000) | 0x32;
            dst_p = (unsigned char*)(((addr + i) & 0xFFFFFF) | 0xC0000000);

            asm volatile("eieio; isync");
            asm volatile("mtdbatu 0, %0" : : "r" (target_dbatu0));
            asm volatile("mtdbatl 0, %0" : : "r" (target_dbatl0));
            asm volatile("eieio; isync");
        }
        if((target_dbatu1 != 0) && (((unsigned int)src_p & 0x00F00000) != (target_dbatu1 & 0x00F00000)))
        {
            target_dbatu1 = ((src + i) & 0x00F00000) | 0xB0000000 | 0x1F;
            target_dbatl1 = ((src + i) & 0xFFF00000) | 0x32;
            src_p = (unsigned char*)(((src + i) & 0xFFFFFF) | 0xB0000000);

            asm volatile("eieio; isync");
            asm volatile("mtdbatu 1, %0" : : "r" (target_dbatu1));
            asm volatile("mtdbatl 1, %0" : : "r" (target_dbatl1));
            asm volatile("eieio; isync");
        }

        *dst_p = *src_p;

        ++dst_p;
        ++src_p;
    }

    /*
     * Restore original DBAT value
     */
    asm volatile("eieio; isync");
    asm volatile("mtdbatu 0, %0" : : "r" (dbatu0));
    asm volatile("mtdbatl 0, %0" : : "r" (dbatl0));
    asm volatile("mtdbatu 1, %0" : : "r" (dbatu1));
    asm volatile("mtdbatl 1, %0" : : "r" (dbatl1));
    asm volatile("eieio; isync");
}

/* Read a 32-bit word with kernel permissions */
uint32_t __attribute__ ((noinline)) kern_read(const void *addr)
{
	uint32_t result;
	asm volatile (
		"li 3,1\n"
		"li 4,0\n"
		"li 5,0\n"
		"li 6,0\n"
		"li 7,0\n"
		"lis 8,1\n"
		"mr 9,%1\n"
		"li 0,0x3400\n"
		"mr %0,1\n"
		"sc\n"
		"nop\n"
		"mr 1,%0\n"
		"mr %0,3\n"
		:	"=r"(result)
		:	"b"(addr)
		:	"memory", "ctr", "lr", "0", "3", "4", "5", "6", "7", "8", "9", "10",
			"11", "12"
	);

	return result;
}

/* Write a 32-bit word with kernel permissions */
void __attribute__ ((noinline)) kern_write(void *addr, uint32_t value)
{
	asm volatile (
		"li 3,1\n"
		"li 4,0\n"
		"mr 5,%1\n"
		"li 6,0\n"
		"li 7,0\n"
		"lis 8,1\n"
		"mr 9,%0\n"
		"mr %1,1\n"
		"li 0,0x3500\n"
		"sc\n"
		"nop\n"
		"mr 1,%1\n"
		:
		:	"r"(addr), "r"(value)
		:	"memory", "ctr", "lr", "0", "3", "4", "5", "6", "7", "8", "9", "10",
			"11", "12"
		);
}

void init_kernel_syscalls(){
    kern_write((void*)(OS_SPECIFICS->addr_KernSyscallTbl1 + (0x25 * 4)), (unsigned int)KernelCopyData);
    kern_write((void*)(OS_SPECIFICS->addr_KernSyscallTbl2 + (0x25 * 4)), (unsigned int)KernelCopyData);
    kern_write((void*)(OS_SPECIFICS->addr_KernSyscallTbl3 + (0x25 * 4)), (unsigned int)KernelCopyData);
    kern_write((void*)(OS_SPECIFICS->addr_KernSyscallTbl4 + (0x25 * 4)), (unsigned int)KernelCopyData);
    kern_write((void*)(OS_SPECIFICS->addr_KernSyscallTbl5 + (0x25 * 4)), (unsigned int)KernelCopyData);
}