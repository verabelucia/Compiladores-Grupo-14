---
layout: default
title: ImagemLang
nav_order: 1
---

ImagemLang é uma linguagem específica de domínio (DSL) para processamento de
imagens, desenvolvida na disciplina FGA0003 - Compiladores 1, tendo C como
linguagem-alvo.

Um programa ImagemLang descreve operações de imagem em alto nível:

```text
imagem foto = "foto.jpg";

redimensionar foto para 750 por 800;
tons_de_cinza foto;
rotacionar foto 90;

salvar foto como "resultado.jpg";
```

O compilador transforma esse programa em código C. O código gerado utiliza um
pequeno runtime próprio, construído sobre as bibliotecas header-only
`stb_image`, `stb_image_write` e `stb_image_resize2`.

## Estado atual

O analisador léxico está implementado e coberto por testes automatizados. O
parser, a análise semântica e o gerador de código ainda não existem.

## Fases do compilador

```text
Código-fonte ImagemLang
        ↓
Análise léxica (Flex)
        ↓
Análise sintática (Bison)
        ↓
AST e tabela de símbolos
        ↓
Análise semântica
        ↓
Representação intermediária
        ↓
Geração de código C
        ↓
GCC + runtime (stb)
        ↓
Executável
```

## Escopo mínimo

A primeira versão oferece:

- declaração e carregamento de imagens;
- declaração de valores inteiros simples;
- redimensionamento;
- conversão para tons de cinza;
- rotação;
- recorte;
- salvamento.

Não fazem parte do escopo inicial: funções, estruturas condicionais, laços de
repetição, expressões aritméticas completas, filtros definidos pelo usuário e
otimizações avançadas.

## Decisões de projeto já tomadas

Estas decisões foram fechadas e não devem ser reabertas sem motivo técnico.
O raciocínio completo está na [especificação da linguagem](linguagem.md).

- **Comandos mutam a imagem; não há composição.** `tons_de_cinza foto;` altera
  `foto` no lugar. A forma `imagem cinza = tons_de_cinza foto;` fica como
  extensão opcional para a entrega final.
- **Dimensões usam `por`, não `x`.** `750 por 800` evita a ambiguidade léxica
  entre o separador `x` e um identificador começado por `x`.
- **Todo comando termina em `;`.** O ponto e vírgula é o token de sincronização
  usado na recuperação de erros sintáticos.
- **O runtime usa `stb`, não MagickWand.** As bibliotecas `stb` são header-only
  e ficam versionadas no próprio repositório, sem dependência de instalação.

## Decisões que precisam ser confirmadas com o professor

- aceitação de C como linguagem-alvo;
- aceitação de uma representação intermediária própria antes da geração de C;
- data exata para disponibilizar a entrega final no repositório.
