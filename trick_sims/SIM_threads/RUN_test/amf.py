
def main():
    trick.real_time_enable()
    trick.itimer_enable()
    trick.exec_set_thread_process_type(1, trick.PROCESS_TYPE_AMF_CHILD)
    trick.exec_set_thread_amf_cycle_time(1, 10.0)

if __name__ == "__main__":
    main()

