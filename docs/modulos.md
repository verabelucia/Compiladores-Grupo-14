---
title: "Módulos e contratos"
parent: Compilador
nav_order: 2
description: "Interfaces entre os módulos, formato exato de cada saída e o catálogo de erros semânticos."
---

Esta página registra as interfaces entre os módulos do compilador, combinadas
pelo grupo para o P2 (semanas 06, 07 e 08). O objetivo é que cada pessoa
trabalhe no seu módulo em paralelo, sem esperar a outra e sem editar o
arquivo de outra pessoa.

Os arquivos `.h` são a fonte da verdade das assinaturas. Esta página
acrescenta o que um `.h` não diz: o texto exato de cada saída e de cada
mensagem de erro, de que os testes dependem.

## Fluxo do compilador

```text
programa.img
   │  yylex()            src/scanner.l     (pronto)
   ▼
tokens
   │  yyparse()          src/parser.y      (pronto)
   ▼
AST (raiz_ast)           src/ast.c         (pronto)
   │  analisar_semantica src/semantico.c   (pronto)
   │    └ tabela_*       src/tabela.c      (pronto)
   ▼
AST validada + tabela de símbolos
   │  gerar_ri           src/ri.c          (pronto)
   ▼
RI (lista de instruções)
```

O `src/main.c` já liga as fases: a semântica só roda se não houve erro léxico
nem sintático, e com qualquer erro o compilador imprime só as mensagens e o
resumo.

## Quem é dono de qual arquivo

| Papel | Arquivos | Situação |
|---|---|---|
| Pessoa 1: AST e parser | `src/ast.h`, `src/ast.c`, `src/parser.y` | **pronto** |
| Pessoa 2: tabela de símbolos | `src/tabela.h`, `src/tabela.c` | **pronto** |
| Pessoa 3: análise semântica | `src/semantico.h`, `src/semantico.c` | **pronto** |
| Pessoa 4: RI e testes | `src/ri.h`, `src/ri.c`, `tests/semantico/`, `tests/intermediario/` | **pronto** |
| Compartilhados | `src/main.c`, `Makefile`, `run_tests.sh` | **prontos**; não precisam mudar no P2 |

Regras:

- Ninguém edita o arquivo de outra pessoa. Quem precisa de algo de outro
  módulo usa só o `.h` dele.
- As assinaturas dos `.h` estão congeladas. Mudar alguma exige combinar antes
  com quem a usa.
- Uma branch por pessoa (`feat/tabela`, `feat/semantico`, `feat/ri`). Antes
  de juntar na `main`: `make clean && make test` com tudo verde.
- Commits no padrão `tipo(escopo): descrição`, por exemplo
  `feat(tabela): insercao e busca de simbolos`.

## Modos do compilador

| Comando | Imprime quando não há erro |
|---|---|
| `imagemc prog.img` | só `analise concluida sem erros.` |
| `imagemc --tokens prog.img` | a sequência de tokens (só a análise léxica) |
| `imagemc --ast prog.img` | a AST |
| `imagemc --tabela prog.img` | a tabela de símbolos (Pessoa 2) |
| `imagemc --ri prog.img` | a representação intermediária (Pessoa 4) |

Nos modos `--ast`, `--tabela` e `--ri`, depois da estrutura vem uma linha em
branco e `analise concluida sem erros.`. Com erro, a saída é:

```saida
<mensagens de erro, em stderr>

analise concluida com N erro(s): L lexico(s), S sintatico(s), M semantico(s).
```

## AST (pronta)

