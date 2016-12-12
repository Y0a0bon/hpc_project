#include <math.h>
#include <shalw.h>
#include <stdio.h>

void gauss_init(void) {
  double gmx, gmy, gsx, gsy;
  
  gmx = size_x * dx / 2 ;
  gmy = size_y * dy / 2 ;
  gsx = 25000 ;
  gsy = 25000 ;
  
  if (rang == MASTER){
    //On initialise le tableau complet
    for (int i = 0; i < size_x;  i++) {
      for (int j = 0; j < size_y ; j++) {
	HFILg(0, i, j) = height *
	  (exp(- pow((i * dx - gmx) / gsx, 2) / 2.)) *
	  (exp(- pow((j * dy - gmy) / gsy, 2) / 2.)) ;
      }
    }

  }
  // on initialise les bandes entieres dans les processus esclaves
  // on rajoute (rang)*size_x/n - 1 pour "décaler" le calcul des valeurs de chaque bande
  for (int i = 0; i < size_x/n + 2;  i++) {

    for (int j = 0; j < size_y ; j++) {
      //calcul des indices correspondants dans le tableau global
      int iglo=i + (rang)*size_x/n -1;
      int jglo=j;
	  
      if(0 <= iglo && iglo < size_x){
	HFIL(0, i, j) = height *
	  (exp(- pow((iglo * dx - gmx) / gsx, 2) / 2.)) *
	  (exp(- pow((jglo * dy - gmy) / gsy, 2) / 2.)) ;
      }
    }
    
  }
  
}
