/* FGA0003 - Compiladores 1 */
/* Curso de Engenharia de Software */
/* Universidade de Brasília (UnB) */
/* ImagemLang - análise semântica (dono: Pessoa 3) */

/* CONTRATO CONGELADO: ver tabela.h. Lista completa das verificações e o
   texto exato de cada mensagem: docs/modulos.md. */

#ifndef IMAGEMLANG_SEMANTICO_H
#define IMAGEMLANG_SEMANTICO_H

#include "ast.h"
#include "tabela.h"

/* Número de erros semânticos relatados até agora. */
extern int erros_semanticos;

/* Único jeito de relatar um erro semântico, usado pela semântica e pela
   tabela. Imprime em stderr, no formato do projeto,
       arquivo:LINHA:COLUNA: erro semantico: <mensagem>
   e conta o erro. 'formato' funciona como no printf. */
void erro_semantico(Posicao pos, const char *formato, ...);

/* Percorre os comandos de 'raiz' na ordem do programa, preenchendo
   'tabela' e relatando cada problema encontrado, sem parar no primeiro.
   Só é chamada quando não houve erro léxico nem sintático.
   Devolve o número de erros semânticos. */
int analisar_semantica(const NoAST *raiz, TabelaSimbolos *tabela);

#endif
