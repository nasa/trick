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
    chkpnt_path = "RUN_test1/chkpnt_5.000000"
    load_checkpoint_at(chkpnt_path, 5.0)

    trick.stop(10.0)

if __name__ == "__main__":
    main()