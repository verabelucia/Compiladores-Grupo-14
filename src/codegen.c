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

static void emitir_falha(FILE *saida, const ProgramaRI *ri,
                         const InstrucaoRI *atual)
{
    int liberou = 0;

    for (const InstrucaoRI *inst = ri->primeira; inst != NULL && inst != atual;
         inst = inst->proximo) {
        if (inst->op != RI_LOAD_IMAGE)
            continue;
        fprintf(saida, "%s img_destroy(im_%s);", liberou ? "" : " {", inst->alvo);
        liberou = 1;
    }
    fprintf(saida, liberou ? " return 1; }\n" : " return 1;\n");
}

static void emitir_instrucao(FILE *saida, const ProgramaRI *ri,
                             const InstrucaoRI *inst)
{
    switch (inst->op) {
    case RI_LOAD_IMAGE:
        fprintf(saida, "    Imagem *im_%s = img_load(", inst->alvo);
        emitir_string(saida, inst->arquivo);
        fprintf(saida, ");\n");
        fprintf(saida, "    if (!im_%s)", inst->alvo);
        emitir_falha(saida, ri, inst);
        break;
    case RI_CONST:
        fprintf(saida, "    int im_%s = %ld;\n", inst->alvo, inst->numero);
        break;
    case RI_RESIZE:
        fprintf(saida, "    if (img_resize(im_%s, ", inst->alvo);
        emitir_valor(saida, &inst->args[RESIZE_LARGURA]);
        fprintf(saida, ", ");
        emitir_valor(saida, &inst->args[RESIZE_ALTURA]);
        fprintf(saida, ") != 0)");
        emitir_falha(saida, ri, inst);
        break;
    case RI_GRAYSCALE:
        fprintf(saida, "    if (img_grayscale(im_%s) != 0)", inst->alvo);
        emitir_falha(saida, ri, inst);
        break;
    case RI_ROTATE:
        fprintf(saida, "    if (img_rotate(im_%s, ", inst->alvo);
        emitir_valor(saida, &inst->args[ROTATE_ANGULO]);
        fprintf(saida, ") != 0)");
        emitir_falha(saida, ri, inst);
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
        fprintf(saida, ") != 0)");
        emitir_falha(saida, ri, inst);
        break;
    case RI_SAVE:
        fprintf(saida, "    if (img_save(im_%s, ", inst->alvo);
        emitir_string(saida, inst->arquivo);
        fprintf(saida, ") != 0)");
        emitir_falha(saida, ri, inst);
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
            emitir_instrucao(saida, ri, inst);
        }
    }

    fprintf(saida, "    return 0;\n");
    fprintf(saida, "}\n");
}
