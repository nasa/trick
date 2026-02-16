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

    ss_end = 1800.0
    cs_end = 3600.0
    cs_logging_break = 2879.66
    second_cycle_update = ((cs_end-1.1)+(ss_end+2.4))/2
    ph_logging_break = 1796.6
    stop_time = cs_end+2.6+10.0

    print("time 0, cycle = 0.1")
    print(f'time {2.4}, cycle = {ph_logging_break}')
    print(f'time {ss_end-1.1}, cycle = {0.1}')
    print(f'time {ss_end+2.4}, cycle = {cs_logging_break}')
    print(f'time {second_cycle_update}, 2nd cycle = {cs_logging_break/4}')
    print(f'time {cs_end-1.1}, cycle = {0.1}')
    print(f'time {stop_time-6.9}, disable job')
    print(f'time {stop_time-4.5}, enable job')
    print(f'stop = {stop_time}')

    trick.add_read(2.4,f'trick.exec_set_job_cycle("trick_data_record_group_{dr_group.get_group_name()}.data_record", 1, {ph_logging_break})')
    trick.add_read(ss_end-1.1,f'trick.get_data_record_group("{dr_group.get_group_name()}").set_cycle(0.1)')
    trick.add_read(ss_end+2.4,f'trick.get_data_record_group("{dr_group.get_group_name()}").set_cycle({cs_logging_break})')
    trick.add_read(second_cycle_update,f'trick.get_data_record_group("{dr_group.get_group_name()}").set_rate(1, {cs_logging_break/4})')
    trick.add_read(cs_end-1.1,f'trick.get_data_record_group("{dr_group.get_group_name()}").set_cycle(0.1)')
    trick.add_read(1.0, f'trick.get_data_record_group("{dr_group.get_group_name()}").set_cycle(0.0)')
    trick.add_read(2.0, f'trick.get_data_record_group("{dr_group.get_group_name()}").set_rate(1, {1/128})')
    trick.add_read(2.0, f'trick.get_data_record_group("{dr_group.get_group_name()}").set_rate(1, {1/128})')
    trick.add_read(stop_time-6.9, f'trick.exec_set_job_onoff("trick_data_record_group_{dr_group.get_group_name()}.data_record", 1, 0)')
    trick.add_read(stop_time-4.5, f'trick.exec_set_job_onoff("trick_data_record_group_{dr_group.get_group_name()}.data_record", 1, 1)')
    trick.stop(stop_time)

if __name__ == "__main__":
    main()
