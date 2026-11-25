#include "codegen.h"

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

static void emitir_valor(FILE *saida, const Valor *valor)
{
    if (valor->tipo == VALOR_LITERAL)
        fprintf(saida, "%ld", valor->numero);
    else
        fprintf(saida, "im_%s", valor->nome);
}

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
    case RI_CROP:
        fprintf(saida, "    if (img_crop(im_%s, ", inst->alvo);
        emitir_valor(saida, &inst->args[CROP_X]);
        fprintf(saida, ", ");
        emitir_valor(saida, &inst->args[CROP_Y]);
        fprintf(saida, ", ");
        emitir_valor(saida, &inst->args[CROP_LARGURA]);
        fprintf(saida, ", ");
        emitir_valor(saida, &inst->args[CROP_ALTURA]);
        fprintf(saida, ") != 0) return 1;\n");
        break;
    case RI_SAVE:
        fprintf(saida, "    if (img_save(im_%s, ", inst->alvo);
        emitir_string(saida, inst->arquivo);
        fprintf(saida, ") != 0) return 1;\n");
        break;
    case RI_FREE:
        fprintf(saida, "    img_destroy(im_%s);\n", inst->alvo);
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
