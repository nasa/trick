
# Creates a local class. The class is destructed/deleted when the function returns.
def create_local_alloc_test():
    test = trick.AllocTest()

# Creates a class that is controlled by the Memory Manager (MM).  It is not freed when the function returns.
# TMM_declare_var returns a void *.  We can cast it to the correct type so we can access the class fields
# in python.  This allocation will not have a name in the memory manager.
def create_tmm_alloc_test():
    return trick.castAsAllocTest(trick.TMM_declare_var_s("AllocTest"))

# Creates a class that is controlled by the Memory Manager (MM).  It is not freed when the function returns.
# This allocation will have a name in the memory manager.
def create_new_alloc_test():
    # test is known to the memory manager, it must be deleted with TMM_delete_var
    return trick.AllocTest(TMMName="my_alloc_test")

# Creates a local class. The class is destructed/deleted when the function returns.
# Underneath this class declares itself to the memory manager and is controlled by the MM.  The class destructor
# automatically removes the allocation from the memory manager.
def create_local_alloc_test_with_mm_interface():
    test = trick.AllocTestWithMMInterface()

# Creates a class that is controlled by the Memory Manager (MM).  It is not freed when the function returns.
# TMM_declare_var returns a void *.  We can cast it to the correct type so we can access the class fields
# in python.  This allocation will not have a name in the memory manager.
def create_tmm_alloc_test_with_mm_interface():
    return trick.castAsAllocTestWithMMInterface(trick.TMM_declare_var_s("AllocTestWithMMInterface"))

# Creates a class that is controlled by the Memory Manager (MM).  It is not freed when the function returns.
# This allocation will have a name in the memory manager.
def create_new_alloc_test_with_mm_interface():
    return trick.AllocTestWithMMInterface(TMMName="my_alloc_test_with_mm_interface")

def main():

    print "\ncreate local python tracked allocation"
    create_local_alloc_test()

    print "\ncreate through TMM_declare_var"
    test = create_tmm_alloc_test()
    trick.TMM_delete_var_a(test)

    print "\ncreate with TMMName"
    test = create_new_alloc_test()
    trick.TMM_delete_var_a(test)

    print "\ncreate local python tracked allocation.  Class uses TRICK_MM_INTERFACE."
    create_local_alloc_test_with_mm_interface()

    print "\ncreate through TMM_declare_var"
    test = create_tmm_alloc_test_with_mm_interface()
    trick.TMM_delete_var_a(test)

    print "\ncreate through TMMName with TRICK_MM_INTERFACE"
    test = create_new_alloc_test_with_mm_interface()
    trick.TMM_delete_var_n("my_alloc_test_with_mm_interface")

    trick.stop(1.0)

if __name__ == "__main__":
    main()

