/* FGA0003 - Compiladores 1 */
/* Curso de Engenharia de Software */
/* Universidade de Brasília (UnB) */
/* ImagemLang - otimizador de RI */

/* Propagação de constantes em duas passagens sobre a RI:
     1. cada argumento de RI_RESIZE, RI_ROTATE e RI_CROP que é uma variável
        passa a ser o valor do RI_CONST que a declarou;
     2. os RI_CONST que ficaram sem nenhum uso são removidos da lista.
   Como a ImagemLang não tem atribuição, o valor de um inteiro nunca muda
   depois da declaração, e a troca do nome pelo valor não altera o
   resultado do programa. */

#include <stdlib.h>
#include <string.h>

#include "otimizador.h"

static int usa_inteiros(OpRI op)
{
    return op == RI_RESIZE || op == RI_ROTATE || op == RI_CROP;
}

/* Último RI_CONST com esse nome antes de 'limite' (a instrução que usa o
   nome). A semântica garante que ele existe e é único; procurar só antes
   do uso mantém a regra "declarar antes de usar" mesmo assim. */
static const InstrucaoRI *buscar_const(const ProgramaRI *ri, const char *nome,
                                       const InstrucaoRI *limite)
{
    const InstrucaoRI *inst, *achada = NULL;

    for (inst = ri->primeira; inst != NULL && inst != limite; inst = inst->proximo)
        if (inst->op == RI_CONST && strcmp(inst->alvo, nome) == 0)
            achada = inst;
    return achada;
}

/* Passagem 1: troca cada argumento-variável pelo valor da constante. */
static void propagar_constantes(ProgramaRI *ri)
{
    InstrucaoRI *inst;
    int i;

    for (inst = ri->primeira; inst != NULL; inst = inst->proximo) {
        if (!usa_inteiros(inst->op))
            continue;
        for (i = 0; i < inst->num_args; i++) {
            Valor *arg = &inst->args[i];
            const InstrucaoRI *def;

            if (arg->tipo != VALOR_VARIAVEL)
                continue;
            def = buscar_const(ri, arg->nome, inst);
            if (def == NULL)
                continue;
            /* A RI é dona do nome copiado: libera antes de trocar. */
            free(arg->nome);
            arg->nome = NULL;
            arg->tipo = VALOR_LITERAL;
            arg->numero = def->numero;
        }
    }
}

/* Algum argumento da RI ainda usa este nome? */
static int nome_em_uso(const ProgramaRI *ri, const char *nome)
{
    const InstrucaoRI *inst;
    int i;

    for (inst = ri->primeira; inst != NULL; inst = inst->proximo) {
        if (!usa_inteiros(inst->op))
            continue;
        for (i = 0; i < inst->num_args; i++)
            if (inst->args[i].tipo == VALOR_VARIAVEL
                && strcmp(inst->args[i].nome, nome) == 0)
                return 1;
    }
    return 0;
}

/* Libera uma instrução e todas as strings que ela possui (as mesmas que
   liberar_ri libera). */
static void liberar_instrucao(InstrucaoRI *inst)
{
    int i;

    free(inst->alvo);
    free(inst->arquivo);
    for (i = 0; i < inst->num_args; i++)
        if (inst->args[i].tipo == VALOR_VARIAVEL)
            free(inst->args[i].nome);
    free(inst);
}

/* Passagem 2: remove os RI_CONST sem uso, ajustando a lista, o ponteiro
   para a última instrução e a contagem. */
static void remover_consts_mortos(ProgramaRI *ri)
{
    InstrucaoRI *anterior = NULL, *inst = ri->primeira, *seguinte;

    while (inst != NULL) {
        seguinte = inst->proximo;
        if (inst->op == RI_CONST && !nome_em_uso(ri, inst->alvo)) {
            if (anterior == NULL)
                ri->primeira = seguinte;
            else
                anterior->proximo = seguinte;
            if (ri->ultima == inst)
                ri->ultima = anterior;
            ri->quantidade--;
            liberar_instrucao(inst);
        } else {
            anterior = inst;
        }
        inst = seguinte;
    }
}

void otimizar_ri(ProgramaRI *ri)
{
    if (ri == NULL)
        return;
    propagar_constantes(ri);
    remover_consts_mortos(ri);
}
