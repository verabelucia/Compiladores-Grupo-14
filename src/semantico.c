/* FGA0003 - Compiladores 1 */
/* Curso de Engenharia de Software */
/* Universidade de Brasília (UnB) */
/* ImagemLang - análise semântica (dono: Pessoa 3) */

#include <stdarg.h>
#include <stdio.h>

#include "semantico.h"

extern const char *arquivo_atual;

int erros_semanticos = 0;

/* Pronta: é a parte do contrato que a tabela (Pessoa 2) também usa. */
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

/* Verifica se o alvo de um comando de imagem (redimensionar, tons_de_cinza,
   rotacionar, recortar, salvar) é do tipo imagem. 'simbolo' é NULL se o
   nome não foi declarado; nesse caso o erro já foi relatado pela tabela e
   não há mais nada a verificar. Mensagem: docs/modulos.md, erro 3. */
static void verificar_alvo(const NoAST *cmd, const Simbolo *simbolo)
{
    if (simbolo == NULL || simbolo->tipo == TIPO_IMAGEM)
        return;
    erro_semantico(cmd->alvo.pos, "'%s' espera imagem, mas '%s' e inteiro",
                   palavra_chave_no(cmd->tipo), cmd->alvo.nome);
}

/* Largura e altura de redimensionar e de recortar. */
static int eh_dimensao(TipoNo tipo, int i)
{
    return tipo == AST_RESIZE
        || (tipo == AST_CROP && (i == CROP_LARGURA || i == CROP_ALTURA));
}

/* Coordenadas x e y de recortar. */
static int eh_coordenada(TipoNo tipo, int i)
{
    return tipo == AST_CROP && (i == CROP_X || i == CROP_Y);
}

static int angulo_permitido(long angulo)
{
    return angulo == 90 || angulo == 180 || angulo == 270;
}

/* Verifica o i-ésimo argumento do comando:
     - se é variável, precisa ser do tipo inteiro (docs/modulos.md, erro 4);
     - domínio, com o número do literal ou o 'valor' da variável na tabela:
       dimensões > 0 (erro 5), ângulo em {90, 180, 270} (erro 6) e
       coordenadas >= 0 (erro 7).
   'simbolo' é o símbolo da variável (NULL se literal ou não declarada). */
static void verificar_argumento(const NoAST *cmd, int i, const Valor *valor,
                                const Simbolo *simbolo)
{
    const char *rotulo = rotulo_arg(cmd->tipo, i);
    long numero;

    if (valor->tipo == VALOR_LITERAL) {
        numero = valor->numero;
    } else {
        /* Não declarada: a tabela já relatou o erro 1. */
        if (simbolo == NULL)
            return;
        if (simbolo->tipo != TIPO_INTEIRO) {
            erro_semantico(valor->pos, "%s deve ser inteiro, mas '%s' e imagem",
                           rotulo, valor->nome);
            return;
        }
        /* Sem atribuição na linguagem, todo inteiro é constante. */
        numero = simbolo->valor;
    }

    if (eh_dimensao(cmd->tipo, i) && numero <= 0)
        erro_semantico(valor->pos, "%s deve ser maior que zero, mas vale %ld",
                       rotulo, numero);
    else if (cmd->tipo == AST_ROTATE && !angulo_permitido(numero))
        erro_semantico(valor->pos,
                       "angulo deve ser 90, 180 ou 270, mas vale %ld", numero);
    /* Hoje não ocorre (a linguagem não tem números negativos); protege a
       regra caso eles sejam introduzidos. */
    else if (eh_coordenada(cmd->tipo, i) && numero < 0)
        erro_semantico(valor->pos, "%s nao pode ser negativo, mas vale %ld",
                       rotulo, numero);
}

/* Percurso pronto: declara, procura cada nome usado e chama as
   verificações acima, na ordem do programa e, dentro de cada comando,
   primeiro o alvo e depois os argumentos. */
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
