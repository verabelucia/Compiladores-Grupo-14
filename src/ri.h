/* FGA0003 - Compiladores 1 */
/* Curso de Engenharia de Software */
/* Universidade de Brasília (UnB) */
/* ImagemLang - representação intermediária (dono: Pessoa 4) */

/* CONTRATO CONGELADO: ver tabela.h. Formato de texto exato do modo --ri
   e exemplo completo: docs/modulos.md. */

#ifndef IMAGEMLANG_RI_H
#define IMAGEMLANG_RI_H

#include <stdio.h>

#include "ast.h"

/* A RI é uma lista linear de instruções, uma por comando, mais um FREE
   para cada imagem no fim do programa (vira img_destroy no gerador de C).

   Variáveis inteiras continuam aparecendo pelo nome (RESIZE foto, largura,
   800). Trocar o nome pelo valor (propagação de constantes) é a otimização
   da semana 09, feita sobre esta mesma estrutura. */
typedef enum {
    RI_LOAD_IMAGE,      /* LOAD_IMAGE "foto.jpg" -> foto        */
    RI_CONST,           /* CONST 750 -> largura                 */
    RI_RESIZE,          /* RESIZE foto, largura, altura         */
    RI_GRAYSCALE,       /* GRAYSCALE foto                       */
    RI_ROTATE,          /* ROTATE foto, angulo                  */
    RI_CROP,            /* CROP foto, x, y, largura, altura     */
    RI_SAVE,            /* SAVE foto, "resultado.jpg"           */
    RI_FREE             /* FREE foto                            */
} OpRI;

typedef struct InstrucaoRI {
    OpRI op;
    char *alvo;                 /* imagem ou variável inteira (cópia própria) */
    char *arquivo;              /* RI_LOAD_IMAGE e RI_SAVE (cópia própria) */
    long numero;                /* RI_CONST */
    Valor args[AST_MAX_ARGS];   /* RESIZE, ROTATE, CROP: mesma ordem da AST;
                                   'nome' das variáveis é cópia própria */
    int num_args;
    Posicao pos;                /* comando de origem, para mensagens */
    struct InstrucaoRI *proximo;
} InstrucaoRI;

typedef struct {
    InstrucaoRI *primeira;
    InstrucaoRI *ultima;
    int quantidade;
} ProgramaRI;

/* Gera a RI a partir de uma AST já validada pela semântica. A RI copia as
   strings de que precisa: a AST pode ser liberada antes ou depois. */
ProgramaRI *gerar_ri(const NoAST *raiz);

/* Modo --ri. Formato exato em docs/modulos.md. */
void imprimir_ri(FILE *saida, const ProgramaRI *ri);

/* Libera a RI inteira. Aceita NULL. */
void liberar_ri(ProgramaRI *ri);

#endif
