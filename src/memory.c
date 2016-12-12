#include <stdlib.h>
#include <shalw.h>
#include <stdio.h>

void mpi_alloc(void) {

//X=COLONNES ; Y=LIGNES

  int size_band=(2*size_x*size_y)/n + 4*size_y;

//Dans une execution, on stocke les 2 bandes corespondant au pas de temps precedent et a celui a calculer + 2 lignes suplementaires pour chacun (besoin des valeurs aux extremites des blocs).


  if (rang == MASTER){
    //Allocating global table
    hFilg = (double *) calloc(2*size_x*size_y, sizeof(double));
    /*uFilg = (double *) calloc(2*size_x*size_y, sizeof(double));
    vFilg = (double *) calloc(2*size_x*size_y, sizeof(double));
    hPhyg = (double *) calloc(2*size_x*size_y, sizeof(double));
    uPhyg = (double *) calloc(2*size_x*size_y, sizeof(double));
    vPhyg = (double *) calloc(2*size_x*size_y, sizeof(double));*/
  }

  //printf("Processus #%d:\t\tsize_band=%d\n", rang, size_band);
  
	//Allocating local tables
  hFil = (double *) calloc(size_band, sizeof(double));
  uFil = (double *) calloc(size_band, sizeof(double));
  vFil = (double *) calloc(size_band, sizeof(double));
  hPhy = (double *) calloc(size_band, sizeof(double));
  uPhy = (double *) calloc(size_band, sizeof(double));
  vPhy = (double *) calloc(size_band, sizeof(double));

}

void dealloc(void) {
  free(hFil);
  free(uFil);
  free(vFil);
  free(hPhy);
  free(uPhy);
  free(vPhy);

  if (rang == MASTER){
    free(hFilg);
    /*free(uFilg);
    free(vFilg);
    free(hPhyg);
    free(uPhyg);
    free(vPhyg);*/
  }
  
}
