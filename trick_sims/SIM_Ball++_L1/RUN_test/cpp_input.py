
def main():

    trick.stop(300.0)

    trick.jit_compile_and_run("RUN_cpp_input/input.cpp")
    trick.jit_compile("RUN_cpp_input/input.cpp")
    trick.jit_run("RUN_cpp_input/input.cpp", "run_event_1")
    trick.jit_compile_and_run("RUN_cpp_input/input.cpp", "run_event_1")

    # test a pre-compiled library.
    #trick.jit_add_library("RUN_cpp_input/jitlib/libinput.so")
    #trick.jit_run("RUN_cpp_input/jitlib/libinput.so", "run_event_1")

if __name__ == "__main__":
    main()


