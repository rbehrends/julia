#ifndef JL_GCEXT_H
#define JL_GCEXT_H

// requires including "julia.h" beforehand.

// Kinds of callbacks. For each kind of callback, there must be
// a corresponding type with the same name, but in all lowercase,
// and with a "_t" suffix.

typedef enum {
    JL_GC_CB_root_scanner,
    JL_GC_CB_task_scanner,
    JL_GC_CB_pre_gc,
    JL_GC_CB_post_gc,
    JL_GC_CB_notify_external_alloc,
    JL_GC_CB_notify_external_free,
    // number of callbacks:
    JL_GC_NUM_CALLBACKS
} jl_gc_callback_t;

typedef void (*jl_gc_cb_func_t)(void);

JL_DLLEXPORT void _jl_gc_register_callback(jl_gc_callback_t cb,
    jl_gc_cb_func_t fn);
JL_DLLEXPORT void _jl_gc_deregister_callback(jl_gc_callback_t cb,
    jl_gc_cb_func_t fn);

// The following macros are a workaround to account for the lack of
// generics in C prior to C11, while keeping invocation typesafe.
// Assignment to local variables both prevents repeated evaluation
// and ensures that the function signature matches.

#define jl_gc_register_callback(cb, func) \
    do { \
        jl_gc_callback_t _cb = JL_GC_CB_##cb; \
        jl_gc_cb_##cb##_t _func = func; \
        _jl_gc_register_callback(_cb, (jl_gc_cb_func_t) _func); \
    } while (0)
#define jl_gc_deregister_callback(cb, func) \
    do { \
        jl_gc_callback_t _cb = JL_GC_CB_##cb; \
        jl_gc_cb_##cb##_t _func = func; \
        _jl_gc_deregister_callback(_cb, (jl_gc_cb_func_t) _func); \
    } while (0)


typedef void (*jl_gc_cb_root_scanner_t)(int full);
typedef void (*jl_gc_cb_task_scanner_t)(jl_task_t *task, int full);
typedef void (*jl_gc_cb_pre_gc_t)(int full);
typedef void (*jl_gc_cb_post_gc_t)(int full);
typedef void (*jl_gc_cb_notify_external_alloc_t)(void *addr, size_t size);
typedef void (*jl_gc_cb_notify_external_free_t)(void *addr);

// Types for mark and sweep functions.
// We make the cache and sp parameters opaque so that the internals
// do not get exposed.
typedef uintptr_t (*jl_markfunc_t)(jl_ptls_t, jl_value_t *obj);
typedef void (*jl_sweepfunc_t)(jl_value_t *obj);

// Function to create a new foreign type with custom
// mark and sweep functions.
JL_DLLEXPORT jl_datatype_t *jl_new_foreign_type(
  jl_sym_t *name,
  jl_module_t *module,
  jl_datatype_t *super,
  jl_markfunc_t markfunc,
  jl_sweepfunc_t sweepfunc,
  int haspointers,
  int large
);

JL_DLLEXPORT size_t jl_gc_max_internal_obj_size(void);
JL_DLLEXPORT size_t jl_gc_external_obj_hdr_size(void);

// The following function must be called to enable conservative
// scanning, i.e. if you wish to use `jl_gc_internal_obj_base_ptr()`
// or `jl_gc_is_internal_obj_alloc()`. It has to be called before
// calling `jl_init()`.
JL_DLLEXPORT void jl_gc_enable_conservative_scanning(void);

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

typedef struct {
    jl_markfunc_t markfunc;
    jl_sweepfunc_t sweepfunc;
} jl_fielddescdyn_t;

JL_DLLEXPORT void *jl_gc_alloc_typed(jl_ptls_t ptls, size_t sz, void *ty);
JL_DLLEXPORT int jl_gc_mark_queue_obj(jl_ptls_t ptls, jl_value_t *obj);
JL_DLLEXPORT void jl_gc_mark_queue_objarray(jl_ptls_t ptls, jl_value_t *parent,
    jl_value_t **objs, size_t nobjs);

JL_DLLEXPORT void jl_gc_enable_foreign_sweepfunc(jl_ptls_t ptls, jl_value_t * bj);

#endif // _JULIA_GCEXT_H
