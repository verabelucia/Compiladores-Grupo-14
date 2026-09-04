/* FGA0003 - Compiladores 1 */
/* Curso de Engenharia de Software */
/* Universidade de Brasília (UnB) */
/* ImagemLang - analisador léxico com Flex */

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tokens.h"

const char *arquivo_atual = "<stdin>";
int erros_lexicos = 0;

/* O Flex conta a linha sozinho (%option yylineno), mas nao conta a coluna. */
static int coluna = 1;
int coluna_token = 1;

long valor_inteiro = 0;
char *valor_texto = NULL;

#define YY_USER_ACTION           \
    coluna_token = coluna;       \
    coluna += yyleng;

static void erro_lexico(const char *mensagem, const char *lexema);
static void guardar_texto(const char *inicio, int tamanho);
%}

%option noyywrap
%option nounput
%option noinput
%option yylineno

LETRA        [A-Za-z_]
DIGITO       [0-9]
IDENT        {LETRA}({LETRA}|{DIGITO})*
INTEIRO      {DIGITO}+
STRING       \"[^\"\n]*\"
STRING_ABERTA \"[^\"\n]*

%%

"//"[^\n]*        { }

[ \t\r]+          { }

\n                { coluna = 1; }

"imagem"          { return KW_IMAGEM; }
"inteiro"         { return KW_INTEIRO; }
"redimensionar"   { return KW_REDIMENSIONAR; }
"para"            { return KW_PARA; }
"por"             { return KW_POR; }
"tons_de_cinza"   { return KW_TONS_DE_CINZA; }
"rotacionar"      { return KW_ROTACIONAR; }
"recortar"        { return KW_RECORTAR; }
"de"              { return KW_DE; }
"tamanho"         { return KW_TAMANHO; }
"salvar"          { return KW_SALVAR; }
"como"            { return KW_COMO; }

{IDENT}           { guardar_texto(yytext, yyleng);
                    return IDENTIFICADOR; }

{INTEIRO}         { valor_inteiro = strtol(yytext, NULL, 10);
                    return LITERAL_INTEIRO; }

{STRING}          { guardar_texto(yytext + 1, yyleng - 2);  /* sem as aspas */
                    return LITERAL_STRING; }

{STRING_ABERTA}   { erro_lexico("string nao terminada", yytext); }

"="               { return ATRIBUICAO; }
","               { return VIRGULA; }
";"               { return PONTO_E_VIRGULA; }

.                 { erro_lexico("caractere invalido", yytext); }

%%

static void guardar_texto(const char *inicio, int tamanho)
{
    free(valor_texto);
    valor_texto = malloc((size_t) tamanho + 1);
    if (valor_texto == NULL) {
        fprintf(stderr, "erro: memoria insuficiente no analisador lexico\n");
        exit(1);
    }
    memcpy(valor_texto, inicio, (size_t) tamanho);
    valor_texto[tamanho] = '\0';
}

static void erro_lexico(const char *mensagem, const char *lexema)
{
    erros_lexicos++;
    fprintf(stderr, "%s:%d:%d: erro lexico: %s: '%s'\n",
            arquivo_atual, yylineno, coluna_token, mensagem, lexema);
}
