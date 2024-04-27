import trick
from threading import Timer

def load_checkpoint(path, sim_time, tries=10): 
    mode = trick.exec_get_mode()
    cur_time = trick.exec_get_sim_time()

    if(mode == 1 and cur_time == sim_time):
        trick.load_checkpoint(path)
        trick.exec_run()
    elif cur_time > sim_time or tries <= 0:
        print("Issue with loading checkpoint from input file!")
        return
    else:
        Timer(0.1, load_checkpoint, (path, sim_time, tries - 1,)).start()

def load_checkpoint_at(path, time):
    Timer(1.0, load_checkpoint, (path, time)).start()
    
    trick.freeze(time)

def main():
    
    exec(open("Modified_data/foo.dr").read())

    # trick.checkpoint(7.0)
    load_checkpoint_at("RUN_test6/chkpnt_7.000000", 5.0) # contains data recording, starts at t=7

    trick.stop(10.0)

if __name__ == "__main__":
    main()