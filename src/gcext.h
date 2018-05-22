#ifndef _JULIA_GCEXT_H
#define _JULIA_GCEXT_H

// requires including "julia.h" beforehand.

// Invoke this function whenever a garbage collection starts
JL_DLLEXPORT extern void (*jl_root_scanner_hook)(int full);

// Invoke this function whenever a garbage collection begins
JL_DLLEXPORT extern void (*jl_pre_gc_hook)(int full);

// Invoke this function whenever a garbage collection ends
JL_DLLEXPORT extern void (*jl_post_gc_hook)(int full);

// This function is called to set elements of the GC context
JL_DLLEXPORT extern void (*jl_gc_set_context_hook)(int tid, int index,
    void *data);

// Size of the GC context in machine words
#define JL_GC_CONTEXT_SIZE 3

#define JL_GC_CONTEXT_TLS 0
#define JL_GC_CONTEXT_CACHE 1
#define JL_GC_CONTEXT_SP 2

// Disable generational GC
JL_DLLEXPORT extern int jl_gc_disable_generational;


// Invoke this function whenever a task is being scanned
JL_DLLEXPORT extern void (*jl_task_scanner_hook)(jl_task_t *task,
  int root_task);


// Invoke these functions to allocate `bigval_t` instances.
JL_DLLEXPORT extern void *(*jl_nonpool_alloc_hook)(size_t size);
JL_DLLEXPORT extern void (*jl_nonpool_free_hook)(void *p);

// Types for mark and finalize functions.
// We make the cache and sp parameters opaque so that the internals
// do not get exposed.
typedef void (*jl_markfunc_t)(void *obj);
typedef void (*jl_finalizefunc_t)(void *obj);

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

JL_DLLEXPORT size_t jl_extend_gc_max_pool_obj_size(void);


// Returns the base address of a memory block, assuming it
// is stored in a julia memory pool. Return NULL otherwise.
JL_DLLEXPORT jl_value_t *jl_pool_base_ptr(void *p);

// Returns 1 if the argument points to actual memory that contains
// or may contain an internal Julia object or 0 if it doesn't.
//
// Furthermore, on success the tag will either be valid tag if p refers
// to a live object or point to an address that isn't one if it is
// invalid.
JL_DLLEXPORT int jl_is_internal_obj_alloc(jl_value_t *p);

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

JL_DLLEXPORT jl_ptls_t jl_extend_get_ptls_states(void);
JL_DLLEXPORT void * jl_extend_gc_alloc(void **context, size_t sz, void *t);
JL_DLLEXPORT void jl_extend_init(void);
JL_DLLEXPORT int jl_gc_mark_queue_obj(void **context, void *obj);

JL_DLLEXPORT void jl_gc_mark_push_remset(void **context, void *obj,
  uintptr_t nptr);

JL_DLLEXPORT void jl_extend_gc_set_needs_finalizer(void *obj);

#endif // _JULIA_GCEXT_H
