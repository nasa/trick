#!/bin/bash
#SBATCH --partition rapid
#SBATCH --time 5
#SBATCH --array=0

echo this job number $SLURM_ARRAY_TASK_ID
RUN_NUM=`printf %01d $SLURM_ARRAY_TASK_ID`
echo $RUN_NUM
./S_main_Linux_4.8_x86_64.exe MONTE_RUN_ERROR_invalid_name/RUN_${RUN_NUM}/monte_input.py
