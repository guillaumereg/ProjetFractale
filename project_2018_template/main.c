#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>
#include "libfractal/fractal.h"

/* Structure du buffer */
typedef struct{
    struct fractal **buf;          /* Buffer partagé */
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
void* threadLecteur ();
/* Prototype fonction pour calculer des fractales*/
void* threadCalculateur ();
/* Prototype fonction pour écrire dans des fichiers*/
void* threadEcrivain ();


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
  int maxThreads = 4;
  int entree = 0;
  int i=1;
  int j=0;

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


  pthread_t thread[maxThreads];
  int err;

  for(j=0;i<maxThreads;j++) {
    err=pthread_create(&(thread[j]),NULL,&threadLecteur,NULL);
    if(err!=0){
      perror("pthread_create");
    }
  }

  for(j=0;i<maxThreads;j++) {
    err=pthread_create(&(thread[j]),NULL,&threadCalculateur,NULL);
    if(err!=0){
      perror("pthread_create");
    }
  }

  for(j=0;i<maxThreads;j++) {
    err=pthread_create(&(thread[j]),NULL,&threadEcrivain,NULL);
    if(err!=0){
      perror("pthread_create");
    }
  }

  for(j=maxThreads-1;j>=0;j--) {
    err=pthread_join(thread[j],NULL);
    if(err!=0){
      perror("pthread_join");
    }
  }



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
void *threadLecteur(){

  /* si l'option "-" est activée, on lit sur l'entrée standart */
  if(strcmp(filename,"-") == 0){

    filename = STDIN_FILENO;

  }
  FILE * fichier = NULL;
  fichier = fopen(filename, "r");  /* on ouvre le fichier en mode lecture */
  if (fichier == NULL){
    printf("Nom de fichier invalide : %s \n" , filename);
    exit(EXIT_FAILURE);
  }

  char * chaine = NULL;
  fgets(chaine, TAILLE_MAX, fichier);  /* on lit le fichier ligne par ligne */
  while(chaine!=NULL){   /*tant qu'il y a des lignes dans le fichier */
    char *result = NULL;
    result = strtok(chaine, " ");

    char * tableChaine [5];
    int i=0;
    while (result != NULL){   /* on fragmente la ligne qu'on lit en fonction des espaces */
        strcpy(tableChaine[i], result);
        i++;
        result = strtok( NULL, " ");
    }
    if (strcmp(tableChaine[0] , "#")!=0){
      if (i!=5){
        printf("format de fractale invalide : %s \n" , filename);  /* dans les cas où il n'y pas pas 5 arguments */
        exit(EXIT_FAILURE);
      }
      char * name = tableChaine[0];
      int width = atoi(tableChaine[1]);
      int height = atoi(tableChaine[2]);
      double a = atoi(tableChaine[3]);
      double b = atoi(tableChaine[4]);
      struct fractal * fracActu = fractal_new(name,width,height,a,b); /* on crée la fractale en fonction des valeurs lues sur la ligne */

      sbuf_insert(buffer_lecteur_calculateur, fracActu); /* on insère la nouvelle fractale sur le buffer associé */


    }
    fgets(chaine, TAILLE_MAX, fichier);
  }

  fclose(fichier);
  pthread_exit(NULL); /* Fin du thread */
}



/* Fonction pour calculer les fractales */
void * threadCalculateur(){

  int bool = 0;

  while (bool == 0){

    struct fractal * fracActu = sbuf_remove(buffer_lecteur_calculateur); /* on sélectionne une fractale depuis le buffer */
    if (fracActu == NULL){
      bool = 1;
      pthread_exit(NULL); /* Fin du thread */
    }

    double moyenne;
    int i;
    for(i=0;i<fractal_get_height(fracActu);i++){   /* on calcule la valeur de chaque pixel de la fractale */
      int j;
      for(j=0;j<fractal_get_width(fracActu);j++){
        int val = fractal_compute_value(fracActu, i, j);
        moyenne += val;
      }
    }
    moyenne = moyenne/(fractal_get_height(fracActu)*fractal_get_width(fracActu)); /* calcul de la valeur moyenne de la fractale */
    if (moyenne > plusGrandeMoyenne){  /* si la moyenne de la fractale atuelle est plus grande que la moyenne max */
      plusGrandeMoyenne = moyenne; /*on sauvegarde la fractale actuelle comme étant la max */
      fracMax = fracActu;
    }
    if (plusieursFichiers == 1){
      sbuf_insert(buffer_calculateur_ecrivain, fracActu); /* on insère la fractale calculée dans le buffer associé */
    }
  }
  pthread_exit(NULL); /* Fin du thread */
}




/* Fonction pour écrire les fractales */
void * threadEcrivain(){

  int bool = 0;

  while (bool == 0){

    struct fractal * fracActu = sbuf_remove(buffer_calculateur_ecrivain); /* on sélectionne une fractale depuis le buffer */
    if (fracActu == NULL){
      bool = 1;
      pthread_exit(NULL); /* Fin du thread */
    }

    char * fichier = NULL;
    fichier = fracActu->name;   /*le nom du fichier = le nom de la fractale */

    if (fichier == NULL){
      printf("Erreur dans le fichier de sortie \n");
      exit(EXIT_FAILURE);
    }
    write_bitmap_sdl(fracActu, fichier); /*on crée un fichier pour la fractale */

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
    *(sp->buf+(sp->front))=NULL;
    sem_post(&sp->mutex);
    sem_post(&sp->slots);
    return fracActu;
}
