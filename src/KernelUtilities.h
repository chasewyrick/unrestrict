#ifndef kutils_h
#define kutils_h

#include <common.h>
#include <mach/mach.h>
#include <offsetcache.h>
#include <stdbool.h>

#if 0
Credits:
- https://stek29.rocks/2018/01/26/sandbox.html
- https://stek29.rocks/2018/12/11/shenanigans.html
- http://newosxbook.com/QiLin/qilin.pdf
- https://github.com/Siguza/v0rtex/blob/e6d54c97715d6dbcdda8b9a8090484a7a47019d0/src/v0rtex.m#L1623
#endif

#if 0
TODO:
- Patchfind proc_lock (High priority)
- Patchfind proc_unlock (High priority)
- Patchfind proc_ucred_lock (High priority)
- Patchfind proc_ucred_unlock (High priority)
- Patchfind vnode_lock (Low priority)
- Patchfind vnode_unlock (Low priority)
- Patchfind mount_lock (Low priority)
- Patchfind mount_unlock (Low priority)
- Patchfind task_set_platform_binary (High priority)
- Patchfind kauth_cred_ref (Low priority)
- Patchfind kauth_cred_unref (Low priority)
- Patchfind chgproccnt (Low priority)
- Patchfind kauth_cred_ref (Low priority)
- Patchfind kauth_cred_unref (Low priority)
- Patchfind extension_destroy (Low priority)
- Patchfind extension_create_mach (Middle priority)
- Use offsetof with XNU headers to find structure offsets (Low priority)
- Update Unrestrict to implement the kernel calls
#endif

#define setoffset(offset, val) set_offset(#offset, val)
#define getoffset(offset) get_offset(#offset)

#define OSBoolTrue getOSBool(true)
#define OSBoolFalse getOSBool(false)

extern kptr_t kernel_base;
extern uint64_t kernel_slide;

extern kptr_t cached_task_self_addr;
extern BOOL found_offsets;

kptr_t task_self_addr(void);
kptr_t ipc_space_kernel(void);
kptr_t find_kernel_base(void);

kptr_t current_thread(void);

mach_port_t fake_host_priv(void);

int message_size_for_kalloc_size(int kalloc_size);

kptr_t get_kernel_proc_struct_addr(void);
BOOL iterate_proc_list(void (^handler)(kptr_t, pid_t, BOOL *));
kptr_t get_proc_struct_for_pid(pid_t pid);
kptr_t get_address_of_port(pid_t pid, mach_port_t port);
kptr_t get_kernel_cred_addr(void);
kptr_t give_creds_to_process_at_addr(kptr_t proc, kptr_t cred_addr);
BOOL set_platform_binary(kptr_t proc, BOOL set);

kptr_t zm_fix_addr(kptr_t addr);

BOOL verify_tfp0(void);

extern int (*pmap_load_trust_cache)(kptr_t kernel_trust, size_t length);
int _pmap_load_trust_cache(kptr_t kernel_trust, size_t length);

BOOL set_host_type(host_t host, uint32_t type);
BOOL export_tfp0(host_t host);
BOOL unexport_tfp0(host_t host);

BOOL set_csflags(kptr_t proc, uint32_t flags, BOOL value);
BOOL set_cs_platform_binary(kptr_t proc, BOOL value);

BOOL execute_with_credentials(kptr_t proc, kptr_t credentials, void (^function)(void));

