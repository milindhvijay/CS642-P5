#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sched.h>
#include <unistd.h>
#include <x86intrin.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdlib.h>

// Return the size of a file
long file_size(const char *filename) {
    struct stat s; 
    if (stat(filename,&s) != 0) {
        printf("Error reading file stats !\n");
        return 1;
    }    
    printf("File size = %lu\n", s.st_size);
    return s.st_size; 
}

/* Performs a dummy access
   to the address specified. */
void maccess(void *addr)
{
    asm volatile ("movq (%0), %%rax\n"
        :
        : "c" (addr)
        : "rax");
}

/* Flushes the cache block corresponding
   to the address from the entire cache
   hierarchy. */
void flush(void *addr)
{
    asm volatile("clflush 0(%0)\n"
        :
        : "c" (addr)
        :"rax");
}

/* Reads the Time Stamp Counter. */
uint64_t rdtsc() {
  uint64_t a, d;
  asm volatile ("mfence");
  asm volatile ("rdtsc" : "=a" (a), "=d" (d));
  a = (d<<32) | a;
  asm volatile ("mfence");
  return a;
}

/* combines rdtsc and maccess */
uint64_t measure_one_block_access_time(void* addr)
{
    uint64_t cycles;

    asm volatile("mov %1, %%r8\n\t"
            "lfence\n\t"
            "rdtsc\n\t"
            "mov %%eax, %%edi\n\t"
            "mov (%%r8), %%r8\n\t"
            "lfence\n\t"
            "rdtsc\n\t"
            "sub %%edi, %%eax\n\t"
    : "=a"(cycles) /*output*/
    : "r"(addr)
    : "r8", "edi");

    return cycles;
}

int main(int argc, char* argv[])
{
    int i, j, k;
    uint64_t time;
    int a[64] = {0};
    unsigned long hit_time, miss_time;

    if (argc != 2) {
        printf("Usage: attack hit_time,miss_time\n");
        return 1;
    }

    hit_time = atoi(strtok(argv[1], ","));
    miss_time = atoi(strtok(NULL, ","));
    if (hit_time < 0 || miss_time < 0) {
        printf("Error: Invalid hit/miss time\n");
        return 1;
    }

	char* filename = "Records.csv";
	int fd = open(filename, O_RDONLY);
    unsigned char *addr = (unsigned char *)mmap(0, file_size(filename), PROT_READ, MAP_SHARED, fd, 0);
    if (addr == (void*) -1 || addr == (void*) 0) {
        printf("error: failed to mmap\n");
        return 1;
    }
    
    /* Insert your code here. Things to keep in mind:
       1. Do not scan the cache sets sequentially.
          Use a function such as (index*prime + offset)%(maxrange).
          This is to overcome prefetching.
       2. Use sched_yield() while waiting for victim to access.
       3. Use hit_time and/or miss_time to detect hits and misses.
       4. Monitor the same cache set N times before monitoring the
          next set. (Clue: N~10 should work)
    */
       
    return 0;    
}
