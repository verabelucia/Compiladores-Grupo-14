/* FGA0003 - Compiladores 1 */
/* Curso de Engenharia de Software */
/* Universidade de Brasília (UnB) */
/* ImagemLang - árvore sintática abstrata (AST) */

#ifndef IMAGEMLANG_AST_H
#define IMAGEMLANG_AST_H

#include <stdio.h>

/* Posição no código-fonte, usada nas mensagens de erro das fases seguintes. */
typedef struct {
    int linha;
    int coluna;
} Posicao;

/* Nome de variável com a posição onde aparece no programa. */
typedef struct {
    char *nome;
    Posicao pos;
} Identificador;

/* Argumento inteiro de um comando: um número escrito no programa (90) ou o
   nome de uma variável inteira (angulo). Corresponde a valor_inteiro na
   gramática. Saber se a variável existe e é inteira é papel da semântica. */
typedef enum {
    VALOR_LITERAL,
    VALOR_VARIAVEL
} TipoValor;

typedef struct {
    TipoValor tipo;
    long numero;        /* VALOR_LITERAL */
    char *nome;         /* VALOR_VARIAVEL */
    Posicao pos;
} Valor;

/* Tipos de nó, um por comando da linguagem (docs/arquitetura.md). */
typedef enum {
    AST_PROGRAM,
    AST_IMAGE_DECL,     /* imagem foto = "foto.jpg";                    */
    AST_INTEGER_DECL,   /* inteiro angulo = 90;                         */
    AST_RESIZE,         /* redimensionar foto para L por A;             */
    AST_GRAYSCALE,      /* tons_de_cinza foto;                          */
    AST_ROTATE,         /* rotacionar foto ANGULO;                      */
    AST_CROP,           /* recortar foto de X, Y tamanho L por A;       */
    AST_SAVE            /* salvar foto como "resultado.jpg";            */
} TipoNo;

/* Posição de cada argumento em NoAST.args, por tipo de comando. */
enum { RESIZE_LARGURA, RESIZE_ALTURA };
enum { ROTATE_ANGULO };
enum { CROP_X, CROP_Y, CROP_LARGURA, CROP_ALTURA };

#define AST_MAX_ARGS 4

/* O programa é um nó AST_PROGRAM com a lista encadeada dos comandos em
   'comandos'; cada comando aponta para o seguinte em 'proximo'. */
typedef struct NoAST {
    TipoNo tipo;
    Posicao pos;                /* palavra-chave que inicia o comando */

    Identificador alvo;         /* variável declarada ou operada pelo comando */
    char *arquivo;              /* AST_IMAGE_DECL e AST_SAVE */
    long numero;                /* AST_INTEGER_DECL */
    Valor args[AST_MAX_ARGS];   /* AST_RESIZE, AST_ROTATE e AST_CROP */
    int num_args;

    int num_comandos;           /* AST_PROGRAM */
    struct NoAST *comandos;     /* AST_PROGRAM: primeiro comando */
    struct NoAST *ultimo;       /* AST_PROGRAM: último comando */
    struct NoAST *proximo;      /* comando seguinte no programa */
} NoAST;

/* Construtores. Os nós passam a ser donos das strings recebidas (nomes e
   caminhos de arquivo alocados pelo lexer) e as liberam em liberar_ast. */
Posicao posicao(int linha, int coluna);
Identificador identificador(char *nome, Posicao pos);
Valor valor_literal(long numero, Posicao pos);
Valor valor_variavel(char *nome, Posicao pos);

NoAST *criar_no_programa(void);
void adicionar_comando(NoAST *programa, NoAST *comando);

NoAST *criar_no_decl_imagem(Posicao pos, Identificador nome, char *arquivo);
NoAST *criar_no_decl_inteiro(Posicao pos, Identificador nome, long numero);
NoAST *criar_no_redimensionar(Posicao pos, Identificador imagem,
                              Valor largura, Valor altura);
NoAST *criar_no_tons_de_cinza(Posicao pos, Identificador imagem);
NoAST *criar_no_rotacionar(Posicao pos, Identificador imagem, Valor angulo);
NoAST *criar_no_recortar(Posicao pos, Identificador imagem, Valor x, Valor y,
                         Valor largura, Valor altura);
NoAST *criar_no_salvar(Posicao pos, Identificador imagem, char *arquivo);

/* Nome do tipo de nó ("RESIZE", ...), usado na impressão. */
const char *nome_tipo_no(TipoNo tipo);

/* Palavra-chave que inicia o comando ("redimensionar", "tons_de_cinza"...),
   para mensagens de erro. */
const char *palavra_chave_no(TipoNo tipo);

/* Nome do i-ésimo argumento do comando ("largura", "angulo", "x"...), na
   ordem de NoAST.args. Devolve NULL se o comando não tem esse argumento. */
const char *rotulo_arg(TipoNo tipo, int i);

/* Imprime a árvore em formato texto (modo --ast). */
void imprimir_ast(FILE *saida, const NoAST *raiz);

/* Raiz do programa construída por yyparse() (definida em parser.y). Fica
   NULL se a análise for abortada; com erros recuperados, contém só os
   comandos válidos. */
extern NoAST *raiz_ast;

/* Libera a árvore inteira (ou um comando avulso), inclusive as strings.
   Aceita NULL. */
void liberar_ast(NoAST *raiz);
void liberar_valor(Valor *valor);

#endif
