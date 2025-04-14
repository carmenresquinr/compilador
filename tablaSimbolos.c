#include "jsonlex.h"

/*********************HASH************************/
entrada *tabla;
int tamTabla = TAMHASH;
int elems = 0;

int h(const char *k, int m) {
    unsigned h = 0, g;
    int i;
    for (i = 0; i < strlen(k); i++) {
        h = (h << 4) + k[i];
        if ((g = h & 0xf0000000)) {
            h = h ^ (g >> 24);
            h = h ^ g;
        }
    }
    return h % m;
}

void initTabla() {
    int i = 0;
    tabla = (entrada *) malloc(tamTabla * sizeof(entrada));
    for (i = 0; i < tamTabla; i++) {
        tabla[i].compLex = -1;
    }
}

void insertar(entrada e) {
    int pos;
    if (++elems >= tamTabla / 2) {
        tamTabla *= 2;
        tabla = (entrada *) realloc(tabla, tamTabla * sizeof(entrada));
    }
    pos = h(e.lexema, tamTabla);
    while (tabla[pos].compLex != -1) {
        pos = (pos + 1) % tamTabla;
    }
    tabla[pos] = e;
}

entrada *buscar(const char *clave) {
    int pos = h(clave, tamTabla);
    while (tabla[pos].compLex != -1 && strcmp(tabla[pos].lexema, clave) != 0) {
        pos = (pos + 1) % tamTabla;
    }
    return &tabla[pos];
}

void initTablaSimbolos() {
    insertar((entrada){LBRACE, "{"});
    insertar((entrada){RBRACE, "}"});
    insertar((entrada){LBRACKET, "["});
    insertar((entrada){RBRACKET, "]"});
    insertar((entrada){COMMA, ","});
    insertar((entrada){COLON, ":"});
    insertar((entrada){TRUE, "true"});
    insertar((entrada){FALSE, "false"});
    insertar((entrada){NUL, "null"});
}
