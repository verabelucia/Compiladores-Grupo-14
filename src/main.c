/* FGA0003 - Compiladores 1 */
/* Curso de Engenharia de Software */
/* Universidade de Brasília (UnB) */
/* ImagemLang - ponto de entrada do compilador */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.tab.h"
#include "ri.h"
#include "semantico.h"
#include "tabela.h"

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

/* O que imprimir quando o programa nao tem erros. */
typedef enum {
    MODO_ANALISE,       /* so o resumo */
    MODO_TOKENS,        /* --tokens: sequencia de tokens (so o lexico) */
    MODO_AST,           /* --ast: arvore sintatica abstrata */
    MODO_TABELA,        /* --tabela: tabela de simbolos */
    MODO_RI             /* --ri: representacao intermediaria */
} Modo;

/* Analise completa: lexico + sintatico (yyparse) e, se nao houve erro,
   semantica. Com qualquer erro, so as mensagens e o resumo sao impressos. */
static int analisar(Modo modo)
{
    TabelaSimbolos tabela;
    ProgramaRI *ri;
    int erros;

    tabela_iniciar(&tabela);

    yyparse();

    /* A semantica precisa de uma arvore completa e confiavel. */
    if (erros_lexicos + erros_sintaticos == 0)
        analisar_semantica(raiz_ast, &tabela);

    erros = erros_lexicos + erros_sintaticos + erros_semanticos;
    if (erros > 0) {
        tabela_liberar(&tabela);
        liberar_ast(raiz_ast);
        fprintf(stderr, "\nanalise concluida com %d erro(s): "
                "%d lexico(s), %d sintatico(s), %d semantico(s).\n",
                erros, erros_lexicos, erros_sintaticos, erros_semanticos);
        return 1;
    }

    switch (modo) {
    case MODO_AST:
        imprimir_ast(stdout, raiz_ast);
        printf("\n");
        break;
    case MODO_TABELA:
        tabela_imprimir(stdout, &tabela);
        printf("\n");
        break;
    case MODO_RI:
        ri = gerar_ri(raiz_ast);
        imprimir_ri(stdout, ri);
        printf("\n");
        liberar_ri(ri);
        break;
    default:
        break;
    }

    tabela_liberar(&tabela);
    liberar_ast(raiz_ast);

    printf("analise concluida sem erros.\n");
    return 0;
}

static const struct {
    const char *opcao;
    Modo modo;
} OPCOES[] = {
    { "--tokens", MODO_TOKENS },
    { "--ast",    MODO_AST },
    { "--tabela", MODO_TABELA },
    { "--ri",     MODO_RI },
};

int main(int argc, char **argv)
{
    Modo modo = MODO_ANALISE;
    const char *caminho = NULL;

    /* stdout line-buffered: sem isso o stderr sai fora de ordem quando as
       duas saidas vao para o mesmo arquivo, como no run_tests.sh. */
    setvbuf(stdout, NULL, _IOLBF, 0);

    for (int i = 1; i < argc; i++) {
        int reconhecida = 0;

        for (size_t k = 0; k < sizeof OPCOES / sizeof OPCOES[0]; k++) {
            if (strcmp(argv[i], OPCOES[k].opcao) == 0 && modo == MODO_ANALISE) {
                modo = OPCOES[k].modo;
                reconhecida = 1;
            }
        }
        if (reconhecida)
            continue;
        if (caminho == NULL && argv[i][0] != '-') {
            caminho = argv[i];
        } else {
            fprintf(stderr, "uso: %s [--tokens | --ast | --tabela | --ri] "
                    "[arquivo.img]\n", argv[0]);
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

    if (modo == MODO_TOKENS)
        return listar_tokens();
    return analisar(modo);
}
