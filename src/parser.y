%require "3.6"

%code requires {
#include "ast.h"
}

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"

int yylex(void);
void yyerror(const char *mensagem);

extern const char *arquivo_atual;
int erros_sintaticos = 0;
NoAST *raiz_ast = NULL;

#define POS(local) posicao((local).first_line, (local).first_column)
%}

%locations
%define parse.error custom

%union {
    long inteiro;
    char *texto;
    NoAST *no;
    Valor valor;
}

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

%type <no> programa comando declaracao_imagem declaracao_inteiro
%type <no> redimensionamento conversao_cinza rotacao recorte salvamento
%type <valor> valor_inteiro

%destructor { free($$); } <texto>
%destructor { liberar_ast($$); } <no>
%destructor { liberar_valor(&$$); } <valor>

%start inicio

%%

inicio
    : programa                  { raiz_ast = $1; }
    ;

programa
    : %empty                    { $$ = criar_no_programa(); }
    | programa comando          { adicionar_comando($1, $2); $$ = $1; }
    ;

comando
    : declaracao_imagem
    | declaracao_inteiro
    | redimensionamento
    | conversao_cinza
    | rotacao
    | recorte
    | salvamento
    | error PONTO_E_VIRGULA     { yyerrok; $$ = NULL; }
    ;

declaracao_imagem
    : KW_IMAGEM IDENTIFICADOR ATRIBUICAO LITERAL_STRING PONTO_E_VIRGULA
        { $$ = criar_no_decl_imagem(POS(@1), identificador($2, POS(@2)), $4); }
    ;

declaracao_inteiro
    : KW_INTEIRO IDENTIFICADOR ATRIBUICAO LITERAL_INTEIRO PONTO_E_VIRGULA
        { $$ = criar_no_decl_inteiro(POS(@1), identificador($2, POS(@2)), $4); }
    ;

redimensionamento
    : KW_REDIMENSIONAR IDENTIFICADOR KW_PARA valor_inteiro KW_POR valor_inteiro PONTO_E_VIRGULA
        { $$ = criar_no_redimensionar(POS(@1), identificador($2, POS(@2)), $4, $6); }
    ;

conversao_cinza
    : KW_TONS_DE_CINZA IDENTIFICADOR PONTO_E_VIRGULA
        { $$ = criar_no_tons_de_cinza(POS(@1), identificador($2, POS(@2))); }
    ;

rotacao
    : KW_ROTACIONAR IDENTIFICADOR valor_inteiro PONTO_E_VIRGULA
        { $$ = criar_no_rotacionar(POS(@1), identificador($2, POS(@2)), $3); }
    ;

recorte
    : KW_RECORTAR IDENTIFICADOR KW_DE valor_inteiro VIRGULA valor_inteiro
      KW_TAMANHO valor_inteiro KW_POR valor_inteiro PONTO_E_VIRGULA
        { $$ = criar_no_recortar(POS(@1), identificador($2, POS(@2)),
                                 $4, $6, $8, $10); }
    ;

salvamento
    : KW_SALVAR IDENTIFICADOR KW_COMO LITERAL_STRING PONTO_E_VIRGULA
        { $$ = criar_no_salvar(POS(@1), identificador($2, POS(@2)), $4); }
    ;

valor_inteiro
    : LITERAL_INTEIRO           { $$ = valor_literal($1, POS(@1)); }
    | IDENTIFICADOR             { $$ = valor_variavel($1, POS(@1)); }
    ;

%%

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

    if (n == 0)
        fprintf(stderr, ", esperado inicio de comando");
    for (int i = 0; i < n; i++)
        fprintf(stderr, "%s %s", i == 0 ? ", esperado" : " ou",
                yysymbol_name(esperados[i]));
    fprintf(stderr, "\n");
    return 0;
}

void yyerror(const char *mensagem)
{
    fprintf(stderr, "%s: erro: %s\n", arquivo_atual, mensagem);
}
