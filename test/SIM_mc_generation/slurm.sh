#SBATCH --partition rapid
#SBATCH --time 5
#SBATCH --array=0

#This is an example sbatch script showing the capability of running an array 
#job in SLURM. SLURM is an HPC (High-Performace-Computing) scheaduling tool
#that can manage thousands or millions of jobs. The Monte Carlo Generation 
#Model is best well suited with an HPC scheadulign tool.
#  
#Slurm: https://slurm.schedmd.com/documentation.html

echo this job number $SLURM_ARRAY_TASK_ID
RUN_NUM=`printf %01d $SLURM_ARRAY_TASK_ID`
echo $RUN_NUM
./S_main_Linux_4.8_x86_64.exe MONTE_RUN_ERROR_invalid_name/RUN_${RUN_NUM}/monte_input.py
