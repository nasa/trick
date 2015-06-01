#!/usr/bin/env python

from random import randint

import subprocess
import commands
import sys
import os

try:
    f = open( commands.getstatusoutput( 'echo $TRICK_HOME' )[1] + "/config_Linux.mk", "r" )

except:
    print "\nUnable to open" + commands.getstatusoutput( 'echo $TRICK_HOME' )[1] + "/config_Linux.mk\n"
    sys.exit(0)



start_sim = ''

for line in f:

    if line.startswith('#'):
        continue

# --join: if a coordinator is running, join it (for checkpointing more than one sim on same machine)
    if "DMTCP  =" in line:
        start_sim =  line.split( "=", 1 )[1].strip() + "/bin/dmtcp_checkpoint " + \
	             "--port " +  str( randint(2000,10000) ) + \
	             " --quiet " + \
	             "--join " + \
		     "--checkpoint-open-files " + \
		     "--ckptdir " + commands.getstatusoutput( 'echo $PWD' )[1] + "/dmtcp_checkpoints " + \
		     "./S_main_" + commands.getstatusoutput( '$TRICK_HOME/bin/gte TRICK_HOST_CPU' )[1] + ".exe " + \
		     " ".join( sys.argv[1:] )  + \
		     " dmtcp"

f.close()

if ( not start_sim ):
    print "\nPlease run configure script in " + commands.getstatusoutput( 'echo $TRICK_HOME' )[1] + \
          " to set DMTCP location.\n"

try:
    subprocess.call( start_sim, shell=True )

except:
    print "\nExiting...\n"
