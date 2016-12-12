#include <stdio.h>
#include <math.h>
#include <shalw.h>
#include <export.h>
#include <mpi.h>
#include <stdio.h>

double hFil_forward(int t, int i, int j) {
  //Phase d'initialisation du filtre
  //HPHY(t - 1, i, j) est encore nul
  if (t <= 2)
    return HPHY(t, i, j);
  return HPHY(t - 1, i, j) +
    alpha * (HFIL(t - 1, i, j) - 2 * HPHY(t - 1, i, j) + HPHY(t, i, j));
}

double uFil_forward(int t, int i, int j) {
  //Phase d'initialisation du filtre
  //UPHY(t - 1, i, j) est encore nul
  if (t <= 2)
    return UPHY(t, i, j);
  return UPHY(t - 1, i, j) +
    alpha * (UFIL(t - 1, i, j) - 2 * UPHY(t - 1, i, j) + UPHY(t, i, j));
}

double vFil_forward(int t, int i, int j) {
  //Phase d'initialisation du filtre
  //VPHY(t - 1, i, j) est encore nul
  if (t <= 2)
    return VPHY(t, i, j);
  return VPHY(t - 1, i, j) +
    alpha * (VFIL(t - 1, i, j) - 2 * VPHY(t - 1, i, j) + VPHY(t, i, j));
}

double hPhy_forward(int t, int i, int j) {
  double c, d;
  
  c = 0.;
  if (i > 0)
    c = UPHY(t - 1, i - 1, j);

  d = 0.;
  if (j < size_y - 1)
    d = VPHY(t - 1, i, j + 1);

  return HFIL(t - 1, i, j) -
    dt * hmoy * ((UPHY(t - 1, i, j) - c) / dx +
		 (d - VPHY(t - 1, i, j)) / dy);
}

double uPhy_forward(int t, int i, int j) {
  double b, e, f, g;
  
  if (i == size_x - 1)
    return 0.;

  b = 0.;
  if (i < size_x - 1)
    b = HPHY(t - 1, i + 1, j);

  e = 0.;
  if (j < size_y - 1)
    e = VPHY(t - 1, i, j + 1);

  f = 0.;
  if (i < size_x - 1)
    f = VPHY(t - 1, i + 1, j);

  g = 0.;
  if (i < size_x - 1 && j < size_y - 1)
    g = VPHY(t - 1, i + 1, j + 1);

  return UFIL(t - 1, i, j) +
    dt * ((-grav / dx) * (b - HPHY(t - 1, i, j)) +
	  (pcor / 4.) * (VPHY(t - 1, i, j) + e + f + g) -
	  (dissip * UFIL(t - 1, i, j)));
}

double vPhy_forward(int t, int i, int j) {
  double c, d, e, f;

  if (j == 0)
    return 0.;

  c = 0.;
  if (j > 0)
    c = HPHY(t - 1, i, j - 1);

  d = 0.;
  if (i > 0 && j > 0)
    d = UPHY(t - 1, i -1, j -1);

  e = 0.;
  if (i > 0)
    e = UPHY(t - 1, i - 1, j);

  f = 0.;
  if (j > 0)
    f = UPHY(t - 1, i, j - 1);

  return VFIL(t - 1, i, j) +
    dt * ((-grav / dy) * (HPHY(t - 1, i, j) - c) -
	  (pcor / 4.) * (d + e + f + UPHY(t - 1, i, j)) -
	  (dissip * VFIL(t - 1, i, j)));
}


/**
 * Fonction forward parallelisee
 */
