#include <stdio.h>
#include <stdlib.h>
#include <mach/mach.h>
#include <sys/types.h>
#include <sys/sysctl.h>

int main() {
    int mib[2];
    int64_t physical_memory;
    size_t length;

    // Get total physical memory
    mib[0] = CTL_HW;
    mib[1] = HW_MEMSIZE;
    length = sizeof(physical_memory);
    if (sysctl(mib, 2, &physical_memory, &length, NULL, 0) == -1) {
        perror("sysctl");
        return 1;
    }

    // Get memory usage
    vm_size_t page_size;
    mach_port_t mach_port = mach_host_self();
    vm_statistics64_data_t vm_stats;
    mach_msg_type_number_t count = sizeof(vm_stats) / sizeof(natural_t);
    if (host_page_size(mach_port, &page_size) != KERN_SUCCESS || host_statistics64(mach_port, HOST_VM_INFO, (host_info64_t)&vm_stats, &count) != KERN_SUCCESS) {
        fprintf(stderr, "Failed to get VM statistics\n");
        return 1;
    }

    int64_t free_memory = (int64_t)vm_stats.free_count * (int64_t)page_size;
    int64_t used_memory = physical_memory - free_memory;
    double used_percentage = (double)used_memory / physical_memory * 100;
    double free_percentage = 100.0 - used_percentage;

    printf("Total Physical Memory: %lld Bytes\n", physical_memory);
    printf("Used Memory: %lld Bytes (%.2f%%)\n", used_memory, used_percentage);
    printf("Free Memory: %lld Bytes (%.2f%%)\n", free_memory, free_percentage);

    return 0;
}