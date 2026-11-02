/* FGA0003 - Compiladores 1 */
/* Curso de Engenharia de Software */
/* Universidade de Brasília (UnB) */
/* ImagemLang - tabela de símbolos (dono: Pessoa 2) */

#include <stdlib.h>
#include <string.h>

#include "semantico.h"
#include "tabela.h"

static char *copiar_nome(const char *nome)
{
    size_t tamanho = strlen(nome) + 1;
    char *copia = malloc(tamanho);

    if (copia == NULL) {
        fprintf(stderr, "erro: memoria insuficiente na tabela de simbolos\n");
        exit(1);
    }
    memcpy(copia, nome, tamanho);
    return copia;
}

void tabela_iniciar(TabelaSimbolos *tabela)
{
    tabela->primeiro = NULL;
    tabela->ultimo = NULL;
    tabela->quantidade = 0;
}

int tabela_declarar(TabelaSimbolos *tabela, Identificador id,
                    TipoSimbolo tipo, long valor)
{
    const Simbolo *existente = tabela_buscar(tabela, id.nome);
    Simbolo *novo;

    if (existente != NULL) {
        erro_semantico(id.pos, "'%s' ja foi declarada na linha %d",
                       id.nome, existente->pos.linha);
        return 0;
    }

    novo = malloc(sizeof *novo);
    if (novo == NULL) {
        fprintf(stderr, "erro: memoria insuficiente na tabela de simbolos\n");
        exit(1);
    }
    novo->nome = copiar_nome(id.nome);
    novo->tipo = tipo;
    novo->valor = valor;
    novo->pos = id.pos;
    novo->proximo = NULL;

    if (tabela->ultimo == NULL)
        tabela->primeiro = novo;
    else
        tabela->ultimo->proximo = novo;
    tabela->ultimo = novo;
    tabela->quantidade++;
    return 1;
}

const Simbolo *tabela_usar(TabelaSimbolos *tabela, const char *nome,
                           Posicao pos)
{
    const Simbolo *simbolo = tabela_buscar(tabela, nome);

    if (simbolo == NULL)
        erro_semantico(pos, "'%s' nao foi declarada", nome);
    return simbolo;
}

const Simbolo *tabela_buscar(const TabelaSimbolos *tabela, const char *nome)
{
    const Simbolo *s;

    for (s = tabela->primeiro; s != NULL; s = s->proximo) {
        if (strcmp(s->nome, nome) == 0)
            return s;
    }
    return NULL;
}

void tabela_imprimir(FILE *saida, const TabelaSimbolos *tabela)
{
    const Simbolo *s;

    fprintf(saida, "TABELA DE SIMBOLOS (%d %s)\n", tabela->quantidade,
            tabela->quantidade == 1 ? "simbolo" : "simbolos");
    for (s = tabela->primeiro; s != NULL; s = s->proximo) {
        const char *ramo = s->proximo == NULL ? "`-- " : "|-- ";

        if (s->tipo == TIPO_IMAGEM)
            fprintf(saida, "%s%s: imagem [%d:%d]\n",
                    ramo, s->nome, s->pos.linha, s->pos.coluna);
        else
            fprintf(saida, "%s%s: inteiro = %ld [%d:%d]\n",
                    ramo, s->nome, s->valor, s->pos.linha, s->pos.coluna);
    }
}

void tabela_liberar(TabelaSimbolos *tabela)
{
    Simbolo *s, *seguinte;

    for (s = tabela->primeiro; s != NULL; s = seguinte) {
        seguinte = s->proximo;
        free(s->nome);
        free(s);
    }
    tabela_iniciar(tabela);
}
