/* FGA0003 - Compiladores 1 */
/* Curso de Engenharia de Software */
/* Universidade de Brasília (UnB) */
/* ImagemLang - otimizador de RI (dono: Pessoa 1) */

/* CONTRATO CONGELADO: as assinaturas abaixo foram combinadas pelo grupo.
   Mudar alguma delas exige combinar antes com quem a usa (main.c).
   Detalhes da otimização: docs/modulos.md. */

#ifndef IMAGEMLANG_OTIMIZADOR_H
#define IMAGEMLANG_OTIMIZADOR_H

#include "ri.h"

/* Aplica otimizações sobre a RI no lugar (in-place), preservando a semântica
   do programa. Escopo da semana 09: propagação de constantes inteiras
   (trocar usos de variáveis declaradas por RI_CONST pelos seus valores
   literais e remover as instruções RI_CONST sem uso). Aceita NULL. */
void otimizar_ri(ProgramaRI *ri);

#endif
