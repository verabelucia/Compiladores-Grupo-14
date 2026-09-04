/* FGA0003 - Compiladores 1
   Curso de Engenharia de Software
   Universidade de Brasilia (UnB)
   ImagemLang - analisador lexico (Flex)

   Reconhece as palavras-chave, identificadores, literais e simbolos
   descritos em docs/tokens.md. Nao verifica a ordem dos tokens nem os
   tipos das variaveis: essas responsabilidades pertencem, respectivamente,
   as analises sintatica e semantica. */

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tokens.h"

/* Nome do arquivo lido, usado nas mensagens de erro. */
const char *arquivo_atual = "<stdin>";

/* Numero de erros lexicos encontrados. */
int erros_lexicos = 0;

/* Coluna do proximo caractere a ser lido e coluna onde o token atual
   comecou. O Flex controla a linha por conta propria (%option yylineno),
   mas nao controla a coluna. */
static int coluna = 1;
int coluna_token = 1;

/* Valores semanticos do ultimo token reconhecido. Serao substituidos por
   yylval quando o parser existir. */
long valor_inteiro = 0;
char *valor_texto = NULL;

/* Executado antes da acao de cada regra. */
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

"//"[^\n]*        { /* comentario de uma linha: ignorado */ }

[ \t\r]+          { /* separadores: ignorados */ }

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

{STRING}          { /* descarta as aspas de abertura e fechamento */
                    guardar_texto(yytext + 1, yyleng - 2);
                    return LITERAL_STRING; }

{STRING_ABERTA}   { erro_lexico("string nao terminada", yytext);
                    /* nao devolve token: o parser nao deve tentar usar
                       um literal que a linguagem nao reconhece */ }

"="               { return ATRIBUICAO; }
","               { return VIRGULA; }
";"               { return PONTO_E_VIRGULA; }

.                 { erro_lexico("caractere invalido", yytext); }

%%

/* Guarda o valor textual do token atual. A memoria anterior e liberada
   porque apenas o ultimo token reconhecido precisa estar disponivel. */
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

/* Formato adotado no projeto: arquivo:linha:coluna: categoria: mensagem */
static void erro_lexico(const char *mensagem, const char *lexema)
{
    erros_lexicos++;
    fprintf(stderr, "%s:%d:%d: erro lexico: %s: '%s'\n",
            arquivo_atual, yylineno, coluna_token, mensagem, lexema);
}
