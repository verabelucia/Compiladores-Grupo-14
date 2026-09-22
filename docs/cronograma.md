---
layout: default
title: "Cronograma"
nav_order: 6
---

Este planejamento associa a ImagemLang ao plano de ensino da disciplina. As
entregas sugeridas são metas da equipe e poderão ser ajustadas após orientação
do professor.

| Data | Atividade da disciplina | Meta da ImagemLang | Situação |
|---|---|---|---|
| 26/08 | Projeto inicial: fase léxica | Fechar escopo, sintaxe e tokens | ✅ concluído |
| 02/09 | Implementação inicial do parser | Implementar a primeira gramática no Bison | ✅ concluído |
| 09/09 | Parser e erros sintáticos | Reconhecer programas válidos e recuperar erros em `;` | ✅ concluído |
| 16/09 | AST e tabela de símbolos | Construir a AST e registrar declarações | ⏳ próximo |
| 23/09, 23h59 | Formulário P1 | Líder registra o progresso da equipe | ⏳ pendente |
| 30/09 (equipes 9 a 16) | P1 | Apresentar proposta, lexer, parser e AST inicial |  |
| 07/10 | Análise semântica | Implementar declarações, tipos e validação de argumentos |  |
| 14/10 | Código intermediário | Gerar instruções intermediárias da DSL |  |
| 21/10 | Otimização | Avaliar uma otimização simples, sem ampliar o escopo |  |
| 28/10 | Código final | Gerar a primeira versão de C |  |
| 04/11, 23h59 | Formulário P2 | Líder registra a evolução desde o P1 |  |
| 09/11 (equipes 16 a 9) | P2 | Demonstrar um protótipo quase ponta a ponta |  |
| 16/11 a 25/11 | Implementação final | Integrar runtime, testes e documentação |  |
| 30/11 ou 02/12 | Entrevista final | Demonstrar e explicar o compilador completo |  |

## Meta para o P1

- linguagem e escopo documentados;
- tokens definidos;
- lexer funcional;
- parser reconhecendo o escopo mínimo;
- erros léxicos e sintáticos básicos;
- AST e tabela de símbolos iniciadas;
- planejamento das próximas etapas.

## Meta para o P2

- lexer, parser e AST completos para o escopo mínimo;
- análise semântica;
- representação intermediária;
- primeira versão do gerador C;
- ao menos um exemplo executado de ponta a ponta;
- testes automatizados básicos.

## Meta para a entrega final

- compilador funcional;
- runtime integrado;
- exemplos válidos e inválidos;
- mensagens claras para as três categorias de erro;
- testes da geração e execução do C;
- instruções de compilação e uso;
- documentação das decisões e dificuldades encontradas.

## Problemas encontrados e soluções

| Problema | Solução |
|---|---|
| O lexer definia os códigos dos tokens num `enum` escrito à mão (`src/tokens.h`), que o Bison também precisaria declarar: duas fontes para o mesmo número. | Os tokens passaram a ser declarados só em `src/parser.y`; o lexer inclui o `build/parser.tab.h` gerado pelo Bison. |
| O lexer passava valores por variáveis globais (`valor_texto`, `valor_inteiro`) que o parser não enxerga. | Valores em `yylval` (`%union`) e posições em `yylloc` (`%locations`). |
| As mensagens padrão do Bison saem em inglês e sem o que era esperado (`syntax error`). | `%define parse.error custom` com apelidos para os tokens: `encontrado 'como', esperado identificador`. |
| Em arquivo sem `;` no fim, o erro de fim de arquivo apontava para a coluna do último `\n`, uma posição que não existe. | Regra `<<EOF>>` no lexer, que registra a posição real do fim do arquivo. |
| Tokens descartados na recuperação de erros vazavam as strings alocadas pelo lexer. | `%destructor { free($$); } <texto>`; os 17 testes rodam limpos com AddressSanitizer. |
| Uma string não terminada engole o `;` e causa um erro sintático em cascata. | Limitação aceita: os comandos seguintes continuam sendo analisados, e o caso tem teste próprio. |
