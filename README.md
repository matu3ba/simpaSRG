# simpaSRG: simple parallelized Seeded Region Growing

## Goal: 1.Robust, 2.in-place, 3. parallelized Seeded Region Growing
- robust: good worst case performance in relation to other cases, even on malformed examples
- in-place: maximum allocation of additional memory in the size of the objects surface
- parallelized: time-sliding and work-sliding on multiple processor cores

## Main Findings
1. First upper bound memory estimation for SRG and for cache usage estimation: Octahedron in Voxel representation + on top cache lines usage estimation.
2. The fundamental bottleneck of Seeded Region Growing(SRG) is memory access delay: more cache is better.
3. SRG has a computable upper memory bound, which in practice does not help much, because L3 cache usage of an operating system is not inspectable (only testable).
4. Parallelization of SRG works well, also for forked data structures.
5. Distinguishing data into tubular, forked and dense block-like structures helps to estimate performance.

## Potential improvements
1. Cache contention of small tube-like structures could use a workaround, but the hardware is surprisingly good on prioritizing one thread to resolve that for linear access patterns.
2. Completely resolving cache contention would need specialised hardware, where tradeoffs must be investigated before making final conclusions.
3. POSIX does not specify how to keep track of detached threads, which might break buffered output pipes (threads are finished, but not cleaned up in the Kernel). A operating system specific workaround could be implemented.
4. Benchmark should be repeated without hyper-threading. This was not done to keep the code simple.
5. The data representation could be optimised to only use the offset and compute the 3D coordinates on demand.

## Requirements

20 GB storage and 8GB RAM are needed to conduct the benchmarks.
Benchmarks vary alot depending on cache-coherence and synchronization of CPUs,
so disable energy saving modes for optimal results.

Speed depends on 1.available cache, 2. geometry classes of input data, 3.physical cores + synchronization mechanism etc.

## Usage
1. Run ./prepare to extract data examples, build binaries and create benchmark code.
2. Run ./benchmark NUMTHREADS CNTRUNS, for where 8 threads with 30 runs take roughly 3h, since
the single thread performance from the simple algorithm is used as comparison.

## Algorithm principle
1. generate paper from paper.tex
2. or check folder tex/algorithm_visualisation/
3. or check src/SIPMA*

### Abandoned ideas (all too expensive)
- 1.Save direction and amount of steps to every voxel
- 2.Use this information to do raycast with free threads
- 3.Use or disable thread depending on distance(+time) points from other seeding points

### Acknowledgements
Many thanks to Dr. Felix Gremse for supervising the work and providing examples.
Thanks to Gremse-IT for providing Imalytics Preclinical for the 3D visualisations of the benchmark examples.

Apologies for the unstructured and numerous build scripts. Keeping everything in a folder with a Makefile would have been easier.
