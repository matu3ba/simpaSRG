#!/usr/local_rwth/bin/zsh
#SBATCH --account=thes0766            # thesis number
#SBATCH --mail-type=BEGIN,END,FAIL         # Mail events (NONE, BEGIN, END, FAIL, ALL)
#SBATCH --mail-user=jan.hafer@rwth-aachen.de    # Where to send mail	
#SBATCH --nodes=1                    # Run all processes on a single node	
#SBATCH --ntasks=1                   # Run a single task		
#SBATCH --ntasks-per-core=1           # NO HYPERTHREADING
#SBATCH --mem=16gb                    # Job memory request
#SBATCH --partition=c18m             # partition
#SBATCH --time=01:00:00              # Time limit hrs:min:sec
module switch intel gcc/10 2> /dev/null
PWD=$(pwd)
cd "$BUILDDIR"
pwd
"./""$PROGN_SIMPLE" "$MHD_IN" "1" "$CNTSEEDING" "$OUTPUTWRITE"  \
  "$CNTRUNS" "$MHD_OUT_SIMPLE"
echo "./""$PROGN_SIMPLE" "$MHD_IN" "1" "$CNTSEEDING" "$OUTPUTWRITE"  \
  "$CNTRUNS" "$MHD_OUT_SIMPLE"
cd "$PWD"
pwd
