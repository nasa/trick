| [Home](/trick) → [Documentation Home](../Documentation-Home) → [Simulation Capabilities](Simulation-Capabilities) → Using STLs in Trick Sims |
|------------------------------------------------------------------|

# Standard Template Libraries (STL) in Trick

STLs may be used in models. However, STL variables (currently) are not data recordable, visible in the variable server, nor directly accessible in the input file. Some STLs can be checkpointed: array, vector, list, deque, set, multiset, map, multimap, stack, queue, priority_queue, pair.

STL classes cannot be directly registered with the memory manager, but they can be processed by the checkpoint agent when nested inside normal C++ classes (including sim objects). 

STL checkpoint restore may slow down the default data jobs of some sims. STL restore is on by default. To turn off STL restore:

If using memory manager through the C interface:
```
int TMM_set_stl_restore (int on_off);
```

If using the memory manager through the C++ interface, set the default or pass a parameter to your read_checkpoint function of choice:
```
int set_restore_stls_default (bool on);
int read_checkpoint( std::istream* in_s, bool do_restore_stls = restore_stls_default);
int read_checkpoint_from_string( const char* s, bool do_restore_stls = restore_stls_default );
int init_from_checkpoint( std::istream* in_s, bool do_restore_stls = restore_stls_default);
```

If using the checkpoint restart C interface:
```
int load_checkpoint_stls( const char * file_name, int with_stls ) ;
```



## What works:

To checkpoint an STL, it **must** be a member of a class or struct. 
```
class AnyClass {
    std::vector<int> vec;
}
```
Declare an instance of this class with the memory manager in the usual way:
```
AnyClass * my_class = tmm->declare_var("AnyClass my_alloc");
```

If it is in a class that is nested in a `sim_object`, it will be registered with the memory manager automatically.

You can nest an arbitrary amount of STLs, they will all be checkpointed as long as 
the base is a member of a class that is registered with the memory manager. There
are a some known [**limitations**](#limitations).

```
class AnyClass {
    std::pair<std::vector<std::map.......>>
}
```

<a id=limitations></a>

## Limitations

The STL checkpointing feature can only handle simple types, pointers, and nested STL types. 

### An STL type within a user defined type within an STL will fail to checkpoint.

For example: a user defined class with an STL in it:
```
class VectorWrapper {
    int a;
    std::vector<int> vec;
}
```

An outer class (which is registered with the memory manager, like a member of a sim_object or something that has been explicitly declared) that has an STL container of these objects:
```
class MyObject {
    std::vector<VectorWrapper> vec_user_defined;
}
```

If MyObject is populated, it will be able to checkpoint and restore without throwing an error, and all the `VectorWrapper` objects will be present, but `vec` will not be restored (`a` will  restore successfully). The contents of `vec` are never written out to the checkpoint file.


If `MyObject` instead has a vector of pointers to `VectorWrapper`, and each `VectorWrapper` is registered with the memory manager, `vec` will checkpoint and restore successfully.
```
class MyObject {
    std::vector<VectorWrapper *> vec_user_defined_ptr;
}
```

### You cannot directly create or register an external stl with the memory manager, you will get a parser error.

STLs can't register it with the memory manager, so there's no way for the checkpoint to know where the memory is.

```
class AnyClass {
    std::vector<int> *anything;
}

class AnyClass {
    std::pair<int,int>[5];
}


memorymanager->declare_var("std::vector<int> my_vector_allocation");

std::vector<int> my_vector;
memorymanager->declare_var_extern(&my_vector, "std::vector<int> my_vector_allocation");

```



[Continue to Threads](Threads)
