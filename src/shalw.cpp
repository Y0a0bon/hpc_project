#include <stdlib.h>
#include <shalw.h>
#include <parse_args.hpp>
#include <memory.h>
#include <init.h>
#include <forward.h>
#include <export.h>
#include <mpi.h>
#ifdef _OPENMP
#include <omp.h>
#endif
#include <sys/time.h>

double *hFil, *uFil, *vFil, *hPhy, *uPhy, *vPhy;
double *hFilg, *uFilg, *vFilg, *hPhyg, *uPhyg, *vPhyg;
int size_x, size_y, nb_steps;
double dx, dy, dt, pcor, grav, dissip, hmoy, alpha, height, epsilon;
bool file_export;
std::string export_path;
int rang, n, proc_per_line;

double get_Time(){
  struct timeval tmp_time;
  gettimeofday(&tmp_time, NULL);
  return tmp_time.tv_sec + (tmp_time.tv_usec * 1.0e-6L);	
}


int main(int argc, char **argv) {
  
  double start=0.0, finish=0.0; 

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &n);
  MPI_Comm_rank(MPI_COMM_WORLD, &rang);


  parse_args(argc, argv);
  /*printf("Processus #%d:\t\tCommand line options parsed\n", rang);*/

  if (size_x%n != 0 ){
    printf("le nombre de processeur doit diviser size_x=%d\n", size_x);
    exit(0);
  }

  proc_per_line=size_x/n;
  
  start = get_Time();

  mpi_alloc();
  /*printf("Processus #%d:\t\tMemory allocated\n", rang);*/

 //gauss_init est modifie
  gauss_init();
  /*printf("Processus #%d:\t\tState initialised\n", rang);*/

  //Pourquoi est ce qu on fait une barriere ???
  MPI_Barrier(MPI_COMM_WORLD);

#ifdef _OPENMP
#pragma omp single
  /*printf("********** OPEN MP **********\n");*/
  forward();
#else
  forward();
#endif
  
  /*printf("Processus #%d:\t\tState computed\n", rang);*/
  
  dealloc();
  /*printf("Processus #%d:\t\tMemory freed\n", rang);*/

  MPI_Finalize();
  finish = get_Time();

  /*fprintf( stdout, "Total computation time (with gettimeofday()) : %g      s\n", (double)(finish - start));*/
  if(rang == MASTER)
  fprintf( stdout, "%d\t%d\t%d\t%d\t%g\n", n, size_x, size_y, nb_steps, (double)(finish - start) );
  return EXIT_SUCCESS;
}
