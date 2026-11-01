/* FGA0003 - Compiladores 1 */
/* Curso de Engenharia de Software */
/* Universidade de Brasília (UnB) */
/* ImagemLang - representação intermediária (dono: Pessoa 4) */

/* A FAZER (Pessoa 4): implementar conforme ri.h e docs/modulos.md. Os
   corpos abaixo são só um esqueleto para o projeto compilar. */

#include <stdlib.h>

#include "ri.h"

ProgramaRI *gerar_ri(const NoAST *raiz)
{
    /* A FAZER: uma instrução por comando de raiz->comandos, na ordem, e no
       fim um RI_FREE para cada imagem declarada, na ordem de declaração. */
    ProgramaRI *ri = calloc(1, sizeof *ri);
    if (ri == NULL) {
        fprintf(stderr, "erro: memoria insuficiente ao gerar a RI\n");
        exit(1);
    }
    (void) raiz;
    return ri;
}

void imprimir_ri(FILE *saida, const ProgramaRI *ri)
{
    /* A FAZER: formato em docs/modulos.md. */
    (void) saida;
    (void) ri;
}

void liberar_ri(ProgramaRI *ri)
{
    /* A FAZER: liberar cada instrução e as strings copiadas. */
    free(ri);
}
