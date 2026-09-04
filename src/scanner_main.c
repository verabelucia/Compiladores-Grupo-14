#include <stdio.h>
#include <stdlib.h>

#include "tokens.h"

extern int yylex(void);
extern int yylineno;
extern FILE *yyin;

extern const char *arquivo_atual;
extern int erros_lexicos;
extern int coluna_token;
extern long valor_inteiro;
extern char *valor_texto;

static const char *nome_token(int token)
{
    switch (token) {
    case KW_IMAGEM:         return "KW_IMAGEM";
    case KW_INTEIRO:        return "KW_INTEIRO";
    case KW_REDIMENSIONAR:  return "KW_REDIMENSIONAR";
    case KW_PARA:           return "KW_PARA";
    case KW_POR:            return "KW_POR";
    case KW_TONS_DE_CINZA:  return "KW_TONS_DE_CINZA";
    case KW_ROTACIONAR:     return "KW_ROTACIONAR";
    case KW_RECORTAR:       return "KW_RECORTAR";
    case KW_DE:             return "KW_DE";
    case KW_TAMANHO:        return "KW_TAMANHO";
    case KW_SALVAR:         return "KW_SALVAR";
    case KW_COMO:           return "KW_COMO";
    case IDENTIFICADOR:     return "IDENTIFICADOR";
    case LITERAL_INTEIRO:   return "LITERAL_INTEIRO";
    case LITERAL_STRING:    return "LITERAL_STRING";
    case ATRIBUICAO:        return "ATRIBUICAO";
    case VIRGULA:           return "VIRGULA";
    case PONTO_E_VIRGULA:   return "PONTO_E_VIRGULA";
    default:                return "TOKEN_DESCONHECIDO";
    }
}

int main(int argc, char **argv)
{
    int token;

    setvbuf(stdout, NULL, _IOLBF, 0);

    if (argc > 2) {
        fprintf(stderr, "uso: %s [arquivo.img]\n", argv[0]);
        return 2;
    }

    if (argc == 2) {
        yyin = fopen(argv[1], "r");
        if (yyin == NULL) {
            fprintf(stderr, "erro: nao foi possivel abrir '%s'\n", argv[1]);
            return 2;
        }
        arquivo_atual = argv[1];
    }

    while ((token = yylex()) != 0) {
        const char *nome = nome_token(token);

        if (token == IDENTIFICADOR || token == LITERAL_STRING) {
            printf("%3d:%-3d %-18s %s\n",
                   yylineno, coluna_token, nome, valor_texto);
        } else if (token == LITERAL_INTEIRO) {
            printf("%3d:%-3d %-18s %ld\n",
                   yylineno, coluna_token, nome, valor_inteiro);
        } else {
            printf("%3d:%-3d %s\n", yylineno, coluna_token, nome);
        }
    }

    if (erros_lexicos > 0) {
        fprintf(stderr, "\nanalise lexica concluida com %d erro(s).\n",
                erros_lexicos);
        return 1;
    }

    printf("\nanalise lexica concluida sem erros.\n");
    return 0;
}
