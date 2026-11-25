#include <stdarg.h>
#include <stdio.h>

#include "semantico.h"

extern const char *arquivo_atual;

int erros_semanticos = 0;

void erro_semantico(Posicao pos, const char *formato, ...)
{
    va_list args;

    erros_semanticos++;
    fprintf(stderr, "%s:%d:%d: erro semantico: ",
            arquivo_atual, pos.linha, pos.coluna);
    va_start(args, formato);
    vfprintf(stderr, formato, args);
    va_end(args);
    fprintf(stderr, "\n");
}

static void verificar_alvo(const NoAST *cmd, const Simbolo *simbolo)
{
    if (simbolo == NULL || simbolo->tipo == TIPO_IMAGEM)
        return;
    erro_semantico(cmd->alvo.pos, "'%s' espera imagem, mas '%s' e inteiro",
                   palavra_chave_no(cmd->tipo), cmd->alvo.nome);
}

static int eh_dimensao(TipoNo tipo, int i)
{
    return tipo == AST_RESIZE
        || (tipo == AST_CROP && (i == CROP_LARGURA || i == CROP_ALTURA));
}

static int eh_coordenada(TipoNo tipo, int i)
{
    return tipo == AST_CROP && (i == CROP_X || i == CROP_Y);
}

static int angulo_permitido(long angulo)
{
    return angulo == 90 || angulo == 180 || angulo == 270;
}

static void verificar_argumento(const NoAST *cmd, int i, const Valor *valor,
                                const Simbolo *simbolo)
{
    const char *rotulo = rotulo_arg(cmd->tipo, i);
    long numero;

    if (valor->tipo == VALOR_LITERAL) {
        numero = valor->numero;
    } else {
        if (simbolo == NULL)
            return;
        if (simbolo->tipo != TIPO_INTEIRO) {
            erro_semantico(valor->pos, "%s deve ser inteiro, mas '%s' e imagem",
                           rotulo, valor->nome);
            return;
        }
        numero = simbolo->valor;
    }

    if (eh_dimensao(cmd->tipo, i) && numero <= 0)
        erro_semantico(valor->pos, "%s deve ser maior que zero, mas vale %ld",
                       rotulo, numero);
    else if (cmd->tipo == AST_ROTATE && !angulo_permitido(numero))
        erro_semantico(valor->pos,
                       "angulo deve ser 90, 180 ou 270, mas vale %ld", numero);
    else if (eh_coordenada(cmd->tipo, i) && numero < 0)
        erro_semantico(valor->pos, "%s nao pode ser negativo, mas vale %ld",
                       rotulo, numero);
}

int analisar_semantica(const NoAST *raiz, TabelaSimbolos *tabela)
{
    const NoAST *cmd;
    int i;

    if (raiz == NULL)
        return erros_semanticos;

    for (cmd = raiz->comandos; cmd != NULL; cmd = cmd->proximo) {
        switch (cmd->tipo) {
        case AST_IMAGE_DECL:
            tabela_declarar(tabela, cmd->alvo, TIPO_IMAGEM, 0);
            break;
        case AST_INTEGER_DECL:
            tabela_declarar(tabela, cmd->alvo, TIPO_INTEIRO, cmd->numero);
            break;
        default:
            verificar_alvo(cmd, tabela_usar(tabela, cmd->alvo.nome,
                                            cmd->alvo.pos));
            for (i = 0; i < cmd->num_args; i++) {
                const Valor *valor = &cmd->args[i];
                const Simbolo *simbolo = NULL;

                if (valor->tipo == VALOR_VARIAVEL)
                    simbolo = tabela_usar(tabela, valor->nome, valor->pos);
                verificar_argumento(cmd, i, valor, simbolo);
            }
            break;
        }
    }
    return erros_semanticos;
}
