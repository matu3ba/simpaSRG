#!/usr/local_rwth/bin/zsh
#SBATCH --account=thes0766            # thesis number
#SBATCH --job-name=preparation      # Job name
#SBATCH --mail-type=END,FAIL         # Mail events (NONE, BEGIN, END, FAIL, ALL)
#SBATCH --mail-user=jan.hafer@rwth-aachen.de    # Where to send mail	
#SBATCH --nodes=1                    # Run all processes on a single node	
#SBATCH --ntasks=1                   # Run a single task		
#SBATCH --cpus-per-task=4            # Number of CPU cores per task
#SBATCH --mem=16gb                    # Job memory request
#SBATCH --partition=c18m             # partition
#SBATCH --time=01:00:00              # Time limit hrs:min:sec
#SBATCH --output=prepare_%j.log     # Standard output and error log
module switch intel gcc/10 &> /dev/null
mkdir -p data
if [ ! -f './data/BoneMaskHigh.img' ]; then
  echo "examples.."
  7z e "compressed/realisticExamples.7z" -odata/
else
  echo "file from compressed were probably extracted, else use 7z e FROM -oTO"
fi
mkdir -p build
cd tools
echo "compiling programs for algorithms.."
./buildpar.sh
echo "compiling tool for exampleCreation.."
cd exampleCreation/; ./build.sh; cd ..
echo "compiling tool for counting changed Voxels.."
cd countChangedVoxels; ./build.sh; cd "../.."
cd "test/numofcores"; ./build.sh; cd "../../build"; ./numofcores > "numofcores.txt"
echo "This are not necessary physical cores!" >> "numofcores.txt"
echo "You may proceed with running ./benchmark which also creates test data"
