import trick
from trick.unit_test import *
from threading import Timer

# This was just here for convenience to dump the checkpoints.

def dump_checkpoint(sim_time): 
    mode = trick.exec_get_mode()
    cur_time = trick.exec_get_sim_time()

    if(mode == 1 and cur_time == sim_time):
        trick.checkpoint()
        trick.exec_run()
    elif cur_time > sim_time:
        return
    else:        
        Timer(0.1, dump_checkpoint, (sim_time,)).start()

def checkpoint(time):
    Timer(1.0, dump_checkpoint, (time,)).start()

    trick.freeze(time)

def main():    
    checkpoint(5.0)

    trick.stop(10.0)

if __name__ == "__main__":
    main()