# ImagemLang

Documentação publicada: <https://verabelucia.github.io/Compiladores-Grupo-14/>

ImagemLang é uma linguagem específica de domínio (DSL) para processamento de
imagens. O projeto será desenvolvido na disciplina FGA0003 - Compiladores 1 e
terá C como linguagem-alvo.

Um programa ImagemLang descreve operações de imagem em alto nível:

```text
imagem foto = "foto.jpg";

redimensionar foto para 750 por 800;
tons_de_cinza foto;
rotacionar foto 90;

salvar foto como "resultado.jpg";
```

O compilador deverá transformar esse programa em código C. O código gerado
utilizará um pequeno runtime próprio, construído sobre as bibliotecas
header-only `stb_image`, `stb_image_write` e `stb_image_resize2`.

## Objetivo acadêmico

O projeto percorrerá as fases tradicionais estudadas na disciplina:

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

A primeira versão deverá oferecer:

- declaração e carregamento de imagens;
- declaração de valores inteiros simples;
- redimensionamento;
- conversão para tons de cinza;
- rotação;
- recorte;
- salvamento.

Não fazem parte do escopo inicial:

- funções;
- estruturas condicionais;
- laços de repetição;
- expressões aritméticas completas;
- filtros definidos pelo usuário;
- otimizações avançadas.

## Decisões de projeto já tomadas

Estas decisões foram fechadas e não devem ser reabertas sem motivo técnico.
O raciocínio completo está em [docs/linguagem.md](docs/linguagem.md).

- **Comandos mutam a imagem; não há composição.** `tons_de_cinza foto;` altera
  `foto` no lugar. A forma `imagem cinza = tons_de_cinza foto;` fica como
  extensão opcional para a entrega final.
- **Dimensões usam `por`, não `x`.** `750 por 800` evita a ambiguidade léxica
  entre o separador `x` e um identificador começado por `x`.
- **Todo comando termina em `;`.** O ponto e vírgula é o token de sincronização
  usado na recuperação de erros sintáticos.
- **O runtime usa `stb`, não MagickWand.** As bibliotecas `stb` são header-only
  e ficam versionadas no próprio repositório, sem dependência de instalação.

## Documentação inicial

A documentação também está publicada como site em
<https://verabelucia.github.io/Compiladores-Grupo-14/>, gerado pelo GitHub
Pages a partir da pasta `docs/` do branch `main`.

- [Especificação da linguagem](docs/linguagem.md)
- [Tokens](docs/tokens.md)
- [Gramática preliminar](docs/gramatica.ebnf)
- [Arquitetura](docs/arquitetura.md)
- [Cronograma](docs/cronograma.md)
- [Programa de exemplo](examples/programa_basico.img)

## Estado atual

Os analisadores léxico e sintático estão implementados e cobertos por 17
testes automatizados. O parser reconhece os sete comandos do escopo mínimo,
relata erros sintáticos com linha e coluna e se recupera no `;` para relatar
vários erros numa só execução. A AST, a tabela de símbolos, a análise
semântica e o gerador de código ainda não existem.

## Como compilar e executar

Requisitos: Flex, Bison 3.6 ou mais recente, GCC e Make.

```sh
make          # gera build/imagemc
make test     # compila, se preciso, e roda a suíte de testes
make clean    # apaga a pasta build/
```

A pasta `build/` não é versionada: ela guarda o `parser.tab.c` e o
`parser.tab.h` gerados pelo Bison, o `lex.yy.c` gerado pelo Flex e o
executável `imagemc`, todos reconstruíveis a partir de `src/`.

### Executando o compilador

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

### Suíte de testes

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

## Decisões que precisam ser confirmadas com o professor

- aceitação de C como linguagem-alvo;
- aceitação de uma representação intermediária própria antes da geração de C;
- data exata para disponibilizar a entrega final no repositório.
