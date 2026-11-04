---
title: "Como compilar e testar"
parent: Projeto
nav_order: 1
description: "Requisitos, compilação, modos de execução do imagemc e a suíte de testes automatizados."
---

## Requisitos

Flex, Bison 3.6 ou mais recente (testado com 3.8.2), GCC e Make.

{: .atencao }
Confira a versão com `bison --version`. Com Bison antigo, o `make` para logo
no início com a mensagem `require bison 3.6`.

- **Linux / WSL:** Ubuntu 22.04 ou mais recente e Debian 12 já trazem o Bison
  3.8 (`sudo apt install flex bison gcc make`). O Ubuntu 20.04 traz o 3.5,
  que não serve: use uma distribuição mais nova no WSL.
- **macOS:** o `/usr/bin/bison` do sistema é o 2.3. Instale pelo Homebrew
  (`brew install bison flex`) e coloque-o antes no PATH:
  `export PATH="$(brew --prefix bison)/bin:$PATH"`.

## Compilando

```sh
make          # gera build/imagemc
make test     # compila, se preciso, e roda a suíte de testes
make clean    # apaga a pasta build/
```

A pasta `build/` não é versionada: ela guarda o `parser.tab.c` e o
`parser.tab.h` gerados pelo Bison, o `lex.yy.c` gerado pelo Flex e o
executável `imagemc`, todos reconstruíveis a partir de `src/`.

## Executando o compilador

| Comando | Imprime quando não há erro |
|---|---|
| `imagemc prog.img` | só `analise concluida sem erros.` |
| `imagemc --tokens prog.img` | a sequência de tokens (só a análise léxica) |
| `imagemc --ast prog.img` | a árvore sintática abstrata |
| `imagemc --tabela prog.img` | a tabela de símbolos |
| `imagemc --ri prog.img` | a representação intermediária |

Por padrão, `imagemc` analisa o programa e informa se há erros:

```sh
./build/imagemc examples/programa_basico.img
```

```saida
analise concluida sem erros.
```

Com `--ast`, imprime também a árvore sintática abstrata, com a posição
(`[linha:coluna]`) de cada comando e de cada argumento:

```sh
./build/imagemc --ast examples/programa_basico.img
```

```saida
PROGRAM (6 comandos)
|-- IMAGE_DECL [2:1]
|   |-- nome: foto [2:8]
|   `-- arquivo: "foto.jpg"
|-- RESIZE [5:1]
|   |-- imagem: foto [5:15]
|   |-- largura: 750 [5:25]
|   `-- altura: 800 [5:33]
|-- GRAYSCALE [6:1]
|   `-- imagem: foto [6:15]
|-- ROTATE [7:1]
|   |-- imagem: foto [7:12]
|   `-- angulo: 90 [7:17]
|-- CROP [8:1]
|   |-- imagem: foto [8:10]
|   |-- x: 10 [8:18]
|   |-- y: 20 [8:22]
|   |-- largura: 300 [8:33]
|   `-- altura: 200 [8:41]
`-- SAVE [11:1]
    |-- imagem: foto [11:8]
    `-- arquivo: "resultado.jpg"

analise concluida sem erros.
```

Se houver qualquer erro, a árvore não é impressa: saem só as mensagens de erro
e o resumo.

Com `--tokens`, `imagemc` para na análise léxica e lista os tokens:

```sh
./build/imagemc --tokens examples/programa_basico.img
```

Erros saem em `stderr`, no formato `arquivo:linha:coluna: categoria: mensagem`:

```saida
programa.img:2:8: erro sintatico: encontrado 'como', esperado identificador
```

Códigos de saída: `0` sem erros, `1` com erros léxicos, sintáticos ou
semânticos, `2` para uso incorreto ou arquivo inexistente.

## Suíte de testes

```sh
make test
```

Cada arquivo `tests/<fase>/<grupo>/<nome>.img` tem um par `<nome>.esperado`
com a saída completa (stdout e stderr) que o compilador deve produzir. O
script `run_tests.sh` compara as duas e informa `PASSOU` ou `FALHOU`. A fase
decide o modo: `tests/lexico/` roda `imagemc --tokens`, `tests/sintatico/`
roda `imagemc --ast`, `tests/semantico/` roda `imagemc --tabela` e
`tests/intermediario/` roda `imagemc --ri`.

| Pasta | Cobre |
|---|---|
| `tests/lexico/validos/` | declarações, programa completo, comentários |
| `tests/lexico/invalidos/` | caractere inválido, string não terminada, vários erros léxicos |
| `tests/sintatico/validos/` | a AST dos sete comandos, de valores passados por variável e de um programa sem comandos |
| `tests/sintatico/invalidos/` | ordem errada, falta de `;`, palavra reservada como nome, fim inesperado, lixo no início de comando, vários erros com recuperação, erro léxico somado a sintático, string não terminada |
| `tests/semantico/validos/` | tabela de símbolos de um programa sem erros |
| `tests/semantico/invalidos/` | os seis erros do catálogo: variável não declarada, redeclarada, comando de imagem sobre inteiro, imagem usada como inteiro, ângulo inválido, dimensão zero |
| `tests/intermediario/validos/` | a RI de um programa completo e de cada comando (declarações, tons de cinza, rotação, recorte) |
