/* FGA0003 - Compiladores 1 */
/* Curso de Engenharia de Software */
/* Universidade de Brasília (UnB) */
/* ImagemLang - analisador léxico com Flex */

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.tab.h"

const char *arquivo_atual = "<stdin>";
int erros_lexicos = 0;

/* O Flex conta a linha sozinho (%option yylineno), mas nao conta a coluna.
   A posicao de cada token vai para yylloc, que o Bison usa em @1, @2... */
static int coluna = 1;

#define YY_USER_ACTION                   \
    yylloc.first_line = yylineno;        \
    yylloc.first_column = coluna;        \
    yylloc.last_line = yylineno;         \
    yylloc.last_column = coluna + yyleng - 1; \
    coluna += yyleng;

static void erro_lexico(const char *mensagem, const char *lexema);
static char *copiar_texto(const char *inicio, int tamanho);
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

{IDENT}           { yylval.texto = copiar_texto(yytext, yyleng);
                    return IDENTIFICADOR; }

{INTEIRO}         { yylval.inteiro = strtol(yytext, NULL, 10);
                    return LITERAL_INTEIRO; }

{STRING}          { /* sem as aspas */
                    yylval.texto = copiar_texto(yytext + 1, yyleng - 2);
                    return LITERAL_STRING; }

{STRING_ABERTA}   { erro_lexico("string nao terminada", yytext); }

"="               { return ATRIBUICAO; }
","               { return VIRGULA; }
";"               { return PONTO_E_VIRGULA; }

.                 { erro_lexico("caractere invalido", yytext); }

<<EOF>>           { /* O fim do arquivo nao passa por YY_USER_ACTION: sem
                       isto, um erro "encontrado fim do arquivo" citaria a
                       posicao do ultimo caractere lido. */
                    yylloc.first_line = yylloc.last_line = yylineno;
                    yylloc.first_column = yylloc.last_column = coluna;
                    return FIM; }

%%

/* Devolve uma copia propria do lexema; quem recebe o token libera. */
static char *copiar_texto(const char *inicio, int tamanho)
{
    char *texto = malloc((size_t) tamanho + 1);
    if (texto == NULL) {
        fprintf(stderr, "erro: memoria insuficiente no analisador lexico\n");
        exit(1);
    }
    memcpy(texto, inicio, (size_t) tamanho);
    texto[tamanho] = '\0';
    return texto;
}

static void erro_lexico(const char *mensagem, const char *lexema)
{
    erros_lexicos++;
    fprintf(stderr, "%s:%d:%d: erro lexico: %s: '%s'\n",
            arquivo_atual, yylloc.first_line, yylloc.first_column,
            mensagem, lexema);
}
