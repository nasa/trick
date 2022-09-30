| [Home](/trick) → [Documentation Home](../Documentation-Home) → [Simulation Capabilities](Simulation-Capabilities) → Using STLs in Trick Sims |
|------------------------------------------------------------------|

# Standard Template Libraries (STL) in Trick

STLs may be used in models. However, STL variables (currently) are not data recordable, visible in the variable server, nor directly accessible in the input file. Some STLs can be checkpointed: array, vector, list, deque, set, multiset map, multimap, stack, queue, priority_queue, pair.

STL classes cannot be directly registered with the memory manager, but they can be processed by the checkpoint agent when nested inside normal C++ classes (including sim objects). 

STL checkpoint restore may slow down the default data jobs of some sims. To disable STL checkpoint restore

### What works:

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

You can nest an arbitrary amount of STLs, they will all be checkpointed as long as 
the base is a member of a class that is registered with the memory manager. There
are a few exceptions due to [**bugs**](#known_bugs) at the moment (but hoppefully not forever!)

```
class AnyClass {
    std::pair<std::vector<std::map.......>>
}
```


### What doesn't work:

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

You cannot directly create or register an external stl with the memory manager, you will get a parser error.
Can't register it with the memory manager, so there's no way for the checkpoint to know where the memory is.
Can't use arrays of STLs.

<a id=known-bugs></a>

### Known bugs

The types in an std::pair cannot be sequence STL types (vector, queue, stack, etc).
```
    // This will fail to checkpoint
    std::pair<int, std::vector<int>> 
```

The types inside an std::set cannot be any STL types (excluding strings).
```
    // This will fail to compile
    std::set<std::vector<int>> 
```

These bugs are being worked on.

[Continue to Threads](Threads)
