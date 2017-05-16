# Conservative stack scanning

This branch implements experimental support for conservative stack scanning. It is a general framework that allows for both exclusive conservative stack scanning, but also precise stack scanning for Julia stack frames and conservative stack scanning for stack frames in foreign languages.

Other objects are still scanned precisely; conservative scanning is used solely for stacks (and can be limited to part of a stack).

The current implementation still has the code generator emit code for the existing GC frames.

# Locating objects

Conservative stack scanning means that any machine word on a stack has to be considered to be a potential GC root. The sole criterion for a word to be interpreted as a GC root is whether it points to an actual object. Obviously, this can lead to non-references (such as integers) being accidentally treated as references, thus keeping dead objects alive.

It is possible for addresses on the stack to point to the interior or just past the end of an object (for example, because the compiler performs strength reduction on an integer loop variable, incrementing the underlying pointer instead). We therefore need a solution to (1) determine whether such an address lies within the permitted range for addresses for an existing object and (2) find the beginning of that object, if it exists.

We employ different strategies for "big" objects (i.e. those derived from `bigval_t`) and for pool-allocated objects.

Big objects are kept in balanced trees instead of linked lists. While this adds O(log n) overhead to allocation (as well as some other operations), allocation of big objects is already a comparatively expensive operation; if there is significant overhead, it does not show up in synthethic benchmarks so far.

We are using treaps for the time being; similar to red-black trees, but unlike AVL trees, the expected number of rotations is constant, rather than logarithmic, making them more cache-friendly. And unlike red-black trees, they are straightforward to implement. We may want to replace them with a different implementation down the road, such as red-black trees or in-memory B-trees (the code has a simple interface that allows for simply plugging in a different balanced tree implementation).

Looking up pool-allocated ("small") objects requires more effort. We are hooking into the existing data structures provided by the allocator. To determine whether a machine word points to a small object, we perform the following checks:

1. Does the word point to an existing pool page with an address between the minimum and maximum valid offsets?
2. If so, does the type header point to an actual type?

If both questions can be answered in the affirmative, we assume that we have an actual pointer to an object and mark it.

We note that it appears to be possible for invalid type words to appear in objects. This seems to happen exclusively on fresh pages (i.e. those that haven't had `sweep_page()` called on them.

# Thread support

With regard to threading, the current safepoint model does not support either conservative or precise scanning of normal stack frames on most architectures, as it is not possible to determine the bottom of the stack. The exception is Darwin, which allows threads to query the registers (including the current stack pointer) of other threads.

Our current (expensive) workaround is to do bulk-scanning of empty pages from the bottom up; a better solution that is compatible with the current safepoint model would be to use `mprotect()` on stack pages and unprotect them as the stack grows for a more precise estimate of the stack bottom.

In the long term, it will be necessary to refine the safepoint implementation. For example, the safepoint mechanism could store the current stack pointer in thread-local data, separated by a signal fence, before (potentially) triggering the segfault. This would allow the signal handler to recover the stack pointer and do conservative stack scanning.

Information related to the state of a thread stack is currently kept in a new `thread_stack_info` field in thread-local storage, containing the beginning of the stack, the end of the stack, and (optionally) information about values in CPU registers.