import re, os
import pdb

class send_hs(object):
    """
    Reads a file containing the send_hs output and returns a send_hs
    object containing the values from that output
    """
    def __init__(self, hs_file):
        self.hs_file = hs_file
        self.actual_init_time = None
        self.actual_elapsed_time = None
        self.start_time = None
        self.stop_time = None
        self.elapsed_time = None
        self.actual_cpu_time_used = None
        self.sim_cpu_time = None
        self.init_cpu_time = None
        self.parse()
    def parse(self):
        f = open(self.hs_file, 'r')
        lines = f.readlines()
        for line in lines:
            self.actual_init_time = self.attempt_hs_match('ACTUAL INIT TIME',self.actual_init_time, line)
            self.actual_elapsed_time = self.attempt_hs_match('ACTUAL ELAPSED TIME',self.actual_elapsed_time, line)
            self.start_time = self.attempt_hs_match('SIMULATION START TIME',self.start_time, line)
            self.stop_time = self.attempt_hs_match('SIMULATION STOP TIME',self.stop_time, line)
            self.elapsed_time = self.attempt_hs_match('SIMULATION ELAPSED TIME',self.elapsed_time, line)
            self.actual_cpu_time_used = self.attempt_hs_match('ACTUAL CPU TIME USED',self.actual_cpu_time_used, line)
            self.sim_cpu_time = self.attempt_hs_match('SIMULATION / CPU TIME',self.sim_cpu_time, line)
            self.init_cpu_time = self.attempt_hs_match('INITIALIZATION CPU TIME',self.init_cpu_time, line)
         # TODO add capture of blade and DIAGNOSTIC: Reached termination time as success criteria

    def attempt_hs_match(self, name, var, text):
        """
        name: pattern to match (e.g. SIMULATION START TIME)
        var:  variable to assign value if match found
        text: text to search for pattern
        returns: var if not found, found value if found
        """
        m = re.match(name+': +([-]?[0-9]*\.?[0-9]+)', text.strip())
        if m:
            return(float(m.group(1)))
        return(var)

    def get(self,name):
       """
       Get a value by the name that appears in the send_hs message
       """
       if 'ACTUAL INIT TIME' in name:
           return self.actual_init_time
       if 'ACTUAL ELAPSED TIME' in name:
           return self.actual_elapsed_time
       if 'SIMULATION START TIME' in name:
           return self.start_time
       if 'SIMULATION STOP TIME' in name:
           return self.stop_time
       if 'SIMULATION ELAPSED TIME' in name:
           return self.elapsed_time
       if 'ACTUAL CPU TIME USED' in name:
           return self.actual_cpu_time_used
       if 'SIMULATION / CPU TIME' in name:
           return self.sim_cpu_time
       if 'INITIALIZATION CPU TIME' in name:
           return self.init_cpu_time
       else:
           return None
