#ifndef _JULIA_GCEXT_H
#define _JULIA_GCEXT_H

// requires including "julia.h" beforehand.

// Invoke this function whenever a garbage collection starts
JL_DLLEXPORT extern void (*jl_root_scanner_hook)(int global);

// Invoke this function whenever a task is being scanned
JL_DLLEXPORT extern void (*jl_task_root_scanner_hook)(jl_task_t *task,
  int global);


// Invoke these functions to allocate `bigval_t` instances.
JL_DLLEXPORT extern void (*jl_nonpool_alloc_hook)(size_t size);
JL_DLLEXPORT extern void (*jl_nonpool_free_hook)(void *p);

// Types for mark and sweep functions.
// We make the cache and sp parameters opaque so that the internals
// do not get exposed.
typedef void (*jl_markfunc_t*)(void *cache, void *sp, void *obj);
typedef void (*jl_sweepfunc_t*)(void *obj);

// Function to create a new foreign type with custom
// mark and sweep functions.
JL_DLLEXPORT jl_datatype_t *jl_new_foreign_type(
  jl_sym_t *name,
  jl_module_t *module,
  jl_datatype_t *super,
  jl_markfunc_t markfunc,
  jl_sweepfunc_t sweepfunc
);

// Returns the base address of a memory block, assuming it
// is stored in a julia memory pool. Return NULL otherwise.
JL_DLLEXPORT jl_value_t jl_pool_base_ptr(void *p);

// Field layout descriptor for custom types that do
// not fit Julia layout conventions. This is associated with
// jl_datatype_t instances where fielddesc_type == 3.
//
// The list of functions is extensible so that we can in the future
// add support for enumerating fields and such.

typedef struct {
  jl_markfunc_t markfunc;
  // jl_sweepfunc_t sweepfunc;
} jl_fielddescdyn_t;

#endif // _JULIA_GCEXT_H
