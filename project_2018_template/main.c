#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "fractal.h"

#define TAILLE_MAX 1000 /* Tableau de taille 1000 */

int main(int argc, char *argv[])
{
  /*options de bases */
  int plusieursFichiers = 0;
  int maxThreads = 1;
  int entree = 0;
  char * fichierSortie;
  int i=1;

  /*vérifications nombres arguments de bases*/
  if (argc < 3) {
    printf("Trop petit nombre d'arguments\n");
    exit(EXIT_FAILURE);
  }

  /*vérifications nombres arguments si option -d activée en premier*/
  if (strcmp(argv[1], "-d")==0){
    i++;
    if(argc<3){
      printf("Trop petit nombre d'arguments\n");
      exit(EXIT_FAILURE);
    }
    plusieursFichiers = 1;
    /*vérifications nombres arguments si option -maxthreads activée en plus de l'option -d*/
    if (strcmp(argv[2], "--maxthreads")==0){
      i=i+2;
      if(argc<5){
        printf("Trop petit nombre d'arguments\n");
        exit(EXIT_FAILURE);
      }
      maxThreads = atoi(argv[3]);
    }
  }
  /*vérifications nombres arguments si  option -maxthreads activée en premier*/
  else if (strcmp(argv[1], "--maxthreads")==0){
      i=i+2;
      if(argc<5){
        printf("Trop petit nombre d'arguments\n");
        exit(EXIT_FAILURE);
      }
      maxThreads = atoi(argv[2]);
      /*vérifications nombres arguments si option -d activée en plus de l'option -maxthreads*/
      if (strcmp(argv[2], "--maxthreads")==0){
        i++;
        if(argc<5){
          printf("Trop petit nombre d'arguments\n");
          exit(EXIT_FAILURE);
        }
        plusieursFichiers = 1;
      }
  }

  /*Y a t'il un fichier de sortie?*/
  int restant = 0;
  if (plusieursFichiers == 0){
    fichierSortie = argv[argc-1];
    restant = 1;
  }

  /*Lecture des fractales*/
  while (i<argc-restant){
    /*Lecture des fractales sur l'entrée standart*/
    if (strcmp(argv[i], "-")==0){
      if(entree == 1){
        printf("Plusieurs fois l'arguments pour lire les entrées\n");
        exit(EXIT_FAILURE);
      }
      entree = 1;
    }
    /*Lecture des fractales dans des fichiers*/
    else {
      FILE * fichier = NULL;
      fichier = fopen(argv[i], "r");
      if (fichier == NULL){
        printf("Nom de fichier invalide : %s \n" , argv[i]);
        exit(EXIT_FAILURE);
      }

      char * chaine;
      while(fgets(chaine, TAILLE_MAX, fichier)!=NULL){
        char *result = NULL;
        result = strtok(chaine, " ");

        char tableChaine [5];
        int j=0;
        while (result != NULL){
           strcpy(tableChaine[i], result);
           i++;
           result = strtok( NULL, " ");
        }
        if (tableChaine[0] != '#'){
          if (tableChaine[4] == NULL){
            printf("format de fractale invalide : %s \n" , argv[i]);
            exit(EXIT_FAILURE);
          }
          char * name = tableChaine[0];
          int width = tableChaine[1];
          int height = tableChaine[2];
          double a = tableChaine[3];
          double b = tableChaine[4];
          struct fractal * fracActu = fractal_new(name,width,height,a,b);
        }
        fclose(fichier);
      }
    }
  }
  return EXIT_SUCCESS;
}
