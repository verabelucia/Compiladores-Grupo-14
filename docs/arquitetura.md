# Arquitetura inicial

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

O formato pretendido é:

```text
arquivo.img:linha:coluna: categoria: mensagem
```

Exemplo:

```text
programa.img:3:16: erro semântico: 'tons_de_cinza' espera imagem, mas 'x' é inteiro
```
