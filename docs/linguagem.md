---
layout: default
title: "Especificação da linguagem"
nav_order: 2
---

## Propósito

A ImagemLang permite descrever transformações de imagens por meio de comandos
de alto nível. O compilador verifica o programa e gera código C equivalente.

## Convenções léxicas

- cada comando termina com `;`;
- identificadores utilizam apenas letras ASCII, números e `_`;
- um identificador não pode começar com número;
- números inteiros são escritos em base decimal;
- caminhos de arquivos ficam entre aspas duplas;
- espaços, tabulações e quebras de linha separam tokens;
- comentários de uma linha começam com `//`;
- as palavras-chave são escritas em letras minúsculas e sem acentos.

## Tipos iniciais

### `imagem`

Representa uma imagem carregada de um arquivo.

```text
imagem foto = "foto.jpg";
```

### `inteiro`

Representa um valor inteiro que pode ser utilizado como argumento de uma
operação.

```text
inteiro angulo = 90;
```

Não haverá expressões aritméticas na primeira versão.

## Comandos

### Redimensionar

```text
redimensionar foto para 750 por 800;
```

Os dois valores representam largura e altura.

### Converter para tons de cinza

```text
tons_de_cinza foto;
```

### Rotacionar

```text
rotacionar foto 90;
```

Na primeira versão, serão aceitos os ângulos `90`, `180` e `270`.

### Recortar

```text
recortar foto de 10, 20 tamanho 300 por 200;
```

Os primeiros valores representam as coordenadas `x` e `y`. Os valores após
`tamanho` representam largura e altura.

### Salvar

```text
salvar foto como "resultado.jpg";
```

## Decisões de design

As decisões abaixo foram fechadas antes da implementação do parser, porque
alterá-las depois exigiria reescrever a gramática.

### Comandos mutam a imagem

Uma operação altera a imagem no lugar e não produz um valor:

```text
tons_de_cinza foto;
```

A forma alternativa, em que a operação retorna uma nova imagem, não faz parte
do escopo inicial:

```text
imagem cinza = tons_de_cinza foto;   // fora do escopo inicial
```

O motivo é o custo em três frentes. A gramática precisaria de um não-terminal
`expressao` capaz de aninhar operações, o que reintroduz parênteses e risco de
conflito no Bison. A análise semântica passaria a sintetizar tipos ao subir na
árvore, em vez de apenas consultar o tipo de um símbolo declarado. E a geração
de código precisaria decidir a semântica de cópia: atribuir uma imagem a outra
variável sem clonar faria com que mutar uma alterasse a outra, exigindo uma
operação `img_clone` e uma política explícita de propriedade dos dados.

A escolha por mutação não reduz a capacidade de demonstrar análise semântica.
A verificação de tipo continua sendo exercida por casos como `tons_de_cinza x`
aplicado a um `inteiro`.

A composição fica registrada como extensão opcional para a entrega final. Se
for implementada, o ponto de entrada é a declaração de imagem, que passaria de
`"imagem" identificador "=" string ";"` para `"imagem" identificador "="
expressao ";"`.

### Dimensões usam `por`, e não `x`

A sintaxe adotada é `redimensionar foto para 750 por 800;`.

A forma `750x800` foi descartada por criar ambiguidade léxica. O scanner teria
de distinguir o separador `x` de um identificador iniciado por `x`, o que
exigiria um token composto para pares de dimensões ou uma regra sensível a
contexto. A palavra `por` resolve o problema com um token de palavra-chave
comum.

### Todo comando termina em ponto e vírgula

O `;` não é apenas pontuação. Ele é o token de sincronização usado na
recuperação de erros sintáticos: ao encontrar um erro, o parser descarta tokens
até o próximo `;` e retoma a análise no comando seguinte, o que permite
relatar vários erros em uma única execução.

Sem um delimitador explícito de fim de comando, a recuperação teria de adivinhar
onde um comando termina e o próximo começa.

### Palavras-chave são reservadas

As palavras `imagem`, `inteiro`, `redimensionar`, `para`, `por`,
`tons_de_cinza`, `rotacionar`, `recortar`, `de`, `tamanho`, `salvar` e `como`
são reservadas e não podem ser usadas como nomes de variáveis.

A consequência é aceita conscientemente: uma variável não pode se chamar `de`
nem `tamanho`. O ganho é que o scanner reconhece cada palavra-chave por uma
regra própria, colocada antes da regra de identificador, sem necessidade de
palavras-chave sensíveis a contexto.

## Exemplos de erros

### Erro léxico

```text
imagem @foto = "foto.jpg";
```

O caractere `@` não pertence ao vocabulário inicial da linguagem.

### Erro sintático

```text
salvar como foto "resultado.jpg";
```

Os elementos são reconhecíveis, mas estão em uma ordem não prevista pela
gramática.

### Erro semântico

```text
inteiro x = 10;
tons_de_cinza x;
```

O comando `tons_de_cinza` exige uma variável do tipo `imagem`.
