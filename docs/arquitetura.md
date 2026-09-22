---
layout: default
title: "Arquitetura"
nav_order: 5
---

## Fluxo do compilador

```text
Arquivo .img
    ↓
Lexer - Flex
    ↓
Tokens
    ↓
Parser - Bison
    ↓
AST
    ↓
Tabela de símbolos e análise semântica
    ↓
Representação intermediária
    ↓
Gerador de código C
    ↓
Arquivo .c
    ↓
GCC + runtime
    ↓
Executável
```

## Componentes planejados

### Lexer

Reconhece tokens, ignora espaços e comentários e informa erros léxicos com
linha e coluna.

### Parser

Verifica se a sequência de tokens pertence à gramática e cria a AST.

Implementado em `src/parser.y` (Bison), com uma regra por comando da
[gramática](gramatica.md). Até a AST existir (semana 06), cada comando
reconhecido é impresso com sua linha e coluna. `valor_inteiro` aceita tanto
um literal quanto um identificador; checar se o identificador é mesmo um
`inteiro` fica para a análise semântica.

Erros sintáticos:

- **Mensagens:** com `%define parse.error custom`, a função
  `yyreport_syntax_error` monta mensagens em português com o token encontrado
  e os esperados: `encontrado 'como', esperado identificador`. Onde há mais de
  cinco alternativas, o que nesta gramática só acontece no início de um
  comando, a mensagem diz `esperado inicio de comando`.
- **Recuperação:** a regra `comando: error ';' { yyerrok; }` descarta tokens
  até o próximo `;` e retoma no comando seguinte. Um programa com três erros
  em linhas diferentes gera três mensagens, e os comandos corretos entre eles
  continuam sendo reconhecidos.
- **Limitação conhecida:** uma string não terminada engole o restante da
  linha, inclusive o `;`. O erro léxico vem acompanhado de um erro sintático
  em cascata na linha seguinte. O teste
  `tests/sintatico/invalidos/08-string-nao-terminada` registra esse
  comportamento.

### AST

Terá inicialmente os seguintes tipos de nó:

```text
PROGRAM
IMAGE_DECL
INTEGER_DECL
RESIZE
GRAYSCALE
ROTATE
CROP
SAVE
```

Cada nó conservará sua posição no código-fonte para permitir mensagens de erro
claras.

### Tabela de símbolos e análise semântica

Registrarão o nome e o tipo de cada variável. As verificações mínimas serão:

- uso antes da declaração;
- redeclaração;
- operação de imagem aplicada a um inteiro;
- argumentos inteiros inválidos;
- dimensões não positivas;
- coordenadas negativas;
- ângulo de rotação não permitido.

Limites de recorte que dependam das dimensões reais da imagem serão verificados
pelo runtime durante a execução.

### Representação intermediária

Uma forma textual inicial poderá ser:

```text
LOAD_IMAGE "foto.jpg" -> foto
RESIZE foto, 750, 800
GRAYSCALE foto
ROTATE foto, 90
SAVE foto, "resultado.jpg"
```

### Gerador de C

Percorrerá a representação intermediária e emitirá chamadas para uma API
pequena e estável:

```text
img_load
img_resize
img_grayscale
img_rotate
img_crop
img_save
img_destroy
```

### Runtime

O runtime esconderá os detalhes da biblioteca de imagens. A implementação usará
as bibliotecas header-only do conjunto `stb`:

- `stb_image.h` para leitura de arquivos;
- `stb_image_write.h` para gravação;
- `stb_image_resize2.h` para redimensionamento.

Os arquivos serão versionados no próprio repositório. A escolha substitui
MagickWand, que exigiria instalar `libmagickwand-dev` em cada máquina e
introduziria uma dependência externa capaz de impedir a compilação durante uma
demonstração. Com `stb`, o projeto compila com `gcc` e `make` em um ambiente
que tenha apenas Flex, Bison e um compilador C.

As operações de tons de cinza, rotação em múltiplos de 90 graus e recorte serão
implementadas diretamente no runtime, percorrendo o vetor de pixels. São
algoritmos curtos, e mantê-los sob nosso controle preserva a separação entre o
compilador e a biblioteca de imagens.

## Tratamento de erros

O formato é:

```text
arquivo.img:linha:coluna: categoria: mensagem
```

As categorias `erro lexico` e `erro sintatico` já estão implementadas:

```text
programa.img:1:8: erro lexico: caractere invalido: '@'
programa.img:2:8: erro sintatico: encontrado 'como', esperado identificador
```

A categoria semântica virá com a análise semântica:

```text
programa.img:3:16: erro semantico: 'tons_de_cinza' espera imagem, mas 'x' e inteiro
```

Ao final, o compilador resume a contagem por categoria e sai com código `1`
se houve qualquer erro.
