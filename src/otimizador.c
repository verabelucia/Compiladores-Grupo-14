/* FGA0003 - Compiladores 1 */
/* Curso de Engenharia de Software */
/* Universidade de Brasília (UnB) */
/* ImagemLang - otimizador de RI (dono: Pessoa 1) */

/* Propagação de constantes sobre a RI: cada argumento de RI_RESIZE,
   RI_ROTATE e RI_CROP que é uma variável passa a ser o valor do RI_CONST
   que a declarou.
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

void otimizar_ri(ProgramaRI *ri)
{
    if (ri == NULL)
        return;
    propagar_constantes(ri);
}
