import trick
from trick.unit_test import *

# This was just here for convenience to dump the checkpoints.

def main():
    exec(open("Modified_data/foo2.dr").read())    

    dr_group = drg[DR_GROUP_ID]

    dr_group.set_cycle(0.1)
    dr_group.add_cycle(100.0)
    
    trick.checkpoint(7.01)

    trick.exec_set_job_cycle("testSimObject.my_foo.increment", 0, 0.1)

    ss_end = 1800.0-1789.0
    cs_end = 30600.0-20000-1789.0
    cs_logging_break = 28796.6-20000-1789.0
    ph_logging_break = 1796.6-1789.0

    ss_end = 1800.0
    cs_end = 36000.0
    cs_logging_break = 28796.6
    second_cycle_update = ((cs_end-1.1)+(ss_end+2.4))/2
    ph_logging_break = 1796.6

    print("time 0, cycle = 0.1")
    print(f'time {2.4}, cycle = {ph_logging_break}')
    print(f'time {ss_end-1.1}, cycle = {0.1}')
    print(f'time {ss_end+2.4}, cycle = {cs_logging_break}')
    print(f'time {second_cycle_update}, 2nd cycle = {cs_logging_break/4}')
    print(f'time {cs_end-1.1}, cycle = {0.1}')
    print(f'stop = {cs_end+2.6}')

    trick.add_read(2.4,f'trick.get_data_record_group("{dr_group.get_group_name()}").set_cycle({ph_logging_break})')
    trick.add_read(ss_end-1.1,f'trick.get_data_record_group("{dr_group.get_group_name()}").set_cycle(0.1)')
    trick.add_read(ss_end+2.4,f'trick.get_data_record_group("{dr_group.get_group_name()}").set_cycle({cs_logging_break})')
    trick.add_read(second_cycle_update,f'trick.get_data_record_group("{dr_group.get_group_name()}").set_rate(1, {cs_logging_break/4})')
    trick.add_read(cs_end-1.1,f'trick.get_data_record_group("{dr_group.get_group_name()}").set_cycle(0.1)')
    trick.stop(cs_end+2.6)

if __name__ == "__main__":
    main()
