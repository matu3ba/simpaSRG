#!/usr/local_rwth/bin/zsh
#SBATCH --account=thes0766            # thesis number
#SBATCH --mail-type=BEGIN,END,FAIL         # Mail events (NONE, BEGIN, END, FAIL, ALL)
#SBATCH --mail-user=jan.hafer@rwth-aachen.de    # Where to send mail	
#SBATCH --nodes=1                    # Run all processes on a single node	
#SBATCH --ntasks=1                   # Run a single task		
#SBATCH --mem=16gb                    # Job memory request
#SBATCH --partition=c18m             # partition
module switch intel gcc/10 &> /dev/null
PWD=$(pwd)
cd "$BUILDDIR"
pwd
"./createExamples" "$SIZE" "1" "1" "5"      #Helix creation
echo "./createExamples" "$SIZE" "1" "1" "5"      #Helix creation
cd "$PWD"
pwd
