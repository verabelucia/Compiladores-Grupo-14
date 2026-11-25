#ifndef IMAGEMLANG_TABELA_H
#define IMAGEMLANG_TABELA_H

#include <stdio.h>

#include "ast.h"

typedef enum {
    TIPO_IMAGEM,
    TIPO_INTEIRO
} TipoSimbolo;

typedef struct Simbolo {
    char *nome;
    TipoSimbolo tipo;
    long valor;
    Posicao pos;
    struct Simbolo *proximo;
} Simbolo;

typedef struct {
    Simbolo *primeiro;
    Simbolo *ultimo;
    int quantidade;
} TabelaSimbolos;

void tabela_iniciar(TabelaSimbolos *tabela);

int tabela_declarar(TabelaSimbolos *tabela, Identificador id,
                    TipoSimbolo tipo, long valor);

const Simbolo *tabela_usar(TabelaSimbolos *tabela, const char *nome,
                           Posicao pos);

const Simbolo *tabela_buscar(const TabelaSimbolos *tabela, const char *nome);

void tabela_imprimir(FILE *saida, const TabelaSimbolos *tabela);

void tabela_liberar(TabelaSimbolos *tabela);

#endif
