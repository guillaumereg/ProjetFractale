#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "libfractal/fractal.h"


#define TAILLE_MAX 1000 /* Tableau de taille 1000 */

char * filename; /* fichier d'entrée qu'on lit actuellement */
char * fichierSortie; /* fichier de sortie */
int plusieursFichiers = 0; /* pour savoir il s'il faut un fichier pour chaque fractale */

double plusGrandeMoyenne = 0; /* stocke la plus grande moyenne de fractale actuelle */
struct fractal * fracMax; /* pointeur vers la fractale avec la plus grande moyenne */

struct fractal * tabFractal[4]; /* tableau de fractales non calculées */
int tableauRempli = 0; /* permet d'indiquer s'il reste de la place dans le tableau */

struct fractal * tabFractalCalculee[4]; /* tableau de fractales calculées */
int fractaleCalculee = 0; /* permet d'indiquer s'il reste de la place dans le tableau */

pthread_cond_t condStockage = PTHREAD_COND_INITIALIZER; /* Création de la condition */
pthread_mutex_t mutexLecture = PTHREAD_MUTEX_INITIALIZER; /* Création du mutex */

pthread_cond_t condCalcul = PTHREAD_COND_INITIALIZER; /* Création de la condition */
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
			    pthread_cond_wait (&condStockage, &mutexLecture);
			    pthread_mutex_unlock (&mutexLecture);

          int a = 0;
          while(a<4 && tabFractal[a]!=NULL){
            a++;
          }
          tabFractal[a] = fracActu;
        }
        tableauRempli++;

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
  struct fractal * fracActu = tabFractal[a];
  tabFractal[a] = NULL;
  tableauRempli --;
  pthread_mutex_lock (&mutexLecture);
	pthread_cond_signal (&condStockage);
  pthread_mutex_unlock (&mutexLecture);
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
    fracMax = fracActu;
  }

  if (fractaleCalculee == 4){
    pthread_mutex_lock (&mutexCalcul);
    pthread_cond_wait (&condCalcul, &mutexCalcul);
    pthread_mutex_unlock (&mutexCalcul);

    int a = 0;
    while(a<4 && tabFractalCalculee[a]!=NULL){
      a++;
    }
    tabFractalCalculee[a] = fracActu;
  }
  fractaleCalculee++;
  pthread_exit(NULL); /* Fin du thread */
}

void * threadEcrivain(void* arg){
  printf("choper les fractales calcluées dans le deuxième tableau et les écrire dans un/des fichiers ");
  pthread_exit(NULL); /* Fin du thread */
}
