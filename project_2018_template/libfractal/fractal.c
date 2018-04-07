#include <stdlib.h>
#include "fractal.h"

struct fractal *fractal_new(const char *name, int width, int height, double a, double b)
{
    struct fractal * frac = (struct fractal *)malloc(sizeof(struct fractal));
    if (frac == NULL){
      return NULL;
    }
    frac->name = (char *)malloc(sizeof(name));
    if(frac->name == NULL){
      return NULL;
    }
    char* strcpy(frac->name, name);
    frac->width = width;
    frac->height = height;
    frac->a = a;
    frac->b = b;
    frac->valeur = (char *)malloc(sizeof(int)*width*height);
    if(frac->valeur == NULL){
      return NULL;
    }
    return frac;
}

void fractal_free(struct fractal *f)
{
    free (f->name);
    free (f->valeur);
    free(f);
}

const char *fractal_get_name(const struct fractal *f)
{
    if (f == NULL){
      return NULL;
    }
    return f->name;
}

int fractal_get_value(const struct fractal *f, int x, int y)
{
  if (f == NULL){
    return 0;
  }
  return f->valeur[x][y];
}

void fractal_set_value(struct fractal *f, int x, int y, int val)
{
  if (f != NULL){
    f->valeur[x][y] = val;
  }
}

int fractal_get_width(const struct fractal *f)
{
  if (f == NULL){
    return 0;
  }
  return f->width;
}

int fractal_get_height(const struct fractal *f)
{
  if (f == NULL){
    return 0;
  }
  return f->height;
}

double fractal_get_a(const struct fractal *f)
{
  if (f == NULL){
    return 0;
  }
  return f->a;
}

double fractal_get_b(const struct fractal *f)
{
  if (f == NULL){
    return 0;
  }
  return f->b;
}
