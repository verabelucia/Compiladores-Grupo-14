#ifndef IMAGEMLANG_SEMANTICO_H
#define IMAGEMLANG_SEMANTICO_H

#include "ast.h"
#include "tabela.h"

extern int erros_semanticos;

void erro_semantico(Posicao pos, const char *formato, ...);

int analisar_semantica(const NoAST *raiz, TabelaSimbolos *tabela);

#endif
