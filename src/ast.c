/* FGA0003 - Compiladores 1 */
/* Curso de Engenharia de Software */
/* Universidade de Brasília (UnB) */
/* ImagemLang - árvore sintática abstrata (AST) */

#include <stdlib.h>

#include "ast.h"

static NoAST *novo_no(TipoNo tipo, Posicao pos)
{
    NoAST *no = calloc(1, sizeof *no);
    if (no == NULL) {
        fprintf(stderr, "erro: memoria insuficiente ao construir a AST\n");
        exit(1);
    }
    no->tipo = tipo;
    no->pos = pos;
    return no;
}

Posicao posicao(int linha, int coluna)
{
    Posicao pos = { linha, coluna };
    return pos;
}

Identificador identificador(char *nome, Posicao pos)
{
    Identificador id = { nome, pos };
    return id;
}

Valor valor_literal(long numero, Posicao pos)
{
    Valor valor = { VALOR_LITERAL, numero, NULL, pos };
    return valor;
}

Valor valor_variavel(char *nome, Posicao pos)
{
    Valor valor = { VALOR_VARIAVEL, 0, nome, pos };
    return valor;
}

NoAST *criar_no_programa(void)
{
    return novo_no(AST_PROGRAM, posicao(1, 1));
}

/* Comandos com erro sintático chegam como NULL e são ignorados. */
void adicionar_comando(NoAST *programa, NoAST *comando)
{
    if (comando == NULL)
        return;
    if (programa->ultimo == NULL)
        programa->comandos = comando;
    else
        programa->ultimo->proximo = comando;
    programa->ultimo = comando;
    programa->num_comandos++;
}

NoAST *criar_no_decl_imagem(Posicao pos, Identificador nome, char *arquivo)
{
    NoAST *no = novo_no(AST_IMAGE_DECL, pos);
    no->alvo = nome;
    no->arquivo = arquivo;
    return no;
}

NoAST *criar_no_decl_inteiro(Posicao pos, Identificador nome, long numero)
{
    NoAST *no = novo_no(AST_INTEGER_DECL, pos);
    no->alvo = nome;
    no->numero = numero;
    return no;
}

NoAST *criar_no_redimensionar(Posicao pos, Identificador imagem,
                              Valor largura, Valor altura)
{
    NoAST *no = novo_no(AST_RESIZE, pos);
    no->alvo = imagem;
    no->args[RESIZE_LARGURA] = largura;
    no->args[RESIZE_ALTURA] = altura;
    no->num_args = 2;
    return no;
}

NoAST *criar_no_tons_de_cinza(Posicao pos, Identificador imagem)
{
    NoAST *no = novo_no(AST_GRAYSCALE, pos);
    no->alvo = imagem;
    return no;
}

NoAST *criar_no_rotacionar(Posicao pos, Identificador imagem, Valor angulo)
{
    NoAST *no = novo_no(AST_ROTATE, pos);
    no->alvo = imagem;
    no->args[ROTATE_ANGULO] = angulo;
    no->num_args = 1;
    return no;
}

NoAST *criar_no_recortar(Posicao pos, Identificador imagem, Valor x, Valor y,
                         Valor largura, Valor altura)
{
    NoAST *no = novo_no(AST_CROP, pos);
    no->alvo = imagem;
    no->args[CROP_X] = x;
    no->args[CROP_Y] = y;
    no->args[CROP_LARGURA] = largura;
    no->args[CROP_ALTURA] = altura;
    no->num_args = 4;
    return no;
}

NoAST *criar_no_salvar(Posicao pos, Identificador imagem, char *arquivo)
{
    NoAST *no = novo_no(AST_SAVE, pos);
    no->alvo = imagem;
    no->arquivo = arquivo;
    return no;
}

const char *nome_tipo_no(TipoNo tipo)
{
    switch (tipo) {
    case AST_PROGRAM:       return "PROGRAM";
    case AST_IMAGE_DECL:    return "IMAGE_DECL";
    case AST_INTEGER_DECL:  return "INTEGER_DECL";
    case AST_RESIZE:        return "RESIZE";
    case AST_GRAYSCALE:     return "GRAYSCALE";
    case AST_ROTATE:        return "ROTATE";
    case AST_CROP:          return "CROP";
    case AST_SAVE:          return "SAVE";
    }
    return "DESCONHECIDO";
}

/* ---- Impressão ---------------------------------------------------------- */

