/*********** Inclusión de cabecera **************/
#include "jsonlex.h"

/************* Variables globales **************/

FILE *archivo;            // Archivo fuente JSON
char lexema[TAMLEX];      // Lexema actual
int numLinea = 1;         // Número de línea actual
int nivelIndentacion = 0; // Para identación
int tokenAnterior = -1;   // Token anterior
token t;                  // Token global para recibir componentes del analizador léxico
FILE *archivoSalida;

// Rutina para manejar errores léxicos
void error(const char *mensaje) {
    fprintf(archivoSalida, "Error Léxico: %s.\n", mensaje);
}

// Obtiene el siguiente token
void getToken() {
    int i = 0;
    char c = 0;
    int estado = 0;
    char msg[41];
    entrada e;

    while ((c = fgetc(archivo)) != EOF) {
        if (c == ' ' || c == '\t') continue;  // Ignorar espacios en blanco
        if (c == '\n') {
            numLinea++;
            continue;
        }

        if (c == '"') {  // Cadenas de texto
            i = 0;
            lexema[i++] = c;
            while ((c = fgetc(archivo)) != '"' && c != EOF) {
                lexema[i++] = c;
                if (i >= TAMLEX) error("Longitud de cadena excede el tamaño máximo");
            }
            lexema[i++] = c;
            lexema[i] = '\0';
            t.compLex = STRING;
            strcpy(e.lexema, lexema);
            insertar(e);
            break;
        } else if (isdigit(c) || c == '-') {  // Números
            i = 0;
            lexema[i++] = c;
            estado = 0;

            while (1) {
                c = fgetc(archivo);
                if (isdigit(c)) {
                    lexema[i++] = c;
                } else if (c == '.' || tolower(c) == 'e') {
                    lexema[i++] = c;
                } else {
                    break;
                }
            }
            lexema[i] = '\0';
            ungetc(c, archivo);

            t.compLex = NUM;
            strcpy(e.lexema, lexema);
            insertar(e);
            break;
        } else if (c == '{') {
            t.compLex = LBRACE;
            t.pe = buscar("{");
            break;
        } else if (c == '}') {
            t.compLex = RBRACE;
            t.pe = buscar("}");
            break;
        } else if (c == '[') {
            t.compLex = LBRACKET;
            t.pe = buscar("[");
            break;
        } else if (c == ']') {
            t.compLex = RBRACKET;
            t.pe = buscar("]");
            break;
        } else if (c == ':') {
            t.compLex = COLON;
            t.pe = buscar(":");
            break;
        } else if (c == ',') {
            t.compLex = COMMA;
            t.pe = buscar(",");
            break;
        } else if (c == 't') {
            if (fgetc(archivo) == 'r' && fgetc(archivo) == 'u' && fgetc(archivo) == 'e') {
                t.compLex = TRUE;
                t.pe = buscar("true");
            } else {
                error("Palabra reservada mal formada: se esperaba 'true'");
            }
            break;
        } else if (c == 'f') {
            if (fgetc(archivo) == 'a' && fgetc(archivo) == 'l' && fgetc(archivo) == 's' && fgetc(archivo) == 'e') {
                t.compLex = FALSE;
                t.pe = buscar("false");
            } else {
                error("Palabra reservada mal formada: se esperaba 'false'");
            }
            break;
        } else if (c == 'n') {
            if (fgetc(archivo) == 'u' && fgetc(archivo) == 'l' && fgetc(archivo) == 'l') {
                t.compLex = NUL;
                t.pe = buscar("null");
            } else {
                error("Palabra reservada mal formada: se esperaba 'null'");
            }
            break;
        } else {
            sprintf(msg, "Caracter inesperado: '%c'", c);
            error(msg);
        }
    }

    if (c == EOF) {
        t.compLex = EOF;
        strcpy(e.lexema, "EOF");
        t.pe = &e;
    }
}


