/*********** Librerías utilizadas **************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/************* Definiciones ********************/

// Códigos
#define STRING      256
#define NUM         257
#define LBRACE      258
#define RBRACE      259
#define LBRACKET    260
#define RBRACKET    261
#define COMMA       262
#define COLON       263
#define TRUE        264
#define FALSE       265
#define NUL         266
//#define EOF         -1

#define TAMBUFF     5
#define TAMLEX      50
#define TAMHASH     101

/************* Estructuras ********************/

typedef struct entrada {
    int compLex;
    char lexema[TAMLEX];
} entrada;

typedef struct {
    int compLex;
    entrada *pe;
} token;

/************* Prototipos ********************/
void insertar(entrada e);
entrada *buscar(const char *clave);
void initTabla();
void initTablaSimbolos();
void getToken();
