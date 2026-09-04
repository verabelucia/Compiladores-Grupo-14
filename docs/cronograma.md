# Cronograma inicial - semestre 2026/2

Este planejamento associa a ImagemLang ao plano de ensino da disciplina. As
entregas sugeridas são metas da equipe e poderão ser ajustadas após orientação
do professor.

| Data | Atividade da disciplina | Meta da ImagemLang |
|---|---|---|
| 26/08 | Projeto inicial: fase léxica | Fechar escopo, sintaxe e tokens |
| 02/09 | Implementação inicial do parser | Implementar a primeira gramática no Bison |
| 09/09 | Parser e erros sintáticos | Reconhecer programas válidos e recuperar erros em `;` |
| 16/09 | AST e tabela de símbolos | Construir a AST e registrar declarações |
| 23/09, 23h59 | Formulário P1 | Líder registra o progresso da equipe |
| 28/09 ou 30/09 | P1 | Apresentar proposta, lexer, parser e AST inicial |
| 07/10 | Análise semântica | Implementar declarações, tipos e validação de argumentos |
| 14/10 | Código intermediário | Gerar instruções intermediárias da DSL |
| 21/10 | Otimização | Avaliar uma otimização simples, sem ampliar o escopo |
| 28/10 | Código final | Gerar a primeira versão de C |
| 04/11, 23h59 | Formulário P2 | Líder registra a evolução desde o P1 |
| 09/11 ou 11/11 | P2 | Demonstrar um protótipo quase ponta a ponta |
| 16/11 a 25/11 | Implementação final | Integrar runtime, testes e documentação |
| 30/11 ou 02/12 | Entrevista final | Demonstrar e explicar o compilador completo |

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
