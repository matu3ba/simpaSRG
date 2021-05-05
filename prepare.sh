#!/usr/bin/env bash
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
