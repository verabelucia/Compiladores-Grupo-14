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
    case RI_LOAD_IMAGE:
        fprintf(saida, "    Imagem *im_%s = img_load(", inst->alvo);
        emitir_string(saida, inst->arquivo);
        fprintf(saida, ");\n");
        fprintf(saida, "    if (!im_%s) return 1;\n", inst->alvo);
        break;
    case RI_CONST:
        fprintf(saida, "    int im_%s = %ld;\n", inst->alvo, inst->numero);
        break;
    case RI_RESIZE:
        fprintf(saida, "    if (img_resize(im_%s, ", inst->alvo);
        emitir_valor(saida, &inst->args[RESIZE_LARGURA]);
        fprintf(saida, ", ");
        emitir_valor(saida, &inst->args[RESIZE_ALTURA]);
        fprintf(saida, ") != 0) return 1;\n");
        break;
    case RI_GRAYSCALE:
        fprintf(saida, "    if (img_grayscale(im_%s) != 0) return 1;\n", inst->alvo);
        break;
    case RI_ROTATE:
        fprintf(saida, "    if (img_rotate(im_%s, ", inst->alvo);
        emitir_valor(saida, &inst->args[ROTATE_ANGULO]);
        fprintf(saida, ") != 0) return 1;\n");
        break;
    case RI_FREE:
        fprintf(saida, "    img_destroy(im_%s);\n", inst->alvo);
        break;
    default:
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
