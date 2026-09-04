/* FGA0003 - Compiladores 1 */
/* Curso de Engenharia de Software */
/* Universidade de Brasília (UnB) */
/* ImagemLang - códigos dos tokens do analisador léxico */

#ifndef IMAGEMLANG_TOKENS_H
#define IMAGEMLANG_TOKENS_H

enum {
    KW_IMAGEM = 256,
    KW_INTEIRO,
    KW_REDIMENSIONAR,
    KW_PARA,
    KW_POR,
    KW_TONS_DE_CINZA,
    KW_ROTACIONAR,
    KW_RECORTAR,
    KW_DE,
    KW_TAMANHO,
    KW_SALVAR,
    KW_COMO,

    IDENTIFICADOR,
    LITERAL_INTEIRO,
    LITERAL_STRING,

    ATRIBUICAO,
    VIRGULA,
    PONTO_E_VIRGULA
};

#endif
