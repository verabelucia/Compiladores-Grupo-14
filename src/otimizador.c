#include <stdlib.h>
#include <string.h>

#include "otimizador.h"

static int usa_inteiros(OpRI op)
{
    return op == RI_RESIZE || op == RI_ROTATE || op == RI_CROP;
}

static const InstrucaoRI *buscar_const(const ProgramaRI *ri, const char *nome,
                                       const InstrucaoRI *limite)
{
    const InstrucaoRI *inst, *achada = NULL;

    for (inst = ri->primeira; inst != NULL && inst != limite; inst = inst->proximo)
        if (inst->op == RI_CONST && strcmp(inst->alvo, nome) == 0)
            achada = inst;
    return achada;
}

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
            free(arg->nome);
            arg->nome = NULL;
            arg->tipo = VALOR_LITERAL;
            arg->numero = def->numero;
        }
    }
}

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
