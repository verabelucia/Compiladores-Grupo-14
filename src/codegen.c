/* FGA0003 - Compiladores 1 */
/* Curso de Engenharia de Software */
/* Universidade de Brasília (UnB) */
/* ImagemLang - gerador de código C (dono: Pessoa 2) */

#include "codegen.h"

/* Emite um literal string C escapando aspas duplas e barras invertidas. */
static void emitir_string(FILE *saida, const char *texto)
{
    fputc('"', saida);
    if (texto != NULL) {
        for (const char *p = texto; *p != '\0'; p++) {
            if (*p == '\\' || *p == '"')
                fputc('\\', saida);
            fputc(*p, saida);
        }
    }
    fputc('"', saida);
}

/* Emite um argumento inteiro: número literal ou variável prefixada (im_). */
static void emitir_valor(FILE *saida, const Valor *valor)
{
    if (valor->tipo == VALOR_LITERAL)
        fprintf(saida, "%ld", valor->numero);
    else
        fprintf(saida, "im_%s", valor->nome);
}

/* Emite o código C correspondente a uma única instrução da RI. */
static void emitir_instrucao(FILE *saida, const InstrucaoRI *inst)
{
    switch (inst->op) {
    default:
        (void) saida;
        (void) emitir_string;
        (void) emitir_valor;
        break;
    }
}

void gerar_c(FILE *saida, const ProgramaRI *ri)
{
    if (saida == NULL)
        return;

    fprintf(saida, "#include \"runtime.h\"\n\n");
    fprintf(saida, "int main(void) {\n");

    if (ri != NULL) {
        for (const InstrucaoRI *inst = ri->primeira; inst != NULL;
             inst = inst->proximo) {
            emitir_instrucao(saida, inst);
        }
    }

    fprintf(saida, "    return 0;\n");
    fprintf(saida, "}\n");
}
