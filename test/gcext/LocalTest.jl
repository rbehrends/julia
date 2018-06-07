# This file is a part of Julia. License is MIT: https://julialang.org/license

module LocalTest
  const Stack = Main.TestGCExt.Stack
  function make()
    ccall(:stk_make, Stack, ())
  end
  function push(stack :: Stack, val :: String)
    ccall(:stk_push, Nothing, (Stack, String), stack, val)
  end
  function top(stack :: Stack)
    return ccall(:stk_top, String, (Stack,), stack)
  end
  function pop(stack :: Stack)
    return ccall(:stk_pop, String, (Stack,), stack)
  end
  function size(stack :: Stack)
    return ccall(:stk_size, UInt, (Stack,), stack)
  end
  function empty(stack :: Stack)
    return size(stack) == 0
  end
  function gc_counter_full()
    return ccall(:get_gc_counter, UInt, (Cint,), 1)
  end
  function gc_counter_inc()
    return ccall(:get_gc_counter, UInt, (Cint,), 0)
  end
  function gc_counter()
    return gc_counter_full() + gc_counter_inc()
  end
  function get_aux_root(n :: Int)
    return ccall(:get_aux_root, String, (UInt,), n)
  end
  function set_aux_root(n :: Int, x :: String)
    return ccall(:set_aux_root, Nothing, (UInt, String), n, x)
  end

  set_aux_root(0, "Root scanner works.");
  function test()
    local stack = make()
    for i in 1:100000
      push(stack, string(i))
    end
    for i in 1:1000
      local stack2 = make()
      while !empty(stack)
	push(stack2, pop(stack))
      end
      stack = stack2
      if i % 100 == 0
	GC.gc()
      end
    end
  end
  @time test()
  print(gc_counter_full(), " full collections.\n")
  print(gc_counter_inc(), " partial collections.\n")
  print(get_aux_root(0), "\n")
end
