<div align="center">

# ImagemLang

**Uma linguagem em português para descrever transformações de imagens, compilada para C.**

[![Documentação](https://img.shields.io/badge/docs-verabelucia.github.io-5b6cf0?style=flat-square)](https://verabelucia.github.io/Compiladores-Grupo-14/)
![Linguagem-alvo](https://img.shields.io/badge/alvo-C-555?style=flat-square&logo=c)
![Flex](https://img.shields.io/badge/léxico-Flex-555?style=flat-square)
![Bison](https://img.shields.io/badge/sintático-Bison%203.6%2B-555?style=flat-square)
![Disciplina](https://img.shields.io/badge/FGA0003-Compiladores%201%20·%20UnB-0b6e4f?style=flat-square)

[Documentação](https://verabelucia.github.io/Compiladores-Grupo-14/) ·
[Especificação](docs/linguagem.md) ·
[Arquitetura](docs/arquitetura.md) ·
[Como compilar](#começando)

</div>

---

## Visão geral

A ImagemLang é uma linguagem específica de domínio (DSL) para processamento de
imagens, desenvolvida pelo Grupo 14 na disciplina FGA0003 - Compiladores 1 da
Universidade de Brasília. O compilador `imagemc` verifica o programa e gera
código C, que usa um pequeno runtime próprio construído sobre as bibliotecas
header-only `stb_image`, `stb_image_write` e `stb_image_resize2`.

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

## Estado do compilador

| # | Fase | Arquivos | Situação |
|:-:|---|---|:-:|
| 1 | Análise léxica (Flex) | `src/scanner.l` | ✅ pronto |
| 2 | Análise sintática (Bison) | `src/parser.y` | ✅ pronto |
| 3 | Árvore sintática abstrata | `src/ast.h`, `src/ast.c` | ✅ pronto |
| 4 | Tabela de símbolos | `src/tabela.h`, `src/tabela.c` | ✅ pronto |
| 5 | Análise semântica | `src/semantico.h`, `src/semantico.c` | ✅ pronto |
| 6 | Representação intermediária | `src/ri.h`, `src/ri.c` | ✅ pronto |
| 7 | Geração de código C | | ⏳ planejado |
| 8 | Runtime (`stb`) e executável | | ⏳ planejado |

As três categorias de erro (léxico, sintático e semântico) saem com arquivo,
linha e coluna, e o compilador relata todos os erros de uma fase de uma vez:

```text
erros.img:3:15: erro semantico: 'tons_de_cinza' espera imagem, mas 'angulo' e inteiro
erros.img:4:17: erro semantico: angulo deve ser 90, 180 ou 270, mas vale 45
erros.img:6:8: erro semantico: 'fot' nao foi declarada

analise concluida com 3 erro(s): 0 lexico(s), 0 sintatico(s), 3 semantico(s).
```

## Começando

**Requisitos:** Flex, Bison 3.6 ou mais recente, GCC e Make.

| Sistema | Instalação |
|---|---|
| Ubuntu 22.04+ / Debian 12 / WSL | `sudo apt install flex bison gcc make` |
| Fedora | `sudo dnf install flex bison gcc make` |
| macOS | `brew install bison flex` e `export PATH="$(brew --prefix bison)/bin:$PATH"` |

> [!WARNING]
> O Ubuntu 20.04 traz o Bison 3.5, que não serve, e o `/usr/bin/bison` do
> macOS é o 2.3. Com Bison antigo, o `make` para logo no início com
> `require bison 3.6`.

```sh
make          # gera build/imagemc
make test     # compila, se preciso, e roda a suíte de testes
make clean    # apaga a pasta build/
```

### Usando o compilador

```sh
./build/imagemc examples/programa_basico.img
```

| Modo | Imprime quando não há erro |
|---|---|
| `imagemc prog.img` | só `analise concluida sem erros.` |
| `imagemc --tokens prog.img` | a sequência de tokens (só a análise léxica) |
| `imagemc --ast prog.img` | a árvore sintática abstrata |
| `imagemc --tabela prog.img` | a tabela de símbolos |
| `imagemc --ri prog.img` | a representação intermediária |

Códigos de saída: `0` sem erros, `1` com qualquer erro léxico, sintático ou
semântico, `2` para uso incorreto ou arquivo inexistente.

### Testes

Cada `tests/<fase>/<grupo>/<nome>.img` tem ao lado um `<nome>.esperado` com a
saída exata do compilador; o `run_tests.sh` compara as duas. Hoje são 30
casos, cobrindo as fases léxica, sintática, semântica e a representação
intermediária. Detalhes em
[Como compilar e testar](https://verabelucia.github.io/Compiladores-Grupo-14/desenvolvimento.html).

## Estrutura do repositório

```text
.
├── src/
│   ├── scanner.l        análise léxica (Flex)
│   ├── parser.y         análise sintática (Bison) e construção da AST
│   ├── ast.{h,c}        árvore sintática abstrata
│   ├── tabela.{h,c}     tabela de símbolos
│   ├── semantico.{h,c}  checagem de tipos e de domínio
│   ├── ri.{h,c}         representação intermediária
│   └── main.c           linha de comando e ligação entre as fases
├── tests/               casos .img e saídas .esperado, por fase
├── examples/            programas de exemplo
├── docs/                documentação, publicada no GitHub Pages
├── Makefile
└── run_tests.sh
```

## Decisões de projeto

- **Comandos mutam a imagem.** `tons_de_cinza foto;` altera `foto` no lugar;
  a composição (`imagem cinza = tons_de_cinza foto;`) fica como extensão
  opcional.
- **Dimensões usam `por`, não `x`.** `750 por 800` evita a ambiguidade entre o
  separador `x` e um identificador que comece com `x`.
- **Todo comando termina em `;`.** É o ponto de sincronização da recuperação
  de erros sintáticos.
- **Runtime com `stb`, não MagickWand.** Bibliotecas header-only versionadas
  no repositório: nada a instalar além do compilador.

O raciocínio completo está na [especificação da linguagem](docs/linguagem.md).

## Documentação

| Página | Conteúdo |
|---|---|
| [Especificação da linguagem](https://verabelucia.github.io/Compiladores-Grupo-14/linguagem.html) | tipos, comandos e decisões de design |
| [Tokens](https://verabelucia.github.io/Compiladores-Grupo-14/tokens.html) | palavras-chave, literais e símbolos |
| [Gramática](https://verabelucia.github.io/Compiladores-Grupo-14/gramatica.html) | a gramática em EBNF |
| [Arquitetura](https://verabelucia.github.io/Compiladores-Grupo-14/arquitetura.html) | fases do compilador e formato dos erros |
| [Módulos e contratos](https://verabelucia.github.io/Compiladores-Grupo-14/modulos.html) | interfaces, saídas e catálogo de erros semânticos |
| [Como compilar e testar](https://verabelucia.github.io/Compiladores-Grupo-14/desenvolvimento.html) | requisitos, modos e suíte de testes |
| [Cronograma](https://verabelucia.github.io/Compiladores-Grupo-14/cronograma.html) | datas, metas e problemas resolvidos |

## Contribuidores

<table>
  <tr>
    <td align="center" width="180">
      <a href="https://github.com/verabelucia">
        <img src="https://github.com/verabelucia.png?size=120" width="80" alt="Vera Lucia Bezerra da Silva"><br>
        <b>Vera Lucia</b>
      </a><br>
      <sub>dona do repositório</sub>
    </td>
    <td align="center" width="180">
      <a href="https://github.com/lucasbbranco">
        <img src="https://github.com/lucasbbranco.png?size=120" width="80" alt="Lucas Branco"><br>
        <b>Lucas Branco</b>
      </a><br>
      <sub>léxico, parser, AST e contratos entre módulos</sub>
    </td>
    <td align="center" width="180">
      <a href="https://github.com/EdsonToppzera">
        <img src="https://github.com/EdsonToppzera.png?size=120" width="80" alt="Edson Gabriel"><br>
        <b>Edson Gabriel</b>
      </a><br>
      <sub>tabela de símbolos</sub>
    </td>
    <td align="center" width="180">
      <a href="https://github.com/antonioscarvalho">
        <img src="https://github.com/antonioscarvalho.png?size=120" width="80" alt="Antonio Carvalho"><br>
        <b>Antonio Carvalho</b>
      </a><br>
      <sub>análise semântica e site da documentação</sub>
    </td>
  </tr>
</table>

---

<div align="center">
<sub>FGA0003 - Compiladores 1 · Grupo 14 · Universidade de Brasília, Faculdade do Gama</sub>
</div>
