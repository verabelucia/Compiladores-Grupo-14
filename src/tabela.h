/* FGA0003 - Compiladores 1 */
/* Curso de Engenharia de Software */
/* Universidade de Brasília (UnB) */
/* ImagemLang - tabela de símbolos (dono: Pessoa 2) */

/* CONTRATO CONGELADO: as assinaturas abaixo foram combinadas pelo grupo.
   Mudar alguma delas exige combinar antes com quem a usa (semantico.c,
   main.c). Detalhes e mensagens de erro: docs/modulos.md. */

#ifndef IMAGEMLANG_TABELA_H
#define IMAGEMLANG_TABELA_H

#include <stdio.h>

#include "ast.h"

/* A ImagemLang não tem funções nem blocos: há um único escopo, global.
   Uma lista encadeada, na ordem das declarações, é suficiente. */

typedef enum {
    TIPO_IMAGEM,
    TIPO_INTEIRO
} TipoSimbolo;

typedef struct Simbolo {
    char *nome;             /* cópia própria (strdup); liberada pela tabela */
    TipoSimbolo tipo;
    long valor;             /* TIPO_INTEIRO: valor da declaração. Como não
                               existe atribuição, todo inteiro é constante. */
    Posicao pos;            /* onde o nome aparece na declaração */
    struct Simbolo *proximo;
} Simbolo;

typedef struct {
    Simbolo *primeiro;
    Simbolo *ultimo;
    int quantidade;
} TabelaSimbolos;

/* Deixa a tabela vazia. Chamada uma vez, antes da análise semântica. */
void tabela_iniciar(TabelaSimbolos *tabela);

/* Registra uma declaração (valor só importa para TIPO_INTEIRO).
   Se o nome já existe, relata, via erro_semantico, na posição id.pos:
       'NOME' ja foi declarada na linha N
   e devolve 0 sem alterar a tabela. Caso contrário, devolve 1. */
int tabela_declarar(TabelaSimbolos *tabela, Identificador id,
                    TipoSimbolo tipo, long valor);

/* Procura um nome usado num comando. Se não existe, relata, via
   erro_semantico, na posição pos:
       'NOME' nao foi declarada
   e devolve NULL. */
const Simbolo *tabela_usar(TabelaSimbolos *tabela, const char *nome,
                           Posicao pos);

/* Procura sem relatar erro (para quem só quer consultar). */
const Simbolo *tabela_buscar(const TabelaSimbolos *tabela, const char *nome);

/* Modo --tabela. Formato exato em docs/modulos.md. */
void tabela_imprimir(FILE *saida, const TabelaSimbolos *tabela);

/* Libera todos os símbolos e deixa a tabela vazia. */
void tabela_liberar(TabelaSimbolos *tabela);

#endif