void imprimirIndentacion(int tokenAnterior, int tokenActual, int linea, FILE* archivoSalida) {
    // Imprimir ESPACIO
    if ((tokenAnterior == STRING && tokenActual == COMMA) ||
        (tokenAnterior == NUM && tokenActual == COMMA) ||
        (tokenAnterior == RBRACE && tokenActual == COMMA) ||
        (tokenAnterior == TRUE && tokenActual == COMMA) ||
        (tokenAnterior == FALSE && tokenActual == COMMA) ||
        (tokenAnterior == STRING && tokenActual == COLON) ||
        (tokenAnterior == NUM && tokenActual == COLON) ||
        (tokenAnterior == FALSE && tokenActual == COLON) ||
        (tokenAnterior == TRUE && tokenActual == COLON) ||    
        (tokenAnterior == COLON && tokenActual == STRING) ||
        (tokenAnterior == COLON && tokenActual == NUM) ||
        (tokenAnterior == COLON && tokenActual == FALSE) ||
        (tokenAnterior == COLON && tokenActual == TRUE) ||
        (tokenAnterior == COLON && tokenActual == LBRACKET)) {
        fprintf(archivoSalida, " ");
        return;
    }

    //Imprimir SALTO DE LÍNEA
    if ((tokenAnterior == LBRACKET && tokenActual == -1) ||
        (tokenAnterior == LBRACE && tokenActual == -1) ||
        (tokenAnterior == COMMA && tokenActual == -1) ||
        (tokenAnterior == STRING && tokenActual == RBRACE) ||
        (tokenAnterior == NUM && tokenActual == RBRACE) ||
        (tokenAnterior == RBRACE && tokenActual != COMMA)) {
        fprintf(archivoSalida, "\n");
    }

    //Disminuir la indentación ANTES de imprimir
    if ((tokenAnterior == LBRACKET && tokenActual == RBRACE) ||
        (tokenAnterior == RBRACE && tokenActual == RBRACKET) ||
        (tokenAnterior == RBRACKET && tokenActual == RBRACE) ||
        (tokenAnterior == STRING && tokenActual == RBRACE) ||
        (tokenAnterior == NUM && tokenActual == RBRACE) ||
        (tokenAnterior == NUM && tokenActual == RBRACE) ||
        (tokenAnterior == STRING && tokenActual == RBRACE)) {
        nivelIndentacion--;    
    }
    
    //Aumentar indentación después de imprimir LBRACE o LBRACKET
    if ((tokenAnterior != -1 && tokenActual == LBRACE && tokenAnterior != LBRACKET && tokenAnterior != COMMA) || 
        (tokenActual == LBRACKET)|| (tokenAnterior == LBRACE && tokenActual == STRING && linea !=0) ||
        (tokenAnterior == LBRACKET && tokenActual == LBRACE)) {
        nivelIndentacion++;
    }
    
    //Imprimir tabulación actual
    for (int i = 0; i < nivelIndentacion; i++) {
        fprintf(archivoSalida, "\t");
    }   
    
    // Caso inicial
    if (tokenAnterior == -1 && tokenActual == LBRACE) {
        nivelIndentacion++;
    }
}

int main(int argc, char *argv[]) {
    initTabla();
    initTablaSimbolos();
    int linea = 0;

    if (argc > 2) {
        if (!(archivo = fopen(argv[1], "r"))) {
            printf("Archivo fuente no encontrado.\n");
            exit(1);
        }
        if (!(archivoSalida = fopen(argv[2], "w"))) {
            printf("No se puede abrir el archivo de salida.\n");
            fclose(archivo);
            exit(1);
        }

        tokenAnterior = -1;
        getToken();  // Cargar primer token en la variable global `t`

        while (t.compLex != EOF) {
            imprimirIndentacion(tokenAnterior, t.compLex, linea, archivoSalida);

            switch (t.compLex) {
                case LBRACE:
                    fprintf(archivoSalida, "L_LLAVE\n");
                    if (tokenAnterior != -1) {
                       linea = 1;
                    }
                    break;
                case RBRACE:
                    fprintf(archivoSalida, "R_LLAVE");
                    break;
                case LBRACKET:
                    fprintf(archivoSalida, "L_CORCHETE\n");
                    break;
                case RBRACKET:
                    fprintf(archivoSalida, "R_CORCHETE\n");
                    break;
                case STRING:
                    fprintf(archivoSalida, "STRING");
                    break;
                case NUM:
                    fprintf(archivoSalida, "NUMBER");
                    break;
                case COMMA:
                    fprintf(archivoSalida, "COMA\n");
                    break;
                case COLON:
                    fprintf(archivoSalida, "DOS_PUNTOS");
                    break;
                case TRUE:
                    fprintf(archivoSalida, "PR_TRUE");
                    break;
                case FALSE:
                    fprintf(archivoSalida, "PR_FALSE");
                    break;
                case NUL:
                    fprintf(archivoSalida, "PR_NULL");
                    break;
                default:
                    fprintf(archivoSalida, "TOKEN_DESCONOCIDO\n");
                    break;
            }

            tokenAnterior = t.compLex;  // Guardar el token actual para la siguiente vuelta
            getToken();  // Leer el siguiente token
        }

        fclose(archivo);
        fclose(archivoSalida);
    } else {
        printf("Debe pasar como parámetros el path al archivo fuente JSON y al archivo de salida.\n");
        exit(1);
    }
    return 0;
}