Descrita na [arquitetura](arquitetura.md#ast) e em `src/ast.h`. Para as
mensagens de erro, a AST oferece `palavra_chave_no(cmd->tipo)`, que devolve
`"tons_de_cinza"`, `"recortar"`..., e `rotulo_arg(cmd->tipo, i)`, que devolve
`"largura"`, `"angulo"`, `"x"`...

## Tabela de símbolos (Pessoa 2)

Contrato: `src/tabela.h`. A linguagem não tem escopos (não há funções nem
blocos), então uma lista encadeada na ordem das declarações basta.

**Formato do `--tabela`**, para o programa de
`tests/sintatico/validos/01-programa-completo.img`:

```saida
TABELA DE SIMBOLOS (2 simbolos)
|-- foto: imagem [2:8]
`-- largura: inteiro = 750 [3:9]

analise concluida sem erros.
```

- Um símbolo por linha, na ordem de declaração, no mesmo estilo de árvore da
  AST (`|-- ` e, no último, `` `-- ``).
- Imagem: `nome: imagem [L:C]`. Inteiro: `nome: inteiro = VALOR [L:C]`. A
  posição é a do nome na declaração.
- Cabeçalho: `(1 simbolo)` no singular; `(0 simbolos)` para a tabela vazia.

## Análise semântica (Pessoa 3)

Contrato: `src/semantico.h`. O percurso da AST já está pronto em
`src/semantico.c`:

1. declarações vão para `tabela_declarar`;
2. o alvo de cada comando passa por `tabela_usar` e depois por
   `verificar_alvo`;
3. cada argumento que é variável passa por `tabela_usar`, e todo argumento
   passa por `verificar_argumento`.

`verificar_alvo` e `verificar_argumento` fazem a checagem de tipos e de
domínio descrita abaixo.

### Catálogo de erros semânticos

Todos saem por `erro_semantico(pos, formato, ...)`, já pronta, no formato
`arquivo:L:C: erro semantico: <mensagem>`.

| # | Situação | Quem relata | Posição | Formato (`printf`) |
|---|---|---|---|---|
| 1 | nome usado sem declaração (alvo ou argumento) | tabela (`tabela_usar`) | do uso | `'%s' nao foi declarada` |
| 2 | nome declarado de novo, com qualquer tipo | tabela (`tabela_declarar`) | do nome na 2ª declaração | `'%s' ja foi declarada na linha %d` |
| 3 | comando de imagem aplicado a um inteiro | semântica (`verificar_alvo`) | do alvo | `'%s' espera imagem, mas '%s' e inteiro` (palavra-chave, nome) |
| 4 | argumento que é variável do tipo imagem | semântica (`verificar_argumento`) | do argumento | `%s deve ser inteiro, mas '%s' e imagem` (rótulo, nome) |
| 5 | largura ou altura ≤ 0 (`redimensionar`, `recortar`) | semântica (`verificar_argumento`) | do argumento | `%s deve ser maior que zero, mas vale %ld` (rótulo, valor) |
| 6 | ângulo fora de {90, 180, 270} | semântica (`verificar_argumento`) | do argumento | `angulo deve ser 90, 180 ou 270, mas vale %ld` |
| 7 | coordenada `x` ou `y` < 0 (`recortar`) | semântica (`verificar_argumento`) | do argumento | `%s nao pode ser negativo, mas vale %ld` (rótulo, valor) |

Regras para não gerar erros em cascata:

- Se o nome não foi declarado (erro 1), não há mais verificações sobre ele.
- O domínio (erros 5, 6 e 7) usa o número do literal ou, para uma variável
  inteira, o `valor` guardado na tabela. Isso funciona porque a linguagem não
  tem atribuição: todo `inteiro` é constante. Se a variável não existe ou é
  imagem, o domínio não é verificado.
- O erro 7 hoje não ocorre: a linguagem não tem números negativos, e zero
  é válido. A verificação existe para manter a regra caso eles sejam
  introduzidos.
- Se uma declaração repetida for recusada (erro 2), o símbolo original
  continua valendo.

### Exemplo completo

Entrada:

```imagemlang
imagem foto = "foto.jpg";
inteiro angulo = 45;
tons_de_cinza angulo;
rotacionar foto angulo;
redimensionar foto para 0 por 600;
salvar fot como "saida.jpg";
imagem foto = "outra.jpg";
recortar foto de 0, 0 tamanho foto por 10;
```