void forward(void) {
  // MPI_Comm comm;
  
  MPI_Status status;
  
  FILE *file = NULL;
  double svdt = 0.;
  int i, j, t = 0;
    
  if (rang == MASTER && file_export) {
      file = create_file();
      export_step(file, t);
      /*printf("Fin export pas 0\n")*/;
    }

  
  for (t = 1; t < nb_steps; t++) {

   
    if (t == 1) {
      svdt = dt;
      dt = 0;

       
    }
    if (t == 2){
      dt = svdt / 2.;
    }
    
    //printf("Processus #%d:\t\tfilling the local tables\n", rang);
#ifdef _OPENMP
#pragma omp parallel for private(i,j) schedule(static)
    for (i = 1; i < (size_x/n + 1); i++) {
      for(j = 0; j < size_y; j++){
	HPHY(t,i, j) = hPhy_forward(t,i, j);
	UPHY(t,i, j) = uPhy_forward(t,i, j);
	VPHY(t,i, j) = vPhy_forward(t,i, j);
	HFIL(t,i, j) = hFil_forward(t,i, j);
	UFIL(t,i, j) = uFil_forward(t,i, j);
	VFIL(t,i, j) = vFil_forward(t,i, j);
      }
    }
#else
    for (i = 1; i < (size_x/n + 1); i++) {
      for (j = 0; j < size_y; j++) {
      
	HPHY(t,i, j) = hPhy_forward(t,i, j);
	UPHY(t,i, j) = uPhy_forward(t,i, j);
	VPHY(t,i, j) = vPhy_forward(t,i, j);
	HFIL(t,i, j) = hFil_forward(t,i, j);
	UFIL(t,i, j) = uFil_forward(t,i, j);
	VFIL(t,i, j) = vFil_forward(t,i, j);
      }
    }
#endif

    //On rassemble les bandes calculees pour le pas t dans le processus root
   
    MPI_Gather(&HFIL(t, 1, 0), (size_x*size_y)/n, MPI_DOUBLE, &HFILg(t, 0, 0), (size_x*size_y)/n , MPI_DOUBLE, MASTER, MPI_COMM_WORLD);
   
    //export par le processus root
    if (file_export && rang == MASTER) {
      /*printf("Processus #%d:\t\texport step %d\n", rang, t);*/
      export_step(file, t);
      
    }

    //Les processus envoient les lignes exterieures aux processus voisins
    //Les processus d index pair envoient puis recoivent, les index impairs font l inverse
    if (rang %2 ==0){

      if (rang >0){
	//envoi de la ligne au dessus
	
	MPI_Send(&HPHY(t, 1, 0), size_y, MPI_DOUBLE, rang - 1, 99, MPI_COMM_WORLD);
	MPI_Send(&UPHY(t, 1, 0), size_y, MPI_DOUBLE, rang - 1, 99, MPI_COMM_WORLD);
	MPI_Send(&VPHY(t, 1, 0), size_y, MPI_DOUBLE, rang - 1, 99, MPI_COMM_WORLD);
      }

      if (rang < n-1){
	//envoi de la ligne en dessous (avant derniere ligne du tabeau local de taille size_x/n +2)
	MPI_Send(&HPHY(t, size_x/n, 0), size_y, MPI_DOUBLE, rang+1, 99, MPI_COMM_WORLD);
	MPI_Send(&UPHY(t, size_x/n, 0), size_y, MPI_DOUBLE, rang+1, 99, MPI_COMM_WORLD);
	MPI_Send(&VPHY(t, size_x/n, 0), size_y, MPI_DOUBLE, rang+1, 99, MPI_COMM_WORLD);
      }

      if (rang > 0){
	MPI_Recv(&HPHY(t, 0, 0), size_y, MPI_DOUBLE, rang -1, 99, MPI_COMM_WORLD, &status);
	MPI_Recv(&UPHY(t, 0, 0), size_y, MPI_DOUBLE, rang -1, 99, MPI_COMM_WORLD, &status);
	MPI_Recv(&VPHY(t, 0, 0), size_y, MPI_DOUBLE, rang -1, 99, MPI_COMM_WORLD, &status);
      }
      
      if (rang < n-1){
	MPI_Recv(&HPHY(t, 1 + size_x/n, 0), size_y, MPI_DOUBLE, rang +1, 99, MPI_COMM_WORLD, &status);
	MPI_Recv(&UPHY(t, 1 + size_x/n, 0), size_y, MPI_DOUBLE, rang +1, 99, MPI_COMM_WORLD, &status);
	MPI_Recv(&VPHY(t, 1 + size_x/n, 0), size_y, MPI_DOUBLE, rang +1, 99, MPI_COMM_WORLD, &status);
      }
    }
    
    else{

      if (rang > 0){
	MPI_Recv(&HPHY(t, 0, 0), size_y, MPI_DOUBLE, rang -1, 99, MPI_COMM_WORLD, &status);
	MPI_Recv(&UPHY(t, 0, 0), size_y, MPI_DOUBLE, rang -1, 99, MPI_COMM_WORLD, &status);
	MPI_Recv(&VPHY(t, 0, 0), size_y, MPI_DOUBLE, rang -1, 99, MPI_COMM_WORLD, &status);
      }
      
      if (rang < n-1){
	MPI_Recv(&HPHY(t, 1 + size_x/n, 0), size_y, MPI_DOUBLE, rang +1, 99, MPI_COMM_WORLD, &status);
	MPI_Recv(&UPHY(t, 1 + size_x/n, 0), size_y, MPI_DOUBLE, rang +1, 99, MPI_COMM_WORLD, &status);
	MPI_Recv(&VPHY(t, 1 + size_x/n, 0), size_y, MPI_DOUBLE, rang +1, 99, MPI_COMM_WORLD, &status);
      }

      if (rang >0){
	//envoi de la ligne au dessus
	MPI_Send(&HPHY(t, 1, 0), size_y, MPI_DOUBLE, rang - 1, 99, MPI_COMM_WORLD);
	MPI_Send(&UPHY(t, 1, 0), size_y, MPI_DOUBLE, rang - 1, 99, MPI_COMM_WORLD);
	MPI_Send(&VPHY(t, 1, 0), size_y, MPI_DOUBLE, rang - 1, 99, MPI_COMM_WORLD);
      }

      if (rang < n-1){
	//envoi de la ligne en dessous (avant derniere ligne du tabeau local de taille size_x/n +2)
	MPI_Send(&HPHY(t, size_x/n, 0), size_y, MPI_DOUBLE, rang+1, 99, MPI_COMM_WORLD);
	MPI_Send(&UPHY(t, size_x/n, 0), size_y, MPI_DOUBLE, rang+1, 99, MPI_COMM_WORLD);
	MPI_Send(&VPHY(t, size_x/n, 0), size_y, MPI_DOUBLE, rang+1, 99, MPI_COMM_WORLD);
      }
    }
      
    
    if (t == 2) {
      dt = svdt;
    }
  }

    //export par le processus root
    if (file_export && rang == MASTER) {
    //printf("Processus #%d:\t\texport step %d\n", rang, t);
    export_step(file, 1);
    }
    
    if (file_export && rang == MASTER) {
    //printf("Processus #%d:\t\tfinalize export\n", rang);
    finalize_export(file);
    }
}
