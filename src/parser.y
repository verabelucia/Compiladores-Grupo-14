/* FGA0003 - Compiladores 1 */
/* Curso de Engenharia de Software */
/* Universidade de Brasília (UnB) */
/* ImagemLang - analisador sintático com Bison */

%require "3.6"

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int yylex(void);
void yyerror(const char *mensagem);

extern const char *arquivo_atual;
int erros_sintaticos = 0;

static char *texto_de_inteiro(long valor);
%}

%locations
%define parse.error custom

%union {
    long inteiro;
    char *texto;
}

/* Os apelidos entre aspas aparecem nas mensagens de erro sintático. */
%token FIM 0                    "fim do arquivo"
%token KW_IMAGEM                "'imagem'"
%token KW_INTEIRO               "'inteiro'"
%token KW_REDIMENSIONAR         "'redimensionar'"
%token KW_PARA                  "'para'"
%token KW_POR                   "'por'"
%token KW_TONS_DE_CINZA         "'tons_de_cinza'"
%token KW_ROTACIONAR            "'rotacionar'"
%token KW_RECORTAR              "'recortar'"
%token KW_DE                    "'de'"
%token KW_TAMANHO               "'tamanho'"
%token KW_SALVAR                "'salvar'"
%token KW_COMO                  "'como'"
%token <texto>   IDENTIFICADOR   "identificador"
%token <inteiro> LITERAL_INTEIRO "numero inteiro"
%token <texto>   LITERAL_STRING  "texto entre aspas"
%token ATRIBUICAO               "'='"
%token VIRGULA                  "','"
%token PONTO_E_VIRGULA          "';'"

%type <texto> valor_inteiro

/* Libera strings descartadas durante a recuperação de erros. */
%destructor { free($$); } <texto>

%%

programa
    : %empty
    | programa comando
    ;

comando
    : declaracao_imagem
    | declaracao_inteiro
    | redimensionamento
    | conversao_cinza
    | rotacao
    | recorte
    | salvamento
    | error PONTO_E_VIRGULA   { yyerrok; }
    ;

declaracao_imagem
    : KW_IMAGEM IDENTIFICADOR ATRIBUICAO LITERAL_STRING PONTO_E_VIRGULA
        { printf("%3d:%-3d imagem %s = \"%s\"\n",
                 @1.first_line, @1.first_column, $2, $4);
          free($2); free($4); }
    ;

declaracao_inteiro
    : KW_INTEIRO IDENTIFICADOR ATRIBUICAO LITERAL_INTEIRO PONTO_E_VIRGULA
        { printf("%3d:%-3d inteiro %s = %ld\n",
                 @1.first_line, @1.first_column, $2, $4);
          free($2); }
    ;

redimensionamento
    : KW_REDIMENSIONAR IDENTIFICADOR KW_PARA valor_inteiro KW_POR valor_inteiro PONTO_E_VIRGULA
        { printf("%3d:%-3d redimensionar %s para %s por %s\n",
                 @1.first_line, @1.first_column, $2, $4, $6);
          free($2); free($4); free($6); }
    ;

conversao_cinza
    : KW_TONS_DE_CINZA IDENTIFICADOR PONTO_E_VIRGULA
        { printf("%3d:%-3d tons_de_cinza %s\n",
                 @1.first_line, @1.first_column, $2);
          free($2); }
    ;

rotacao
    : KW_ROTACIONAR IDENTIFICADOR valor_inteiro PONTO_E_VIRGULA
        { printf("%3d:%-3d rotacionar %s %s\n",
                 @1.first_line, @1.first_column, $2, $3);
          free($2); free($3); }
    ;

recorte
    : KW_RECORTAR IDENTIFICADOR KW_DE valor_inteiro VIRGULA valor_inteiro
      KW_TAMANHO valor_inteiro KW_POR valor_inteiro PONTO_E_VIRGULA
        { printf("%3d:%-3d recortar %s de %s, %s tamanho %s por %s\n",
                 @1.first_line, @1.first_column, $2, $4, $6, $8, $10);
          free($2); free($4); free($6); free($8); free($10); }
    ;

salvamento
    : KW_SALVAR IDENTIFICADOR KW_COMO LITERAL_STRING PONTO_E_VIRGULA
        { printf("%3d:%-3d salvar %s como \"%s\"\n",
                 @1.first_line, @1.first_column, $2, $4);
          free($2); free($4); }
    ;

valor_inteiro
    : LITERAL_INTEIRO   { $$ = texto_de_inteiro($1); }
    | IDENTIFICADOR     { $$ = $1; }
    ;

%%

static char *texto_de_inteiro(long valor)
{
    char *texto = malloc(24);
    if (texto == NULL) {
        fprintf(stderr, "erro: memoria insuficiente no analisador sintatico\n");
        exit(1);
    }
    snprintf(texto, 24, "%ld", valor);
    return texto;
}

/* Chamada pelo Bison (parse.error custom) a cada erro sintático. Monta a
   mensagem no formato arquivo:linha:coluna: erro sintatico: ... */
static int yyreport_syntax_error(const yypcontext_t *contexto)
{
    enum { MAX_ESPERADOS = 5 };
    yysymbol_kind_t esperados[MAX_ESPERADOS];
    const YYLTYPE *local = yypcontext_location(contexto);
    int n = yypcontext_expected_tokens(contexto, esperados, MAX_ESPERADOS);

    erros_sintaticos++;
    fprintf(stderr, "%s:%d:%d: erro sintatico: encontrado %s",
            arquivo_atual, local->first_line, local->first_column,
            yysymbol_name(yypcontext_token(contexto)));

    /* n == 0 quando ha mais de MAX_ESPERADOS alternativas. Nesta gramatica
       isso so acontece onde comeca um comando (sao 7 palavras-chave). */
    if (n == 0)
        fprintf(stderr, ", esperado inicio de comando");
    for (int i = 0; i < n; i++)
        fprintf(stderr, "%s %s", i == 0 ? ", esperado" : " ou",
                yysymbol_name(esperados[i]));
    fprintf(stderr, "\n");
    return 0;
}

/* Erros que nao sao sintaticos (ex.: pilha esgotada) passam por aqui. */
void yyerror(const char *mensagem)
{
    fprintf(stderr, "%s: erro: %s\n", arquivo_atual, mensagem);
}