/* Rótulos dos argumentos de cada comando, na ordem de NoAST.args. */
static const char *const ROTULOS_RESIZE[] = { "largura", "altura" };
static const char *const ROTULOS_ROTATE[] = { "angulo" };
static const char *const ROTULOS_CROP[] = { "x", "y", "largura", "altura" };

static const char *const *rotulos_args(TipoNo tipo)
{
    switch (tipo) {
    case AST_RESIZE:  return ROTULOS_RESIZE;
    case AST_ROTATE:  return ROTULOS_ROTATE;
    case AST_CROP:    return ROTULOS_CROP;
    default:          return NULL;
    }
}

/* Desenha o começo de uma linha da árvore: "|-- " ou, no último filho, "`-- ". */
static void ramo(FILE *saida, const char *prefixo, int ultimo)
{
    fprintf(saida, "%s%s", prefixo, ultimo ? "`-- " : "|-- ");
}

static void imprimir_posicao(FILE *saida, Posicao pos)
{
    fprintf(saida, " [%d:%d]\n", pos.linha, pos.coluna);
}

static void imprimir_valor(FILE *saida, const char *rotulo, const Valor *valor)
{
    if (valor->tipo == VALOR_LITERAL)
        fprintf(saida, "%s: %ld", rotulo, valor->numero);
    else
        fprintf(saida, "%s: %s (variavel)", rotulo, valor->nome);
    imprimir_posicao(saida, valor->pos);
}

static void imprimir_comando(FILE *saida, const NoAST *no, int ultimo)
{
    /* Prefixo dos filhos: continua a linha vertical se ainda há comandos. */
    const char *prefixo = ultimo ? "    " : "|   ";
    const char *const *rotulos = rotulos_args(no->tipo);
    int tem_arquivo = no->tipo == AST_IMAGE_DECL || no->tipo == AST_SAVE;
    int tem_numero = no->tipo == AST_INTEGER_DECL;
    int filhos = 1 + no->num_args + tem_arquivo + tem_numero;
    int i;

    ramo(saida, "", ultimo);
    fprintf(saida, "%s", nome_tipo_no(no->tipo));
    imprimir_posicao(saida, no->pos);

    /* Declarações nomeiam a variável criada; os demais comandos, a imagem. */
    ramo(saida, prefixo, filhos == 1);
    fprintf(saida, "%s: %s",
            (no->tipo == AST_IMAGE_DECL || tem_numero) ? "nome" : "imagem",
            no->alvo.nome);
    imprimir_posicao(saida, no->alvo.pos);

    for (i = 0; i < no->num_args; i++) {
        ramo(saida, prefixo, i == no->num_args - 1);
        imprimir_valor(saida, rotulos[i], &no->args[i]);
    }

    if (tem_arquivo) {
        ramo(saida, prefixo, 1);
        fprintf(saida, "arquivo: \"%s\"\n", no->arquivo);
    }
    if (tem_numero) {
        ramo(saida, prefixo, 1);
        fprintf(saida, "valor: %ld\n", no->numero);
    }
}

void imprimir_ast(FILE *saida, const NoAST *raiz)
{
    const NoAST *cmd;

    if (raiz == NULL)
        return;

    fprintf(saida, "PROGRAM (%d %s)\n", raiz->num_comandos,
            raiz->num_comandos == 1 ? "comando" : "comandos");
    for (cmd = raiz->comandos; cmd != NULL; cmd = cmd->proximo)
        imprimir_comando(saida, cmd, cmd->proximo == NULL);
}

/* ---- Liberação ---------------------------------------------------------- */

void liberar_valor(Valor *valor)
{
    free(valor->nome);
    valor->nome = NULL;
}

static void liberar_comando(NoAST *cmd)
{
    int i;

    free(cmd->alvo.nome);
    free(cmd->arquivo);
    for (i = 0; i < cmd->num_args; i++)
        liberar_valor(&cmd->args[i]);
    free(cmd);
}

/* Aceita a raiz (AST_PROGRAM, libera todos os comandos) ou um comando
   avulso, que é o que o Bison descarta quando a análise é abortada. */
void liberar_ast(NoAST *raiz)
{
    NoAST *cmd, *seguinte;

    if (raiz == NULL)
        return;
    if (raiz->tipo != AST_PROGRAM) {
        liberar_comando(raiz);
        return;
    }
    for (cmd = raiz->comandos; cmd != NULL; cmd = seguinte) {
        seguinte = cmd->proximo;
        liberar_comando(cmd);
    }
    free(raiz);
}
