#include <common.h>
#include <KernelMemory.h>
#include <KernelExecution.h>
#include <KernelOffsets.h>
#include <KernelUtilities.h>
#include <utils.h>
#include <patchfinder64.h>

static BOOL unrestrict_initialized = NO;

static BOOL ensure_offsets(void) {
    auto ret = NO;
#if !__arm64e__
    if (!have_kmem_read() || !KERN_POINTER_VALID(kernel_base)) goto out;
    auto missing = NO;
#define ensure_offset(x) do { \
    if (!KERN_POINTER_VALID(getoffset(x))) { \
        LOG("Missing offset: %s", #x); \
        if (!missing) { \
            if (init_kernel(kread, kernel_base, NULL) != 0) { \
                LOG("Unable to initialize patchfinder"); \
                goto out; \
            } \
            missing = YES; \
        } \
        LOG("Finding offset: %s", #x); \
        setoffset(x, find_ ##x()); \
        if (!KERN_POINTER_VALID(getoffset(x))) { \
            LOG("Unable to find offset: %s", #x); \
            goto out; \
        } \
    } \
} while (false)
    ensure_offset(OSBoolean_True);
    ensure_offset(osunserializexml);
    ensure_offset(smalloc);
    ensure_offset(add_x0_x0_0x40_ret);
    ensure_offset(zone_map_ref);
    ensure_offset(kernel_task);
    ensure_offset(proc_find);
    ensure_offset(proc_rele);
    ensure_offset(extension_create_file);
    ensure_offset(extension_add);
    ensure_offset(extension_release);
    ensure_offset(sfree);
    ensure_offset(sstrdup);
    ensure_offset(strlen);
    ensure_offset(issue_extension_for_mach_service);
#undef ensure_offset
    if (missing) {
        set_kernel_task_info();
        missing = NO;
    }
    ret = YES;
out:;
    term_kernel();
#endif
    return ret;
}

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
    if ((!restore_kernel_base(&kernel_base, &kernel_slide) ||
        !restore_kernel_offset_cache()) &&
        !restore_file_offset_cache("/jb/offsets.plist", &kernel_base, &kernel_slide)) {
        LOG("Unable to restore offset cache");
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
    if (!ensure_offsets()) {
        LOG("Unable to ensure offsets");
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
