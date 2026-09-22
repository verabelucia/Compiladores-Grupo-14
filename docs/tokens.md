---
layout: default
title: "Tokens"
nav_order: 3
---

O analisador léxico é implementado com Flex (`src/scanner.l`). Sua função é
transformar os caracteres do programa em tokens consumidos pelo parser.

Os códigos numéricos dos tokens não são escritos à mão: eles vêm das
declarações `%token` em `src/parser.y`, e o Bison os gera em
`build/parser.tab.h`, que o lexer inclui. Assim, lexer e parser nunca
divergem sobre o número de um token.

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

O fim de arquivo é o token `FIM`, de código `0`: é o valor que o Bison
espera de `yylex()` quando a entrada acaba. Ele é declarado explicitamente
para ganhar o apelido "fim do arquivo" e uma posição própria. A regra
`<<EOF>>` do lexer registra a linha e a coluna onde o arquivo termina; sem
ela, um erro como `encontrado fim do arquivo, esperado ';'` apontaria para a
coluna do último `\n` lido, uma posição que não existe no arquivo.

## Apelidos nas mensagens de erro

Cada token tem um apelido em `src/parser.y`, usado nas mensagens de erro
sintático no lugar do nome interno:

| Token | Apelido na mensagem |
|---|---|
| palavras-chave | a própria palavra entre aspas simples, ex.: `'como'` |
| `IDENTIFICADOR` | `identificador` |
| `LITERAL_INTEIRO` | `numero inteiro` |
| `LITERAL_STRING` | `texto entre aspas` |
| símbolos | o próprio símbolo entre aspas simples, ex.: `';'` |
| `FIM` | `fim do arquivo` |

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
- entregar os tokens ao Bison, com o valor em `yylval` (`texto` ou `inteiro`)
  e a posição em `yylloc`.

As strings de identificadores e literais são alocadas pelo lexer, e quem
consome o token as libera. Durante a recuperação de erros, o Bison descarta
tokens; a diretiva `%destructor` libera as strings desses tokens descartados.

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
