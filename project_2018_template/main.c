#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "libfractal/fractal.h"


#define TAILLE_MAX 1000 /* Tableau de taille 1000 */

char * filename;
char * fichierSortie;
int plusieursFichiers = 0;
double plusGrandeMoyenne = 0;

struct fractal * tabFractal[4];
int tableauRempli = 0;

struct fractal * tabFractalCalculee[4];
int fractaleCalculee = 0;


pthread_cond_t condStockage = PTHREAD_COND_INITIALIZER; /* Création de la condition */
pthread_cond_t condCalcul = PTHREAD_COND_INITIALIZER; /* Création de la condition */

pthread_mutex_t mutexLecture = PTHREAD_MUTEX_INITIALIZER; /* Création du mutex */
pthread_mutex_t mutexCalcul = PTHREAD_MUTEX_INITIALIZER; /* Création du mutex */


void* threadLecteur (void* arg);
void* threadCalculateur (void* arg);
void* threadEcrivain (void* arg);

int main(int argc, char *argv[]){

  /*options de bases */
  int maxThreads = 1;
  int entree = 0;
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
        printf("Plusieurs fois l'argument pour lire les entrées\n");
        exit(EXIT_FAILURE);
      }
      entree = 1;
    }
    filename = argv[i];

    printf("gérer le fait qu'il y a un nombre inconnu de thread");

    pthread_t monThreadLecteur;
    pthread_t monThreadCalculateur;
    pthread_t monThreadEcrivain;

    pthread_create (&monThreadLecteur, NULL, threadLecteur, (void *)NULL);
    pthread_create (&monThreadCalculateur, NULL, threadCalculateur, (void *)NULL);
    pthread_create (&monThreadEcrivain, NULL, threadEcrivain, (void *)NULL);
  }
  return EXIT_SUCCESS;
}

void *threadLecteur(void* arg){

  if(strcmp(filename,"-") == 0){
    printf("à coder");
  }else {
    FILE * fichier = NULL;
    fichier = fopen(filename, "r");
    if (fichier == NULL){
      printf("Nom de fichier invalide : %s \n" , filename);
      exit(EXIT_FAILURE);
    }

    char * chaine;
    fgets(chaine, TAILLE_MAX, fichier);
    while(chaine!=NULL){
      char *result = NULL;
      result = strtok(chaine, " ");

      char * tableChaine [5];
      int i=0;
      while (result != NULL){
         strcpy(tableChaine[i], result);
         i++;
         result = strtok( NULL, " ");
      }
      if (strcmp(tableChaine[0] , "#")!=0){
        if (i!=5){
          printf("format de fractale invalide : %s \n" , filename);
          exit(EXIT_FAILURE);
        }
        char * name = tableChaine[0];
        int width = atoi(tableChaine[1]);
        int height = atoi(tableChaine[2]);
        double a = atoi(tableChaine[3]);
        double b = atoi(tableChaine[4]);
        struct fractal * fracActu = fractal_new(name,width,height,a,b);

        if (tableauRempli == 4){
          pthread_mutex_lock (&mutexLecture);
			    pthread_cond_signal (&condStockage);
			    pthread_mutex_unlock (&mutexLecture);

          int a = 0;
          while(a<4 && tabFractal[a]!=NULL){
            a++;
          }
          tabFractal[a] = fracActu;
        }
        tableauRempli++;

        printf (" bref faut un truc qui marche qui permet d'écrire les fractales dans un tableau tout en attendant quand le tableau est rempli");
        printf (" et écrire dès qu'une place se libère ");
      }
      fgets(chaine, TAILLE_MAX, fichier);
    }
    fclose(fichier);
  }
  pthread_exit(NULL); /* Fin du thread */
}

void * threadCalculateur(void* arg){

  int a = 0;
  while(a<4 && tabFractal[a]!=NULL){
    a++;
  }

  printf (" bref dès que le thread est libre et qu'il y a une fractale dans le tableau, copier la fractale et la supprimer du tableau");
  struct fractal * fracActu ;
  double moyenne;
  int i;
  for(i=0;i<fracActu->height;i++){
    int j;
    for(j=0;j<fracActu->width;j++){
      int val = fractal_compute_value(fracActu, i, j);
      moyenne += val;
    }
  }
  moyenne = moyenne/(fracActu->height*fracActu->width);
  if (moyenne > plusGrandeMoyenne){
    plusGrandeMoyenne = moyenne;
    printf("Sauvegarder la fractale avec la plus grande moyenne");
  }
  printf (" bref faut un truc qui marche qui permet d'écrire les fractales calculée dans un 2ème tableau tout en attendant quand le tableau est rempli ");
  printf (" et écrire dès qu'une place ce libère ");
  pthread_exit(NULL); /* Fin du thread */
}

void * threadEcrivain(void* arg){
  printf("choper les fractales calcluées dans le deuxième tableau et les écrire dans un/des fichiers ");
  pthread_exit(NULL); /* Fin du thread */
}
