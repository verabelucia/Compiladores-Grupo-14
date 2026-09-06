---
layout: default
title: "Tokens"
nav_order: 3
---

O analisador léxico será implementado com Flex. Sua função será transformar os
caracteres do programa em tokens consumidos pelo parser.

## Palavras-chave

| Token | Lexema |
|---|---|
| `KW_IMAGEM` | `imagem` |
| `KW_INTEIRO` | `inteiro` |
| `KW_REDIMENSIONAR` | `redimensionar` |
| `KW_PARA` | `para` |
| `KW_POR` | `por` |
| `KW_TONS_DE_CINZA` | `tons_de_cinza` |
| `KW_ROTACIONAR` | `rotacionar` |
| `KW_RECORTAR` | `recortar` |
| `KW_DE` | `de` |
| `KW_TAMANHO` | `tamanho` |
| `KW_SALVAR` | `salvar` |
| `KW_COMO` | `como` |

## Tokens com valor

| Token | Padrão conceitual | Exemplos |
|---|---|---|
| `IDENTIFICADOR` | `[A-Za-z_][A-Za-z0-9_]*` | `foto`, `angulo_1` |
| `LITERAL_INTEIRO` | `[0-9]+` | `90`, `750` |
| `LITERAL_STRING` | texto entre aspas duplas | `"foto.jpg"` |

As regras das palavras-chave deverão aparecer antes da regra de identificador
no arquivo Flex. Assim, `imagem` será reconhecida como `KW_IMAGEM`, e não como
um identificador comum.

Como consequência, todas as palavras-chave são reservadas: nenhuma delas pode
ser usada como nome de variável. Isso inclui palavras curtas e de aparência
comum, como `de`, `por`, `para` e `tamanho`. A decisão está registrada em
[linguagem.md](linguagem.md).

## Símbolos

| Token | Símbolo |
|---|---|
| `ATRIBUICAO` | `=` |
| `VIRGULA` | `,` |
| `PONTO_E_VIRGULA` | `;` |

O fim de arquivo não é declarado como token. No Flex, o fim da entrada é
sinalizado por `yylex()` retornando `0`, valor que o Bison interpreta como
`$end`.

## Elementos ignorados

- espaços;
- tabulações;
- quebras de linha, preservando a contagem de linhas;
- comentários iniciados por `//`.

## Responsabilidades do lexer

- reconhecer tokens;
- conservar o valor de identificadores, inteiros e strings;
- acompanhar linha e coluna;
- reportar caracteres inválidos;
- reportar strings não terminadas;
- entregar os tokens ao Bison.

O lexer não verifica a ordem dos tokens nem a compatibilidade de tipos. Essas
responsabilidades pertencem, respectivamente, às análises sintática e
semântica.

## Tokenização de exemplo

Entrada:

```text
imagem foto = "foto.jpg";
```

Saída conceitual:

```text
KW_IMAGEM
IDENTIFICADOR("foto")
ATRIBUICAO
LITERAL_STRING("foto.jpg")
PONTO_E_VIRGULA
```