uint32_t get_proc_memstat_state(kptr_t proc);
BOOL set_proc_memstat_state(kptr_t proc, uint32_t memstat_state);
BOOL set_proc_memstat_internal(kptr_t proc, BOOL set);
BOOL get_proc_memstat_internal(kptr_t proc);
size_t kstrlen(kptr_t ptr);
kptr_t kstralloc(const char *str);
BOOL kstrfree(kptr_t ptr);
kptr_t sstrdup(const char *str);
void sfree(kptr_t ptr);
int extension_create_file(kptr_t saveto, kptr_t sb, const char *path, size_t path_len, uint32_t subtype);
int extension_create_mach(kptr_t saveto, kptr_t sb, const char *name, uint32_t subtype);
int extension_add(kptr_t ext, kptr_t sb, const char *desc);
void extension_release(kptr_t ext);
void extension_destroy(kptr_t ext);
BOOL set_file_extension(kptr_t sandbox, const char *exc_key, const char *path);
BOOL set_mach_extension(kptr_t sandbox, const char *exc_key, const char *name);
kptr_t proc_find(pid_t pid);
void proc_rele(kptr_t proc);
void proc_lock(kptr_t proc);
void proc_unlock(kptr_t proc);
void proc_ucred_lock(kptr_t proc);
void proc_ucred_unlock(kptr_t proc);
void vnode_lock(kptr_t vp);
void vnode_unlock(kptr_t vp);
void mount_lock(kptr_t mp);
void mount_unlock(kptr_t mp);
void task_set_platform_binary(kptr_t task, boolean_t is_platform);
void kauth_cred_ref(kptr_t cred);
void kauth_cred_unref(kptr_t cred);
int chgproccnt(uid_t uid, int diff);
kptr_t vfs_context_current(void);
int vnode_lookup(const char *path, int flags, kptr_t *vpp, kptr_t ctx);
int vnode_put(kptr_t vp);
BOOL OSDictionary_SetItem(kptr_t OSDictionary, const char *key, kptr_t val);
kptr_t OSDictionary_GetItem(kptr_t OSDictionary, const char *key);
BOOL OSDictionary_Merge(kptr_t OSDictionary, kptr_t OSDictionary2);
uint32_t OSDictionary_ItemCount(kptr_t OSDictionary);
kptr_t OSDictionary_ItemBuffer(kptr_t OSDictionary);
kptr_t OSDictionary_ItemKey(kptr_t buffer, uint32_t idx);
kptr_t OSDictionary_ItemValue(kptr_t buffer, uint32_t idx);
uint32_t OSArray_ItemCount(kptr_t OSArray);
BOOL OSArray_Merge(kptr_t OSArray, kptr_t OSArray2);
kptr_t OSArray_GetObject(kptr_t OSArray, uint32_t idx);
void OSArray_RemoveObject(kptr_t OSArray, uint32_t idx);
kptr_t OSArray_ItemBuffer(kptr_t OSArray);
kptr_t OSObjectFunc(kptr_t OSObject, uint32_t off);
void OSObject_Release(kptr_t OSObject);
void OSObject_Retain(kptr_t OSObject);
uint32_t OSObject_GetRetainCount(kptr_t OSObject);
uint32_t OSString_GetLength(kptr_t OSString);
kptr_t OSString_CStringPtr(kptr_t OSString);
char *OSString_CopyString(kptr_t OSString);
kptr_t OSUnserializeXML(const char *buffer);
kptr_t get_exception_osarray(const char **exceptions, bool is_file_extension);
char **copy_amfi_entitlements(kptr_t present);
kptr_t getOSBool(BOOL value);
BOOL entitle_process(kptr_t amfi_entitlements, const char *key, kptr_t val);
BOOL set_sandbox_exceptions(kptr_t sandbox);
BOOL check_for_exception(char **current_exceptions, const char *exception);
BOOL set_amfi_exceptions(kptr_t amfi_entitlements, const char *exc_key, const char **exceptions, bool is_file_extension);
BOOL set_exceptions(kptr_t sandbox, kptr_t amfi_entitlements);
kptr_t get_amfi_entitlements(kptr_t cr_label);
kptr_t get_sandbox(kptr_t cr_label);
BOOL entitle_process_with_pid(pid_t pid, const char *key, kptr_t val);
BOOL remove_memory_limit(void);
BOOL restore_kernel_task_port(task_t *out_kernel_task_port);
BOOL restore_kernel_base(uint64_t *out_kernel_base, uint64_t *out_kernel_slide);
BOOL restore_kernel_offset_cache(void);
BOOL restore_file_offset_cache(const char *offset_cache_file_path, kptr_t *out_kernel_base, uint64_t *out_kernel_slide);
BOOL convert_port_to_task_port(mach_port_t port, kptr_t space, kptr_t task_kaddr);
kptr_t make_fake_task(kptr_t vm_map);
BOOL make_port_fake_task_port(mach_port_t port, kptr_t task_kaddr);
BOOL set_hsp4(task_t port);
kptr_t get_vnode_for_path(const char *path);
kptr_t get_vnode_for_snapshot(int fd, char *name);
BOOL set_kernel_task_info(void);
int issue_extension_for_mach_service(kptr_t sb, kptr_t ctx, const char *entry_name, void *desc);
BOOL unrestrict_process(pid_t pid);
BOOL unrestrict_process_with_task_port(task_t task_port);
BOOL revalidate_process(pid_t pid);
BOOL revalidate_process_with_task_port(task_t task_port);

#endif /* kutils_h */
