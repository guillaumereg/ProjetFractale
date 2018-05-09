#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include "libfractal/fractal.h"


#define TAILLE_MAX 1000 /* Tableau de taille 1000 */

char * filename; /* fichier d'entrée qu'on lit actuellement */
char * fichierSortie; /* fichier de sortie */
int plusieursFichiers = 0; /* pour savoir il s'il faut un fichier pour chaque fractale */

double plusGrandeMoyenne = 0; /* stocke la plus grande moyenne de fractale actuelle */
struct fractal * fracMax; /* pointeur vers la fractale avec la plus grande moyenne */

pthread_cond_t condStockage = PTHREAD_COND_INITIALIZER; /* Création de la condition */
pthread_mutex_t mutexLecture = PTHREAD_MUTEX_INITIALIZER; /* Création du mutex */

pthread_cond_t condCalcul = PTHREAD_COND_INITIALIZER; /* Création de la condition */
pthread_mutex_t mutexCalcul = PTHREAD_MUTEX_INITIALIZER; /* Création du mutex */

sem_t empty_1;
sem_t full_1;

sem_t empty_2;
sem_t full_2;


void* threadLecteur (void* arg);
void* threadCalculateur (void* arg);
void* threadEcrivain (void* arg);


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


/* Fonction pour lire les fichiers */
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


        /* Ici faut faire le truc pour le buffer */




      }
      fgets(chaine, TAILLE_MAX, fichier);
    }
    fclose(fichier);
  }
  pthread_exit(NULL); /* Fin du thread */
}



/* Fonction pour calculer les fractales */
void * threadCalculateur(void* arg){


  struct fractal * fracActu ;

/* Ici faut faire le truc pour le buffer */


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


/* Ici faut faire un truc pour le buffer */


  pthread_exit(NULL); /* Fin du thread */
}




/* Fonction pour écrire les fractales */
void * threadEcrivain(void* arg){
  printf("choper les fractales calculées dans le deuxième tableau et les écrire dans un/des fichiers ");
  pthread_exit(NULL); /* Fin du thread */
}



/* Structure du buffer */
typedef struct {
    int *buf;          /* Buffer partagé */
    int n;             /* Nombre de slots dans le buffer */
    int front;         /* buf[(front+1)%n] est le premier élément */
    int rear;          /* buf[rear%n] est le dernier */
    sem_t mutex;       /* Protège l'accès au buffer */
    sem_t slots;       /* Nombre de places libres */
    sem_t items;       /* Nombre d'items dans le buffer */
} sbuf_t;



/* Fonction pour initialiser un buffer */
void sbuf_init(sbuf_t *sp, int n){
    sp->buf = calloc(n, sizeof(int));
    sp->n = n;                       /* Buffer content les entiers */
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
void sbuf_insert(sbuf_t *sp, int item){
    sem_wait(&sp->slots);
    sem_wait(&sp->mutex);
    sp->rear++;
    sp->rear=(sp->rear)%(sp->n);
    *(sp->buf+(sp->rear))=item;
    sem_post(&sp->mutex);
    sem_post(&sp->items);
}



/* Fonction pour retirer un élément du buffer */
int sbuf_remove(sbuf_t *sp){
    int x;
    sem_wait(&sp->items);
    sem_wait(&sp->mutex);
    sp->front++;
    sp->front=(sp->front)%(sp->n);
    x=*(sp->buf+(sp->front));
    *(sp->buf+(sp->front))=0;
    sem_post(&sp->mutex);
    sem_post(&sp->slots);
    return x;
}

/* Fonction producteur */
void producer(){
  int a;
  while(true){
    a=produce(a);
    sem_wait(&empty);
    pthread_mutex_lock(&mutex);
    /* section critique */
    insert_item();
    pthread_mutex_unlock(&mutex);
    sem_post(&full);
  }
}

/* Fonction consommateur */
void consumer(void){
  int a;
  while(true){
    sem_wait(&full);
    pthread_mutex_lock(&mutex);
    /* section critique */
    a=remove(a);
    pthread_mutex_unlock(&mutex);
    sem_post(&empty);
  }
}
