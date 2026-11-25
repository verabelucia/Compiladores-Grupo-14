/* FGA0003 - Compiladores 1 */
/* Curso de Engenharia de Software */
/* Universidade de Brasília (UnB) */
/* ImagemLang - gerador de código C */

/* CONTRATO CONGELADO: as assinaturas abaixo foram combinadas pelo grupo.
   Mudar alguma delas exige combinar antes com quem a usa (main.c).
   Detalhes da geração de código: docs/modulos.md. */

#ifndef IMAGEMLANG_CODEGEN_H
#define IMAGEMLANG_CODEGEN_H

#include <stdio.h>

#include "ri.h"

/* Emite em 'saida' um programa C completo (#include "runtime.h" e int main)
   equivalente às instruções de 'ri', chamando a API de runtime/runtime.h.
   Variáveis da DSL recebem prefixo (ex.: im_foto) para evitar colisão com
   palavras reservadas do C. Aceita ri vazio ou NULL sem falhar. */
void gerar_c(FILE *saida, const ProgramaRI *ri);

#endif
