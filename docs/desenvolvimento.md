---
layout: default
title: "Como compilar e testar"
nav_order: 7
---

Requisitos: Flex, Bison 3.6 ou mais recente, GCC e Make.

```sh
make          # gera build/imagemc
make test     # compila, se preciso, e roda a suíte de testes
make clean    # apaga a pasta build/
```

A pasta `build/` não é versionada: ela guarda o `parser.tab.c` e o
`parser.tab.h` gerados pelo Bison, o `lex.yy.c` gerado pelo Flex e o
executável `imagemc`, todos reconstruíveis a partir de `src/`.

## Executando o compilador

Por padrão, `imagemc` faz a análise sintática e imprime cada comando
reconhecido com linha e coluna:

```sh
./build/imagemc examples/programa_basico.img
```

```text
  2:1   imagem foto = "foto.jpg"
  5:1   redimensionar foto para 750 por 800
  6:1   tons_de_cinza foto
  7:1   rotacionar foto 90
  8:1   recortar foto de 10, 20 tamanho 300 por 200
 11:1   salvar foto como "resultado.jpg"

analise sintatica concluida sem erros.
```

Essa saída é provisória: a partir da semana 06, o parser passa a construir a
AST em vez de imprimir os comandos.

Com `--tokens`, `imagemc` para na análise léxica e lista os tokens:

```sh
./build/imagemc --tokens examples/programa_basico.img
```

Erros saem em `stderr`, no formato `arquivo:linha:coluna: categoria: mensagem`:

```text
programa.img:2:8: erro sintatico: encontrado 'como', esperado identificador
```

Códigos de saída: `0` sem erros, `1` com erros léxicos ou sintáticos, `2` para
uso incorreto ou arquivo inexistente.

## Suíte de testes

```sh
make test
```

Cada arquivo `tests/<fase>/<grupo>/<nome>.img` tem um par `<nome>.esperado`
com a saída completa (stdout e stderr) que o compilador deve produzir. O
script `run_tests.sh` compara as duas e informa `PASSOU` ou `FALHOU`. A fase
decide o modo: `tests/lexico/` roda `imagemc --tokens` e `tests/sintatico/`
roda `imagemc`.

| Pasta | Cobre |
|---|---|
| `tests/lexico/validos/` | declarações, programa completo, comentários |
| `tests/lexico/invalidos/` | caractere inválido, string não terminada, vários erros léxicos |
| `tests/sintatico/validos/` | os sete comandos, valores passados por variável, programa sem comandos |
| `tests/sintatico/invalidos/` | ordem errada, falta de `;`, palavra reservada como nome, fim inesperado, lixo no início de comando, vários erros com recuperação, erro léxico somado a sintático, string não terminada |
