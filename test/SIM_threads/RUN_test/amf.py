
def main():
    trick.real_time_enable()
    trick.itimer_enable()
    trick.exec_set_thread_process_type(trick_cthreadmap.getThreadId("C1_THREAD"), trick.PROCESS_TYPE_AMF_CHILD)
    trick.exec_set_thread_amf_cycle_time(trick_cthreadmap.getThreadId("C1_THREAD"), 10.0)

if __name__ == "__main__":
    main()

