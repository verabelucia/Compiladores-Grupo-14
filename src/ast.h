#ifndef IMAGEMLANG_AST_H
#define IMAGEMLANG_AST_H

#include <stdio.h>

typedef struct {
    int linha;
    int coluna;
} Posicao;

typedef struct {
    char *nome;
    Posicao pos;
} Identificador;

typedef enum {
    VALOR_LITERAL,
    VALOR_VARIAVEL
} TipoValor;

typedef struct {
    TipoValor tipo;
    long numero;
    char *nome;
    Posicao pos;
} Valor;

typedef enum {
    AST_PROGRAM,
    AST_IMAGE_DECL,
    AST_INTEGER_DECL,
    AST_RESIZE,
    AST_GRAYSCALE,
    AST_ROTATE,
    AST_CROP,
    AST_SAVE
} TipoNo;

enum { RESIZE_LARGURA, RESIZE_ALTURA };
enum { ROTATE_ANGULO };
enum { CROP_X, CROP_Y, CROP_LARGURA, CROP_ALTURA };

#define AST_MAX_ARGS 4

typedef struct NoAST {
    TipoNo tipo;
    Posicao pos;

    Identificador alvo;
    char *arquivo;
    long numero;
    Valor args[AST_MAX_ARGS];
    int num_args;

    int num_comandos;
    struct NoAST *comandos;
    struct NoAST *ultimo;
    struct NoAST *proximo;
} NoAST;

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

const char *nome_tipo_no(TipoNo tipo);

const char *palavra_chave_no(TipoNo tipo);

const char *rotulo_arg(TipoNo tipo, int i);

void imprimir_ast(FILE *saida, const NoAST *raiz);

extern NoAST *raiz_ast;

void liberar_ast(NoAST *raiz);
void liberar_valor(Valor *valor);

#endif