Saída esperada (`imagemc --tabela erros.img`):

```saida
erros.img:3:15: erro semantico: 'tons_de_cinza' espera imagem, mas 'angulo' e inteiro
erros.img:4:17: erro semantico: angulo deve ser 90, 180 ou 270, mas vale 45
erros.img:5:25: erro semantico: largura deve ser maior que zero, mas vale 0
erros.img:6:8: erro semantico: 'fot' nao foi declarada
erros.img:7:8: erro semantico: 'foto' ja foi declarada na linha 1
erros.img:8:31: erro semantico: largura deve ser inteiro, mas 'foto' e imagem

analise concluida com 6 erro(s): 0 lexico(s), 0 sintatico(s), 6 semantico(s).
```

As posições foram conferidas com `imagemc --ast` sobre a mesma entrada.

## Representação intermediária (Pessoa 4)

Contrato: `src/ri.h`. A RI tem uma instrução por comando, na ordem do
programa, e no fim um `FREE` para cada imagem, na ordem de declaração. O
`FREE` vira `img_destroy` no gerador de C.

| Instrução | Comando de origem | Texto |
|---|---|---|
| `RI_LOAD_IMAGE` | `imagem foto = "foto.jpg";` | `LOAD_IMAGE "foto.jpg" -> foto` |
| `RI_CONST` | `inteiro largura = 750;` | `CONST 750 -> largura` |
| `RI_RESIZE` | `redimensionar foto para L por A;` | `RESIZE foto, L, A` |
| `RI_GRAYSCALE` | `tons_de_cinza foto;` | `GRAYSCALE foto` |
| `RI_ROTATE` | `rotacionar foto G;` | `ROTATE foto, G` |
| `RI_CROP` | `recortar foto de X, Y tamanho L por A;` | `CROP foto, X, Y, L, A` |
| `RI_SAVE` | `salvar foto como "x.png";` | `SAVE foto, "x.png"` |
| `RI_FREE` | (fim do programa) | `FREE foto` |

Argumentos que são variáveis aparecem **pelo nome** (`RESIZE foto, largura,
800`). Trocar o nome pelo valor (propagação de constantes) fica para a
otimização da semana 09.

**Formato do `--ri`**, para `tests/sintatico/validos/01-programa-completo.img`:

```saida
RI (8 instrucoes)
   1  LOAD_IMAGE "foto.jpg" -> foto
   2  CONST 750 -> largura
   3  RESIZE foto, largura, 800
   4  GRAYSCALE foto
   5  ROTATE foto, 90
   6  CROP foto, 10, 20, 300, 200
   7  SAVE foto, "resultado.jpg"
   8  FREE foto

analise concluida sem erros.
```

- Cada linha: número da instrução com `%4d`, dois espaços e o texto.
- Cabeçalho: `(1 instrucao)` no singular; `(0 instrucoes)` para um programa
  vazio.

## Testes

O `run_tests.sh` já reconhece as pastas novas e escolhe o modo pela fase:

| Pasta | Modo |
|---|---|
| `tests/lexico/{validos,invalidos}/` | `--tokens` |
| `tests/sintatico/{validos,invalidos}/` | `--ast` |
| `tests/semantico/{validos,invalidos}/` | `--tabela` |
| `tests/intermediario/validos/` | `--ri` |

Para criar um caso: `NOME.img` e, ao lado, `NOME.esperado` com a saída
exata. Os exemplos desta página (a tabela, o catálogo de erros e a RI) podem
ser usados como os primeiros casos. Antes de juntar na `main`, rode também o
AddressSanitizer:

```sh
gcc -Wall -Wextra -g -fsanitize=address,undefined -Isrc -Ibuild \
    -o build/asan build/parser.tab.c build/lex.yy.c src/*.c
```
