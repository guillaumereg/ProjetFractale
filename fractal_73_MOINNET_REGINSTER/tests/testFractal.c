#include "CUnit/CUnit.h"
#include "CUnit/Basic.h"
#include <stdio.h>
#include <stdlib.h>
#include "../libfractal/fractal.h"

struct fractal * new_frac = NULL;
char * name = "fractalNuméro1";
int width = 1;
int height = 2;
double a = 0;
double b = -1;

int init_suite(void) { return 0; }
int clean_suite(void) { return 0; }

/*vérification de si la fractale est correctement créée*/
void test_fractale_cree(void) {
  new_frac = (struct fractal *)fractal_new(name,width,height,a,b);
  CU_ASSERT_PTR_NOT_NULL(new_frac);
}

/*vérification si la fonction get_name marche*/
void test_get_name(void) {
  CU_ASSERT_PTR_EQUAL(fractal_get_name(new_frac), *name);
}

/*vérification si la fonction get_width marche*/
void test_get_width(void) {
  CU_ASSERT_EQUAL(fractal_get_width(new_frac), width);
}

/*vérification si la fonction get_height marche*/
void test_get_height(void) {
  CU_ASSERT_EQUAL(fractal_get_height(new_frac), height);
}

/*vérification si la fonction get_a marche*/
void test_get_a(void) {
  CU_ASSERT_EQUAL(fractal_get_a(new_frac), a);
}

/*vérification si la fonction get_b marche*/
void test_get_b(void) {
  CU_ASSERT_EQUAL(fractal_get_b(new_frac), b);
}

/*vérification si la fonction set_value et get_value marchent*/
void test_value(void) {
  fractal_set_value(new_frac, 0 , 0, 140);
  int valeur = fractal_get_value(new_frac, 0 , 0);
  CU_ASSERT_EQUAL(valeur, 140);
}

/*vérification de si la fractale est correctement libérée*/
void test_fractale_liberee(void) {
  free(new_frac);
  CU_ASSERT_PTR_NULL(new_frac);
}

/*fonction main*/
int main ( void ){

   CU_pSuite pSuite = NULL;

   if ( CUE_SUCCESS != CU_initialize_registry() ){
      return CU_get_error();
    }

   pSuite = CU_add_suite( "suite_test_fractale", init_suite, clean_suite );  /*création de la suite de test*/
   if ( NULL == pSuite ) {
      CU_cleanup_registry();
      return CU_get_error();
   }

/*on appelle un à un les différents tests créés */
   if ( (NULL == CU_add_test(pSuite, "test_fractale_cree", test_fractale_cree))||
         (NULL == CU_add_test(pSuite, "test_get_name", test_get_name))||
         (NULL == CU_add_test(pSuite, "test_get_width", test_get_width))||
         (NULL == CU_add_test(pSuite, "test_get_height", test_get_height))||
         (NULL == CU_add_test(pSuite, "test_get_a", test_get_a))||
         (NULL == CU_add_test(pSuite, "test_get_b", test_get_b))||
         (NULL == CU_add_test(pSuite, "test_value", test_value))||
         (NULL == CU_add_test(pSuite, "test_fractale_liberee", test_fractale_liberee))
      )
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

 }
