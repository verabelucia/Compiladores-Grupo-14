#ifndef IMAGEMLANG_RI_H
#define IMAGEMLANG_RI_H

#include <stdio.h>

#include "ast.h"

typedef enum {
    RI_LOAD_IMAGE,
    RI_CONST,
    RI_RESIZE,
    RI_GRAYSCALE,
    RI_ROTATE,
    RI_CROP,
    RI_SAVE,
    RI_FREE
} OpRI;

typedef struct InstrucaoRI {
    OpRI op;
    char *alvo;
    char *arquivo;
    long numero;
    Valor args[AST_MAX_ARGS];
    int num_args;
    Posicao pos;
    struct InstrucaoRI *proximo;
} InstrucaoRI;

typedef struct {
    InstrucaoRI *primeira;
    InstrucaoRI *ultima;
    int quantidade;
} ProgramaRI;

ProgramaRI *gerar_ri(const NoAST *raiz);

void imprimir_ri(FILE *saida, const ProgramaRI *ri);

void liberar_ri(ProgramaRI *ri);

#endif
