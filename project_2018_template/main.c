#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include "libfractal/fractal.h"

/* Structure du buffer */
typedef struct{
    int *buf;          /* Buffer partagé */
    int n;             /* Nombre de slots dans le buffer */
    int front;         /* buf[(front+1)%n] est le premier élément */
    int rear;          /* buf[rear%n] est le dernier */
    sem_t mutex;       /* Protège l'accès au buffer */
    sem_t slots;       /* Nombre de places libres */
    sem_t items;       /* Nombre d'items dans le buffer */
} sbuf_t;


#define TAILLE_MAX 1000 /* Tableau de taille 1000 */
int SIZE_FRACTALE = sizeof(struct fractal);

char * filename; /* fichier d'entrée qu'on lit actuellement */
char * fichierSortie; /* fichier de sortie */
int plusieursFichiers = 0; /* pour savoir il s'il faut un fichier pour chaque fractale */

double plusGrandeMoyenne = 0; /* stocke la plus grande moyenne de fractale actuelle */
struct fractal * fracMax; /* pointeur vers la fractale avec la plus grande moyenne */

sbuf_t * buffer_lecteur_calculateur;
sbuf_t * buffer_calculateur_ecrivain;

/* Prototype fonction pour lire des fichiers*/
void* threadLecteur (void* arg);
/* Prototype fonction pour calculer des fractales*/
void* threadCalculateur (void* arg);
/* Prototype fonction pour écrire dans des fichiers*/
void* threadEcrivain (void* arg);


/* Prototype fonction pour initialiser un buffer */
void sbuf_init(sbuf_t *sp, int n);
/* Prototype fonction pour libérer un buffer */
void sbuf_clean(sbuf_t *sp);
/* Prototype fonction pour insérer un nouvel élément dans un buffer */
void sbuf_insert(sbuf_t *sp, struct fractal * fracActu);
/* Prototype fonction pour retirer un élément du buffer */
struct fractal * sbuf_remove(sbuf_t *sp);




/*méthode main */
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

  fichierSortie = argv[argc-1];


  /*Lecture des fractales*/
  while (i<argc-1){

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


    pthread_create (&monThreadLecteur,NULL,&threadLecteur,NULL);
    pthread_create (&monThreadCalculateur,NULL,&threadCalculateur,NULL);
    if (plusieursFichiers == 1){
      pthread_create (&monThreadEcrivain,NULL,&threadEcrivain,NULL);
    }

    /*initialisation des buffer*/
    sbuf_init(buffer_lecteur_calculateur,4);
    sbuf_init(buffer_calculateur_ecrivain,4);

  }

  if(plusieursFichiers == 0){
    write_bitmap_sdl(fracMax, fichierSortie);
  }
  sbuf_clean(buffer_lecteur_calculateur);
  sbuf_clean(buffer_calculateur_ecrivain);

  return EXIT_SUCCESS;
}



/* Fonction pour lire les fichiers */
void *threadLecteur(void* arg){

  if(strcmp(filename,"-") == 0){

    filename = STDIN_FILENO;

  }
  FILE * fichier = NULL;
  fichier = fopen(filename, "r");
  if (fichier == NULL){
    printf("Nom de fichier invalide : %s \n" , filename);
    exit(EXIT_FAILURE);
  }

  char * chaine = NULL;
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

      sbuf_insert(buffer_lecteur_calculateur, fracActu);

    }
    fgets(chaine, TAILLE_MAX, fichier);
  }
  fclose(fichier);

  pthread_exit(NULL); /* Fin du thread */
}



/* Fonction pour calculer les fractales */
void * threadCalculateur(void* arg){

  int bool = 0;

  while (bool == 0){

    struct fractal * fracActu = sbuf_remove(buffer_lecteur_calculateur);
    if (fracActu == NULL){
      bool = 1;
      pthread_exit(NULL); /* Fin du thread */
    }

    double moyenne;
    int i;
    for(i=0;i<fractal_get_height(fracActu);i++){
      int j;
      for(j=0;j<fractal_get_width(fracActu);j++){
        int val = fractal_compute_value(fracActu, i, j);
        moyenne += val;
      }
    }
    moyenne = moyenne/(fractal_get_height(fracActu)*fractal_get_width(fracActu));
    if (moyenne > plusGrandeMoyenne){
      plusGrandeMoyenne = moyenne;
      fracMax = fracActu;
    }

    sbuf_insert(buffer_calculateur_ecrivain, fracActu);
  }
  pthread_exit(NULL); /* Fin du thread */
}




/* Fonction pour écrire les fractales */
void * threadEcrivain(void* arg){

  int bool = 0;

  while (bool == 0){

    struct fractal * fracActu = sbuf_remove(buffer_calculateur_ecrivain);
    if (fracActu == NULL){
      bool = 1;
      pthread_exit(NULL); /* Fin du thread */
    }

    char * fichier = NULL;
    fichier = fracActu->name;

    if (fichier == NULL){
      printf("Erreur dans le fichier de sortie \n");
      exit(EXIT_FAILURE);
    }
    write_bitmap_sdl(fracActu, fichier);

  }
  pthread_exit(NULL); /* Fin du thread */
}


/* Fonction pour initialiser un buffer */
void sbuf_init(sbuf_t *sp, int n){
    sp->buf = calloc(n, sizeof(struct fractal));
    sp->n = n;
    sp->front = sp->rear = 0;        /* Buffer vide si front == rear */
    sem_init(&sp->mutex, 0, 1);      /* Exclusion mutuelle */
    sem_init(&sp->slots, 0, n);      /* Au début, n slots vides */
    sem_init(&sp->items, 0, 0);      /* Au début, rien à consommer */
}



/* Fonction pour libérer un buffer */
void sbuf_clean(sbuf_t *sp){
    free(sp->buf);
}



/* Fonction pour insérer un nouvel élément dans un buffer */
void sbuf_insert(sbuf_t *sp, struct fractal * fracActu){
    sem_wait(&sp->slots);
    sem_wait(&sp->mutex);
    sp->rear++;
    sp->rear=(sp->rear)%(sp->n);
    *(sp->buf+(sp->rear)*SIZE_FRACTALE)=fracActu;
    sem_post(&sp->mutex);
    sem_post(&sp->items);
}



/* Fonction pour retirer un élément du buffer */
struct fractal * sbuf_remove(sbuf_t *sp){
    struct fractal * fracActu;
    sem_wait(&sp->items);
    sem_wait(&sp->mutex);
    sp->front++;
    sp->front=(sp->front)%(sp->n);
    fracActu=*(sp->buf+(sp->front)*SIZE_FRACTALE);
    *(sp->buf+(sp->front))=0;
    sem_post(&sp->mutex);
    sem_post(&sp->slots);
    return fracActu;
}
