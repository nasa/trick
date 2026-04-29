
def main():
    trick.real_time_enable()
    trick.itimer_enable()
    trick.exec_set_software_frame(0.5)
    trick.exec_set_thread_process_type(1, trick.PROCESS_TYPE_ASYNC_CHILD)
    
    trick.var_allow_connections()
    trick.var_resolve_hostname()

if __name__ == "__main__":
    main()


