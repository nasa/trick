
def main():
    trick.real_time_enable()
    trick.itimer_enable()
    trick.exec_set_software_frame(0.5)
    trick.exec_set_thread_process_type(1, trick.PROCESS_TYPE_ASYNC_CHILD)
    
    trick.var_allow_all_connections()

if __name__ == "__main__":
    main()


