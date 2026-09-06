---
layout: default
title: "Como compilar e testar"
nav_order: 7
---

Requisitos: Flex, GCC e Make (o Make entra junto com o parser). Enquanto o
parser não existe, dois comandos bastam:

```sh
mkdir -p build
flex -o build/lex.yy.c src/scanner.l
gcc -Wall -Wextra -g -Isrc -o build/scanner build/lex.yy.c src/scanner_main.c
```

A pasta `build/` não é versionada: ela guarda apenas o `lex.yy.c` gerado pelo
Flex e o executável produzido pelo GCC, ambos reconstruíveis a partir de
`src/`.

## Executando o scanner

Para ver a sequência de tokens de um programa:

```sh
./build/scanner examples/programa_basico.img
```

O arquivo de exemplo é este:

```text
// Carrega a imagem original.
imagem foto = "foto.jpg";

// Aplica as transformações.
redimensionar foto para 750 por 800;
tons_de_cinza foto;
rotacionar foto 90;
recortar foto de 10, 20 tamanho 300 por 200;

// Grava o resultado.
salvar foto como "resultado.jpg";
```

## Suíte de testes

```sh
./run_tests.sh
```

Cada arquivo `tests/<grupo>/<nome>.img` tem um par `<nome>.esperado` com a
saída completa que o scanner deve produzir. O script compara as duas e informa
`PASSOU` ou `FALHOU`.

Os testes em `tests/validos/` cobrem declarações, um programa completo e o
tratamento de comentários. Os de `tests/invalidos/` cobrem caractere fora do
vocabulário, string não terminada e a recuperação após vários erros na mesma
entrada.
