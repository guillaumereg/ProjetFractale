#include <stdlib.h>
#include "fractal.h"
#include <tools.c>

struct fractal *fractal_new(const char *name, int width, int height, double a, double b)
{
    struct fractal * frac = (struct fractal *)malloc(sizeof(struct fractal));
    if (frac == NULL){
      return NULL;
    }
    frac->name = name;
    frac->width = width;
    frac->height = height;
    frac->a = a;
    frac->b = b;
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
    return NULL;
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
    return NULL;
  }
  return f->width;
}

int fractal_get_height(const struct fractal *f)
{
  if (f == NULL){
    return NULL;
  }
  return f->height;
}

double fractal_get_a(const struct fractal *f)
{
  if (f == NULL){
    return NULL;
  }
  return f->a;
}

double fractal_get_b(const struct fractal *f)
{
  if (f == NULL){
    return NULL;
  }
  return f->b;
}
