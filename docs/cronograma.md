---
title: "Cronograma"
parent: Projeto
nav_order: 2
description: "Datas da disciplina, metas de cada ponto de controle e problemas resolvidos no caminho."
---

Este planejamento associa a ImagemLang ao plano de ensino da disciplina. As
entregas sugeridas são metas da equipe e poderão ser ajustadas após orientação
do professor.

| Data | Atividade da disciplina | Meta da ImagemLang | Situação |
|---|---|---|---|
| 26/08 | Projeto inicial: fase léxica | Fechar escopo, sintaxe e tokens | ✅ concluído |
| 02/09 | Implementação inicial do parser | Implementar a primeira gramática no Bison | ✅ concluído |
| 09/09 | Parser e erros sintáticos | Reconhecer programas válidos e recuperar erros em `;` | ✅ concluído |
| 16/09 | AST e tabela de símbolos | Construir a AST e registrar declarações | ✅ concluído |
| 23/09, 23h59 | Formulário P1 | Líder registra o progresso da equipe | ✅ concluído |
| 30/09 (equipes 9 a 16) | P1 | Apresentar proposta, lexer, parser e AST inicial | ✅ concluído |
| 07/10 | Análise semântica | Implementar declarações, tipos e validação de argumentos | ✅ concluído |
| 14/10 | Código intermediário | Gerar instruções intermediárias da DSL | ✅ concluído |
| 21/10 | Otimização | Avaliar uma otimização simples, sem ampliar o escopo | ✅ concluído |
| 28/10 | Código final | Gerar a primeira versão de C | ✅ concluído |
| 04/11, 23h59 | Formulário P2 | Líder registra a evolução desde o P1 | ✅ concluído |
| 09/11 (equipes 16 a 9) | P2 | Demonstrar um protótipo quase ponta a ponta | ✅ concluído |
| 16/11 a 25/11 | Implementação final | Integrar runtime, testes e documentação | ✅ concluído |
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
| O prólogo `%{ %}` do Bison é copiado antes do `%code requires`, então o tipo `NoAST` ainda não existia onde `raiz_ast` era declarada. | `ast.h` incluído também no prólogo do `parser.y`. |
| Trocar o eco provisório dos comandos pela AST mudaria a saída de todos os testes sintáticos. | Saída padrão passou a ser só o resumo; a árvore sai com `--ast`, e os testes sintáticos passaram a conferir a árvore. |
| As verificações de declaração e de tipos iam ficar no mesmo `semantico.c`. | As verificações de declaração ficaram no `tabela.c`, e a função `erro_semantico` é compartilhada (ver [Módulos e contratos](modulos.md)). |
| A tabela de símbolos só podia ser testada quando a semântica a chamasse, o que impediria desenvolver as duas em paralelo. | No Dia 1, o percurso da AST que chama a tabela já foi entregue no `semantico.c`; a semântica ficou só com as verificações de tipo e domínio. |
| Na integração, o `--ri` imprimia uma linha em branco a mais que o combinado no contrato. | Corrigido no `ri.c`; os `.esperado` da RI foram ajustados ao formato do contrato. |
| A `stb_image_resize2.h` saiu do repositório `nothings/stb` em outubro de 2026, e o download passou a dar erro 404. | Cópia obtida do novo repositório oficial, `nothings/stb_image_resize`, com a versão registrada em [Runtime e executável](runtime.md). |
| A checagem `x + largura <= largura da imagem` do recorte pode estourar o `int` com valores muito grandes. | A mesma condição foi escrita com subtração, que não estoura. |
| PNG em tons de cinza tem 1 ou 2 canais, e cada operação do runtime precisaria tratar esses casos. | `img_load` converte tudo para RGB ou RGBA, e as operações só tratam 3 ou 4 canais. |
| Ao remover um `RI_CONST` sem uso, a lista da RI podia ficar com o ponteiro do fim apontando para uma instrução já liberada (caso de um programa só com inteiros). | O otimizador ajusta `primeira`, `ultima` e `quantidade` ao remover; o caso tem teste próprio e roda limpo com AddressSanitizer. |
| Na validação final, o `--c` não passava pelo otimizador: o C gerado mantinha `int im_largura = 750;`, e uma constante sem uso virava variável C não usada (aviso do GCC). | O `main.c` chama `otimizar_ri` antes de `gerar_c`; novo teste `tests/codegen/validos/05-constantes`. Todo C gerado dos programas válidos compila com `-Wall -Wextra -Werror`. |
| Quando uma operação falhava em execução (recorte fora da imagem, extensão inválida), o programa gerado saía com `return 1` sem liberar as imagens, e o AddressSanitizer acusava vazamento. | O gerador de C libera todas as imagens já carregadas antes de sair por erro; novo teste `tests/codegen/validos/07-varias-imagens`. |
| O `make demo` dependia de copiar uma `foto.jpg` à mão para a raiz do repositório e não funcionava num clone limpo. | O `Makefile` usa a foto da documentação quando não há `foto.jpg` e roda a demonstração dentro de `build/`; `foto.jpg` e `resultado.jpg` na raiz ficam no `.gitignore`. |
