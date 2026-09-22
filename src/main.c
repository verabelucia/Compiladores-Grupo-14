/* FGA0003 - Compiladores 1 */
/* Curso de Engenharia de Software */
/* Universidade de Brasília (UnB) */
/* ImagemLang - ponto de entrada do compilador */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.tab.h"

extern int yylex(void);
extern FILE *yyin;

extern const char *arquivo_atual;
extern int erros_lexicos;
extern int erros_sintaticos;

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

/* Modo --tokens: imprime a sequencia de tokens, sem analise sintatica. */
static int listar_tokens(void)
{
    int token;

    while ((token = yylex()) != FIM) {
        const char *nome = nome_token(token);

        if (token == IDENTIFICADOR || token == LITERAL_STRING) {
            printf("%3d:%-3d %-18s %s\n", yylloc.first_line,
                   yylloc.first_column, nome, yylval.texto);
            free(yylval.texto);
        } else if (token == LITERAL_INTEIRO) {
            printf("%3d:%-3d %-18s %ld\n", yylloc.first_line,
                   yylloc.first_column, nome, yylval.inteiro);
        } else {
            printf("%3d:%-3d %s\n", yylloc.first_line,
                   yylloc.first_column, nome);
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

/* Modo padrao: analise sintatica; cada comando reconhecido e impresso. */
static int analisar(void)
{
    int erros;

    yyparse();

    erros = erros_lexicos + erros_sintaticos;
    if (erros > 0) {
        fprintf(stderr, "\nanalise concluida com %d erro(s): "
                "%d lexico(s), %d sintatico(s).\n",
                erros, erros_lexicos, erros_sintaticos);
        return 1;
    }

    printf("\nanalise sintatica concluida sem erros.\n");
    return 0;
}

int main(int argc, char **argv)
{
    int so_tokens = 0;
    const char *caminho = NULL;

    /* stdout line-buffered: sem isso o stderr sai fora de ordem quando as
       duas saidas vao para o mesmo arquivo, como no run_tests.sh. */
    setvbuf(stdout, NULL, _IOLBF, 0);

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--tokens") == 0 && !so_tokens) {
            so_tokens = 1;
        } else if (caminho == NULL && argv[i][0] != '-') {
            caminho = argv[i];
        } else {
            fprintf(stderr, "uso: %s [--tokens] [arquivo.img]\n", argv[0]);
            return 2;
        }
    }

    if (caminho != NULL) {
        yyin = fopen(caminho, "r");
        if (yyin == NULL) {
            fprintf(stderr, "erro: nao foi possivel abrir '%s'\n", caminho);
            return 2;
        }
        arquivo_atual = caminho;
    }

    return so_tokens ? listar_tokens() : analisar();
}
