#include <common.h>
#include <KernelMemory.h>
#include <KernelExecution.h>
#include <KernelOffsets.h>
#include <KernelUtilities.h>
#include <utils.h>

static BOOL unrestrict_initialized = NO;

void unrestrict_initialize(void) {
    LOG("Initializing unrestrict");
    if (unrestrict_initialized) {
        LOG("Already initialized");
        return;
    }
    if (!restore_kernel_task_port(&tfp0)) {
        LOG("Unable to restore kernel task port");
        return;
    }
    if (!restore_kernel_base(&kernel_base, &kernel_slide)) {
        LOG("Unable to restore kernel_base");
        return;
    }
    if (!restore_kernel_offset_cache()) {
        LOG("Unable to restore kernel offset cache");
        return;
    }
    if (!init_kexec()) {
        LOG("Unable to initialize kernel code execution");
        return;
    }
    if (!remove_memory_limit()) {
        LOG("Unable to remove memory limit");
        return;
    }
    LOG("Initialized unrestrict successfully");
    unrestrict_initialized = YES;
}

void unrestrict_deinitialize(void) {
    LOG("Deinitializing unrestrict");
    term_kexec();
    unrestrict_initialized = NO;
}

BOOL MSunrestrict0(mach_port_t task_port) {
    if (!unrestrict_initialized) return true;
    return unrestrict_process_with_task_port(task_port);
}

BOOL MSrevalidate0(mach_port_t task_port) {
    if (!unrestrict_initialized) return true;
    return revalidate_process_with_task_port(task_port);
}

#ifdef HAVE_MAIN

__attribute__((constructor))
static void ctor() {
    unrestrict_initialize();
}

__attribute__((destructor))
static void dtor() {
    unrestrict_deinitialize();
}

#endif
