#ifndef JL_GCEXT_H
#define JL_GCEXT_H

// requires including "julia.h" beforehand.

typedef void *jl_gc_context_t;
typedef void (*jl_gc_root_scanner_hook_t)(int full);
typedef void (*jl_gc_task_scanner_hook_t)(jl_task_t *task, int full);
typedef void (*jl_pre_gc_hook_t)(int full);
typedef void (*jl_post_gc_hook_t)(int full);
typedef void (*jl_gc_context_hook_t)(int tid, int index, jl_gc_context_t ctx);


// Hook to invoke whenever a garbage collection starts
JL_DLLEXPORT extern void jl_set_gc_root_scanner_hook(jl_gc_root_scanner_hook_t hook);
JL_DLLEXPORT extern jl_gc_root_scanner_hook_t jl_get_gc_root_scanner_hook(void);

// Hook to invoke whenever a garbage collection begins
JL_DLLEXPORT extern void jl_set_pre_gc_hook(jl_pre_gc_hook_t hook);
JL_DLLEXPORT extern jl_pre_gc_hook_t jl_get_pre_gc_hook(void);

// Hook to invoke whenever a garbage collection ends
JL_DLLEXPORT extern void jl_set_post_gc_hook(jl_post_gc_hook_t hook);
JL_DLLEXPORT extern jl_post_gc_hook_t jl_get_post_gc_hook(void);

// Hook to invoke whenever a task is being scanned
JL_DLLEXPORT extern void jl_set_gc_task_scanner_hook(jl_gc_task_scanner_hook_t hook);
JL_DLLEXPORT extern jl_gc_task_scanner_hook_t jl_get_gc_task_scanner_hook(void);

// Hook to invoke for setting (parts of the) GC context
JL_DLLEXPORT extern void jl_set_gc_context_hook(jl_gc_context_hook_t hook);
JL_DLLEXPORT extern jl_gc_context_hook_t jl_get_gc_context_hook(void);


// Size of the GC context in machine words
#define JL_GC_CONTEXT_SIZE 3

#define JL_GC_CONTEXT_TLS 0
#define JL_GC_CONTEXT_CACHE 1
#define JL_GC_CONTEXT_SP 2

typedef void *(*jl_gc_external_obj_alloc_hook_t)(size_t size);
typedef void (*jl_gc_external_obj_free_hook_t)(void *addr);

// Hooks to invoke to allocate `bigval_t` instances.
JL_DLLEXPORT extern void jl_set_gc_external_obj_alloc_hook(jl_gc_external_obj_alloc_hook_t hook);
JL_DLLEXPORT extern jl_gc_external_obj_alloc_hook_t jl_get_gc_external_obj_alloc_hook(void);
JL_DLLEXPORT extern void jl_set_gc_external_obj_free_hook(jl_gc_external_obj_free_hook_t hook);
JL_DLLEXPORT extern jl_gc_external_obj_free_hook_t jl_get_gc_external_obj_free_hook(void);

// Types for mark and finalize functions.
// We make the cache and sp parameters opaque so that the internals
// do not get exposed.
typedef uintptr_t (*jl_markfunc_t)(int tid, jl_value_t *obj);
typedef void (*jl_finalizefunc_t)(jl_value_t *obj);

// Function to create a new foreign type with custom
// mark and finalize functions.
JL_DLLEXPORT jl_datatype_t *jl_new_foreign_type(
  jl_sym_t *name,
  jl_module_t *module,
  jl_datatype_t *super,
  jl_markfunc_t markfunc,
  jl_finalizefunc_t finalizefunc,
  int haspointers,
  int large
);

JL_DLLEXPORT size_t jl_gc_max_internal_obj_size(void);
JL_DLLEXPORT size_t jl_gc_external_obj_hdr_size(void);


// Returns the base address of a memory block, assuming it
// is stored in a julia memory pool. Return NULL otherwise.
JL_DLLEXPORT jl_value_t *jl_gc_internal_obj_base_ptr(void *p);

// Returns 1 if the argument points to actual memory that contains
// or may contain an internal Julia object or 0 if it doesn't.
//
// Furthermore, on success the tag will either be valid tag if p refers
// to a live object or point to an address that isn't one if it is
// invalid.
JL_DLLEXPORT int jl_gc_is_internal_obj_alloc(jl_value_t *p);

// Field layout descriptor for custom types that do
// not fit Julia layout conventions. This is associated with
// jl_datatype_t instances where fielddesc_type == 3.
//
// The list of functions is extensible so that we can in the future
// add support for enumerating fields and such.

typedef struct {
  jl_markfunc_t markfunc;
  jl_finalizefunc_t finalizefunc;
} jl_fielddescdyn_t;

JL_DLLEXPORT void * jl_gc_alloc_typed(jl_gc_context_t *context, size_t sz, void *ty);
JL_DLLEXPORT int jl_gc_mark_queue_obj(jl_gc_context_t *context, jl_value_t *obj);
JL_DLLEXPORT void jl_gc_mark_push_remset(jl_gc_context_t *context, jl_value_t *obj,
  uintptr_t nptr);

JL_DLLEXPORT void jl_gc_set_needs_foreign_finalizer(jl_value_t *obj);
JL_DLLEXPORT void jl_init_gc_context(void);

#endif // _JULIA_GCEXT_H
