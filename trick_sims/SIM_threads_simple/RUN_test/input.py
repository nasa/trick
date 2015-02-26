# $Id: input.py 1703 2011-06-17 20:52:09Z wwoodard $

import os

def main():
    trick.real_time_enable()
    trick.itimer_enable()
    trick.exec_set_software_frame(0.10)

    trick.exec_set_thread_process_type(1, trick.PROCESS_TYPE_AMF_CHILD)
    trick.exec_set_thread_amf_cycle_time(1, 0.5)

    trick.exec_set_thread_process_type(2, trick.PROCESS_TYPE_AMF_CHILD)
    trick.exec_set_thread_amf_cycle_time(2, 0.1)

if __name__ == "__main__":
    main()

