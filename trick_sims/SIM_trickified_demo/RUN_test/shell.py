
# This input file opens interactive python shells, one during initialization, and
# the second 5 seconds into the sim.  Once inside the shell perform these 2 imports
# to get access to all of the simulation variables
#

import trick
from code import InteractiveConsole


class FileCacher:
    "Cache the stdout text so we can analyze it before returning it"
    def __init__(self): self.reset()
    def reset(self): self.out = []
    def write(self,line): self.out.append(line)
    def flush(self):
        output = '\n'.join(self.out)
        self.reset()
        return output

class Shell(InteractiveConsole):
    "Wrapper around Python that can filter input/output to the shell"
    def __init__(self):
        self.stdout = sys.stdout
        self.cache = FileCacher()
        InteractiveConsole.__init__(self)
        return

    def get_output(self): sys.stdout = self.cache
    def return_output(self): sys.stdout = self.stdout

    def push(self,line):
        self.get_output()
        # you can filter input here by doing something like
        # line = filter(line)
        InteractiveConsole.push(self,line)
        self.return_output()
        output = self.cache.flush()
        # you can filter the output here by doing something like
        # output = filter(output)
        print output # or do something else with it
        return

sh = Shell()

def main():

    my_integ_loop.getIntegrator(trick.Runge_Kutta_2, 4)

    trick_message.mpublisher.sim_name = "ball_sim"

    read = 0.0
    trick.add_read(read,"""trick.checkpoint("chkpnt_pre_init")""")

    trick.exec_set_freeze_frame(0.10)
    trick.stop(300.0)

def event():
    global sh
    trick.add_read(float(trick.exec_get_sim_time() + 5.0),"""sh.interact()""")

if __name__ == "__main__":
    main()
    event() ;
    sh.interact()


