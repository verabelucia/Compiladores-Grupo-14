
/* FGA0003 - Compiladores 1 */
/* Curso de Engenharia de Software */
/* Universidade de Brasilia (UnB) */
/* ImagemLang - representacao intermediaria (Pessoa 4) */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ri.h"

/* Faz uma copia de um texto na memoria. */
static char *copiar_texto(const char *texto)
{
    size_t tamanho = strlen(texto) + 1;
    char *copia = malloc(tamanho);

    if (copia == NULL) {
        fprintf(stderr, "erro: memoria insuficiente ao gerar a RI\n");
        exit(1);
    }

    memcpy(copia, texto, tamanho);

    return copia;
}

/* Cria uma nova instrucao da RI. */
static InstrucaoRI *criar_instrucao(OpRI op, Posicao pos)
{
    InstrucaoRI *nova = calloc(1, sizeof *nova);

    if (nova == NULL) {
        fprintf(stderr, "erro: memoria insuficiente ao gerar a RI\n");
        exit(1);
    }

    nova->op = op;
    nova->pos = pos;

    return nova;
}

/* Adiciona uma instrucao ao final da lista. */
static void adicionar_instrucao(ProgramaRI *ri, InstrucaoRI *nova)
{
    if (ri->primeira == NULL) {
        ri->primeira = nova;
    } else {
        ri->ultima->proximo = nova;
    }

    ri->ultima = nova;
    ri->quantidade++;
}

/* Copia os argumentos de um comando da AST para a RI. */
static void copiar_argumentos(InstrucaoRI *nova, const NoAST *comando)
{
    nova->num_args = comando->num_args;

    for (int i = 0; i < comando->num_args; i++) {
        nova->args[i] = comando->args[i];

        if (comando->args[i].tipo == VALOR_VARIAVEL) {
            nova->args[i].nome =
                copiar_texto(comando->args[i].nome);
        }
    }
}

/* Transforma a AST em representacao intermediaria. */
ProgramaRI *gerar_ri(const NoAST *raiz)
{
    ProgramaRI *ri = calloc(1, sizeof *ri);

    if (ri == NULL) {
        fprintf(stderr, "erro: memoria insuficiente ao gerar a RI\n");
        exit(1);
    }

    if (raiz == NULL) {
        return ri;
    }

    /* Primeira passagem: gera uma instrucao para cada comando. */
    for (const NoAST *comando = raiz->comandos;
         comando != NULL;
         comando = comando->proximo) {

        InstrucaoRI *nova = NULL;

        switch (comando->tipo) {

            case AST_IMAGE_DECL:
                nova = criar_instrucao(RI_LOAD_IMAGE, comando->pos);
                nova->alvo = copiar_texto(comando->alvo.nome);
                nova->arquivo = copiar_texto(comando->arquivo);
                break;

            case AST_INTEGER_DECL:
                nova = criar_instrucao(RI_CONST, comando->pos);
                nova->alvo = copiar_texto(comando->alvo.nome);
                nova->numero = comando->numero;
                break;

            case AST_RESIZE:
                nova = criar_instrucao(RI_RESIZE, comando->pos);
                nova->alvo = copiar_texto(comando->alvo.nome);
                copiar_argumentos(nova, comando);
                break;

            case AST_GRAYSCALE:
                nova = criar_instrucao(RI_GRAYSCALE, comando->pos);
                nova->alvo = copiar_texto(comando->alvo.nome);
                break;

            case AST_ROTATE:
                nova = criar_instrucao(RI_ROTATE, comando->pos);
                nova->alvo = copiar_texto(comando->alvo.nome);
                copiar_argumentos(nova, comando);
                break;

            case AST_CROP:
                nova = criar_instrucao(RI_CROP, comando->pos);
                nova->alvo = copiar_texto(comando->alvo.nome);
                copiar_argumentos(nova, comando);
                break;

            case AST_SAVE:
                nova = criar_instrucao(RI_SAVE, comando->pos);
                nova->alvo = copiar_texto(comando->alvo.nome);
                nova->arquivo = copiar_texto(comando->arquivo);
                break;

            default:
                break;
        }

        if (nova != NULL) {
            adicionar_instrucao(ri, nova);
        }
    }

    /* Segunda passagem: libera as imagens na ordem de declaracao. */
    for (const NoAST *comando = raiz->comandos;
         comando != NULL;
         comando = comando->proximo) {

        if (comando->tipo == AST_IMAGE_DECL) {
            InstrucaoRI *nova =
                criar_instrucao(RI_FREE, comando->pos);

            nova->alvo = copiar_texto(comando->alvo.nome);
            adicionar_instrucao(ri, nova);
        }
    }

    return ri;
}

/* Imprime as instrucoes da representacao intermediaria. */
void imprimir_ri(FILE *saida, const ProgramaRI *ri)
{
    fprintf(saida, "RI (%d %s)\n",
            ri->quantidade,
            ri->quantidade == 1 ? "instrucao" : "instrucoes");

    int numero = 1;

    for (const InstrucaoRI *atual = ri->primeira;
         atual != NULL;
         atual = atual->proximo) {

        fprintf(saida, "%4d  ", numero++);

        switch (atual->op) {

            case RI_LOAD_IMAGE:
                fprintf(saida, "LOAD_IMAGE \"%s\" -> %s",
                        atual->arquivo, atual->alvo);
                break;

            case RI_CONST:
                fprintf(saida, "CONST %ld -> %s",
                        atual->numero, atual->alvo);
                break;

            case RI_RESIZE:
                fprintf(saida, "RESIZE %s, ", atual->alvo);
                break;

            case RI_GRAYSCALE:
                fprintf(saida, "GRAYSCALE %s", atual->alvo);
                break;

            case RI_ROTATE:
                fprintf(saida, "ROTATE %s, ", atual->alvo);
                break;

            case RI_CROP:
                fprintf(saida, "CROP %s, ", atual->alvo);
                break;

            case RI_SAVE:
                fprintf(saida, "SAVE %s, \"%s\"",
                        atual->alvo, atual->arquivo);
                break;

            case RI_FREE:
                fprintf(saida, "FREE %s", atual->alvo);
                break;
        }

        if (atual->op == RI_RESIZE ||
            atual->op == RI_ROTATE ||
            atual->op == RI_CROP) {

            for (int i = 0; i < atual->num_args; i++) {

                if (i > 0) {
                    fprintf(saida, ", ");
                }

                if (atual->args[i].tipo == VALOR_LITERAL) {
                    fprintf(saida, "%ld",
                            atual->args[i].numero);
                } else {
                    fprintf(saida, "%s",
                            atual->args[i].nome);
                }
            }
        }

        fprintf(saida, "\n");
    }
}

/* Libera toda a memoria utilizada pela RI. */
void liberar_ri(ProgramaRI *ri)
{
    if (ri == NULL) {
        return;
    }

    InstrucaoRI *atual = ri->primeira;

    while (atual != NULL) {
        InstrucaoRI *proxima = atual->proximo;

        free(atual->alvo);
        free(atual->arquivo);

        for (int i = 0; i < atual->num_args; i++) {
            if (atual->args[i].tipo == VALOR_VARIAVEL) {
                free(atual->args[i].nome);
            }
        }

        free(atual);
        atual = proxima;
    }

    free(ri);
}
