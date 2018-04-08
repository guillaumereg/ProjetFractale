#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "fractal.h"

int main(int argc, char *argv[])
{
  //options de bases
  int plusieursFichiers = 0;
  int maxThreads = 1;
  int entree = 0;
  char * fichierSortie;
  int i=1;

  //vérifications nombres arguments de bases
  if (argc < 3) {
    printf("Trop petit nombre d'arguments\n");
    exit(EXIT_FAILURE);
  }

  //vérifications nombres arguments si option -d activée en premier
  if (strcmp(argv[1], "-d")==0){
    i++;
    if(argc<3){
      printf("Trop petit nombre d'arguments\n");
      exit(EXIT_FAILURE);
    }
    plusieursFichiers = 1;
    //vérifications nombres arguments si option -maxthreads activée en plus de l'option -d
    if (strcmp(argv[2], "--maxthreads")==0){
      i=i+2;
      if(argc<5){
        printf("Trop petit nombre d'arguments\n");
        exit(EXIT_FAILURE);
      }
      maxThreads = atoi(argv[3]);
    }
  }
  //vérifications nombres arguments si  option -maxthreads activée en premier
  else if (strcmp(argv[1], "--maxthreads")==0){
      i=i+2;
      if(argc<5){
        printf("Trop petit nombre d'arguments\n");
        exit(EXIT_FAILURE);
      }
      maxThreads = atoi(argv[2]);
      //vérifications nombres arguments si option -d activée en plus de l'option -maxthreads
      if (strcmp(argv[2], "--maxthreads")==0){
        i++;
        if(argc<5){
          printf("Trop petit nombre d'arguments\n");
          exit(EXIT_FAILURE);
        }
        plusieursFichiers = 1;
      }
  }

  int restant = 0;
  if (plusieursFichiers == 0){
    fichierSortie = argv[argc-1];
    restant = 1;
  }

  while (i<argc-restant){
    i++;
  }
  return EXIT_SUCCESS;

}
