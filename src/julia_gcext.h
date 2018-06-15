// This file is a part of Julia. License is MIT: https://julialang.org/license

/*
  interface for extending the garbage collection process with foreign types
*/

#ifndef JL_GCEXT_H
#define JL_GCEXT_H

// requires including "julia.h" beforehand.

typedef void (*jl_gc_root_scanner_hook_t)(int full);
typedef void (*jl_gc_task_scanner_hook_t)(jl_task_t *task, int full);
typedef void (*jl_pre_gc_hook_t)(int full);
typedef void (*jl_post_gc_hook_t)(int full);
typedef void (*jl_gc_register_bigval_hook_t)(void *addr, size_t size);
typedef void (*jl_gc_deregister_bigval_hook_t)(void *addr);

typedef struct _jl_gc_hooks_t {
  struct _jl_gc_hooks_t *next; // linked list

  // Hook set by foreign code and invoked by Julia during a garbage collection
  // when walking the GC roots
  jl_gc_root_scanner_hook_t root_scanner_hook;

  // Hook set by foreign code and invoked by Julia whenever a task is being scanned
  jl_gc_task_scanner_hook_t task_scanner_hook;

  // Hook set by foreign code and invoked by Julia whenever a garbage collection begins
  jl_pre_gc_hook_t pre_gc_hook;

  // Hook set by foreign code and invoked by Julia whenever a garbage collection ends
  jl_post_gc_hook_t post_gc_hook;

  // Hooks to register `bigval_t` instances
  jl_gc_register_bigval_hook_t register_bigval;

  // Hooks to deregister `bigval_t` instances
  jl_gc_deregister_bigval_hook_t deregister_bigval;
} jl_gc_hooks_t;


// register global gc hooks
JL_DLLEXPORT extern void jl_register_gc_hooks(jl_gc_hooks_t *hooks);



// Types for mark and finalize functions.
// We make the cache and sp parameters opaque so that the internals
// do not get exposed.
typedef uintptr_t (*jl_markfunc_t)(jl_ptls_t, jl_value_t *obj);
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
//
// Returns the allocated size of a Julia object.
JL_DLLEXPORT size_t jl_gc_alloc_size(jl_value_t *p);


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

JL_DLLEXPORT void * jl_gc_alloc_typed(jl_ptls_t ptls, size_t sz, void *ty);
JL_DLLEXPORT int jl_gc_mark_queue_obj(jl_ptls_t ptls, jl_value_t *obj);

JL_DLLEXPORT void jl_gc_set_needs_foreign_finalizer(jl_value_t *obj);

#endif // _JULIA_GCEXT_H
