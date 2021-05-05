#include<cstdio>
#include<ctime>
#include<cstdint>
#include<chrono>
#include<iostream>

int main() {

  //clock() has only microsecond resolution

	
	//finish = clock();
  //test = clock();
	//printf ("Dauer: %d Ticks\n", finish); //still works, but wrong???
  //printf ("Groeße von clock_t: %d\n", sizeof(clock_t)); 
	printf ("Dauer: %lu Ticks\n", clock()); //%lu is correct type -> only ms exact
	printf ("Dauer: %lu Ticks\n", clock());

  //simplest and most consistent behavior
  //printf("Dauer: %llu blubb\n", std::chrono::high_resolution_clock::now());
  //printf("Dauer: %llu blubb\n", std::chrono::high_resolution_clock::now());

  //std::chrono::time_point<std::chrono::high_resolution_clock, 
  //std::chrono::nanoseconds> time_point;
  //time_point = std::chrono::time_point_cast<std::chrono::nanoseconds>
  //            (std::chrono::high_resolution_clock::now());

  //printf ("Groeße von clock_t: %ld\n", sizeof(time_point)); 
  //printf ("Chrono high resolution clock: %llu\n", time_point); 
  int64_t x{};


  auto start = std::chrono::steady_clock::now();
  printf ("chrono now: %lu\n", std::chrono::high_resolution_clock
              ::now().time_since_epoch().count());
  for (uint64_t i=0; i<5e9; ++i)
    x = i*173 + 25*i % 193;
  printf ("chrono now: %lu\n", std::chrono::high_resolution_clock
              ::now().time_since_epoch().count());
  printf("x, %ld", x);
  auto end = std::chrono::steady_clock::now();
  printf ("Elapsed time algo: %ld ms\n", std::chrono::duration_cast<std::chrono::nanoseconds>
    (end - start).count() );

	return 0;
}
