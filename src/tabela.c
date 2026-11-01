/* FGA0003 - Compiladores 1 */
/* Curso de Engenharia de Software */
/* Universidade de Brasília (UnB) */
/* ImagemLang - tabela de símbolos (dono: Pessoa 2) */

/* A FAZER (Pessoa 2): implementar as funções conforme tabela.h e
   docs/modulos.md. Os corpos abaixo são só um esqueleto para o projeto
   compilar enquanto o módulo não existe. */

#include <stdlib.h>
#include <string.h>

#include "semantico.h"
#include "tabela.h"

void tabela_iniciar(TabelaSimbolos *tabela)
{
    tabela->primeiro = NULL;
    tabela->ultimo = NULL;
    tabela->quantidade = 0;
}

int tabela_declarar(TabelaSimbolos *tabela, Identificador id,
                    TipoSimbolo tipo, long valor)
{
    /* A FAZER: se tabela_buscar encontrar id.nome, relatar a redeclaração
       com erro_semantico e devolver 0; senão, inserir no fim da lista. */
    (void) tabela;
    (void) id;
    (void) tipo;
    (void) valor;
    return 1;
}

const Simbolo *tabela_usar(TabelaSimbolos *tabela, const char *nome,
                           Posicao pos)
{
    /* A FAZER: buscar; se não achar, relatar com erro_semantico. */
    (void) pos;
    return tabela_buscar(tabela, nome);
}

const Simbolo *tabela_buscar(const TabelaSimbolos *tabela, const char *nome)
{
    /* A FAZER: percorrer a lista comparando com strcmp. */
    (void) tabela;
    (void) nome;
    return NULL;
}

void tabela_imprimir(FILE *saida, const TabelaSimbolos *tabela)
{
    /* A FAZER: formato em docs/modulos.md. */
    (void) saida;
    (void) tabela;
}

void tabela_liberar(TabelaSimbolos *tabela)
{
    /* A FAZER: liberar cada símbolo (e o nome copiado). */
    tabela_iniciar(tabela);
}
