/* FGA0003 - Compiladores 1
   Curso de Engenharia de Software
   Universidade de Brasilia (UnB)
   ImagemLang - codigos dos tokens reconhecidos pelo analisador lexico

   Este arquivo existe enquanto o parser nao foi implementado. Quando o
   parser.y entrar, os codigos passam a ser gerados pelo Bison em
   parser.tab.h e este arquivo deixa de ser necessario. Os valores comecam
   em 256 porque o Bison reserva os codigos abaixo disso para caracteres. */

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
