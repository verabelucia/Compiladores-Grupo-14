---
layout: default
title: Início
nav_order: 1
permalink: /
description: "ImagemLang: uma DSL em português para processar imagens, compilada para C. FGA0003 Compiladores 1, UnB, Grupo 14."
---

<section class="il-hero">
  <span class="il-badge">FGA0003 Compiladores 1 · UnB · Grupo 14</span>
  <h1>ImagemLang</h1>
  <p class="il-tagline">Uma linguagem específica de domínio, escrita em português, para descrever transformações de imagens. O compilador verifica o programa e gera código C.</p>
  <div class="il-actions">
    <a class="btn btn-primary" href="{{ '/linguagem.html' | relative_url }}">Conhecer a linguagem</a>
    <a class="btn btn-ghost" href="{{ '/desenvolvimento.html' | relative_url }}">Compilar e testar</a>
    <a class="btn btn-ghost" href="https://github.com/verabelucia/Compiladores-Grupo-14" target="_blank" rel="noopener">Código no GitHub</a>
  </div>
  <div class="il-code-window">
    <div class="il-code-bar"><i></i><i></i><i></i><span>examples/programa_basico.img</span></div>
<pre><code class="language-imagemlang">// Carrega a imagem original.
imagem foto = "foto.jpg";

// Aplica as transformações.
redimensionar foto para 750 por 800;
tons_de_cinza foto;
rotacionar foto 90;
recortar foto de 10, 20 tamanho 300 por 200;

// Grava o resultado.
salvar foto como "resultado.jpg";</code></pre>
  </div>
</section>

<div class="il-stats">
  <div><strong>7</strong><span>comandos no escopo mínimo</span></div>
  <div><strong>5 de 8</strong><span>fases do compilador prontas</span></div>
  <div><strong>17</strong><span>testes automatizados</span></div>
  <div><strong>3</strong><span>categorias de erro com linha e coluna</span></div>
</div>

## Estado do compilador

Cada fase recebe o resultado da anterior. O programa só segue para a próxima
fase se não houver erro; com erro, o compilador lista todos os problemas
encontrados e para.

<ol class="il-pipeline">
  <li class="il-ok"><div class="il-fase"><strong>Análise léxica</strong><span>Flex transforma o texto em tokens, com linha e coluna.</span></div><span class="il-status il-ok">pronto</span></li>
  <li class="il-ok"><div class="il-fase"><strong>Análise sintática</strong><span>Bison valida a ordem dos tokens e se recupera no <code>;</code>.</span></div><span class="il-status il-ok">pronto</span></li>
  <li class="il-ok"><div class="il-fase"><strong>AST</strong><span>Árvore sintática abstrata do programa (<code>--ast</code>).</span></div><span class="il-status il-ok">pronto</span></li>
  <li class="il-ok"><div class="il-fase"><strong>Tabela de símbolos</strong><span>Declarações, redeclarações e nomes não declarados (<code>--tabela</code>).</span></div><span class="il-status il-ok">pronto</span></li>
  <li class="il-ok"><div class="il-fase"><strong>Análise semântica</strong><span>Tipos e domínio: dimensões &gt; 0, ângulos 90, 180 ou 270.</span></div><span class="il-status il-ok">pronto</span></li>
  <li class="il-wip"><div class="il-fase"><strong>Representação intermediária</strong><span>Uma instrução linear por comando (<code>--ri</code>).</span></div><span class="il-status il-wip">em andamento</span></li>
  <li><div class="il-fase"><strong>Geração de código C</strong><span>Chamadas a uma API pequena: <code>img_load</code>, <code>img_resize</code>…</span></div><span class="il-status il-plan">planejado</span></li>
  <li><div class="il-fase"><strong>Runtime e executável</strong><span>GCC + bibliotecas header-only <code>stb</code>.</span></div><span class="il-status il-plan">planejado</span></li>
</ol>

## Comandos da linguagem

Cada comando altera a imagem no lugar e termina em `;`. Os argumentos
numéricos podem ser números ou variáveis `inteiro`.

<div class="il-grid">
  <div class="il-card"><h4>Declarar imagem</h4><p>Carrega uma imagem de um arquivo.</p><code class="il-card-ex language-imagemlang">imagem foto = "foto.jpg";</code></div>
  <div class="il-card"><h4>Declarar inteiro</h4><p>Constante usada como argumento.</p><code class="il-card-ex language-imagemlang">inteiro angulo = 90;</code></div>
  <div class="il-card"><h4>Redimensionar</h4><p>Largura e altura maiores que zero.</p><code class="il-card-ex language-imagemlang">redimensionar foto para 750 por 800;</code></div>
  <div class="il-card"><h4>Tons de cinza</h4><p>Converte a imagem para escala de cinza.</p><code class="il-card-ex language-imagemlang">tons_de_cinza foto;</code></div>
  <div class="il-card"><h4>Rotacionar</h4><p>Só 90, 180 ou 270 graus.</p><code class="il-card-ex language-imagemlang">rotacionar foto 90;</code></div>
  <div class="il-card"><h4>Recortar</h4><p>Posição <code>x, y</code> e tamanho do recorte.</p><code class="il-card-ex language-imagemlang">recortar foto de 10, 20 tamanho 300 por 200;</code></div>
  <div class="il-card"><h4>Salvar</h4><p>Grava o resultado em um arquivo.</p><code class="il-card-ex language-imagemlang">salvar foto como "resultado.jpg";</code></div>
</div>

## Erros claros

As três categorias de erro seguem o mesmo formato,
`arquivo:linha:coluna: categoria: mensagem`. Dentro de cada fase, o
compilador relata todos os erros de uma vez, sem parar no primeiro:

```saida
erros.img:3:15: erro semantico: 'tons_de_cinza' espera imagem, mas 'angulo' e inteiro
erros.img:4:17: erro semantico: angulo deve ser 90, 180 ou 270, mas vale 45
erros.img:5:25: erro semantico: largura deve ser maior que zero, mas vale 0
erros.img:6:8: erro semantico: 'fot' nao foi declarada

analise concluida com 4 erro(s): 0 lexico(s), 0 sintatico(s), 4 semantico(s).
```

A análise semântica só roda quando não há erro léxico nem sintático. Exemplos
das outras duas categorias:

```saida
programa.img:1:8: erro lexico: caractere invalido: '@'
programa.img:2:8: erro sintatico: encontrado 'como', esperado identificador
```

## Decisões de projeto

Estas decisões estão fechadas e só devem ser reabertas por motivo técnico. O
raciocínio completo está na [especificação da linguagem](linguagem.md).

<div class="il-grid il-grid-2">
  <div class="il-card"><h4>Comandos mutam a imagem</h4><p><code>tons_de_cinza foto;</code> altera <code>foto</code> no lugar. A composição (<code>imagem cinza = tons_de_cinza foto;</code>) fica como extensão opcional.</p></div>
  <div class="il-card"><h4>Dimensões usam <code>por</code></h4><p><code>750 por 800</code> evita a ambiguidade léxica entre o separador <code>x</code> e um identificador que comece com <code>x</code>.</p></div>
  <div class="il-card"><h4>Todo comando termina em <code>;</code></h4><p>O ponto e vírgula é o ponto de sincronização da recuperação de erros sintáticos.</p></div>
  <div class="il-card"><h4>Runtime com <code>stb</code></h4><p>Bibliotecas header-only versionadas no repositório: só Flex, Bison, GCC e Make para compilar.</p></div>
</div>

## Escopo

<div class="il-grid il-grid-2 il-scope">
  <div class="il-card il-in"><h4>Dentro da primeira versão</h4><ul><li>declaração e carregamento de imagens</li><li>valores inteiros simples</li><li>redimensionamento</li><li>tons de cinza</li><li>rotação</li><li>recorte</li><li>salvamento</li></ul></div>
  <div class="il-card il-out"><h4>Fora do escopo inicial</h4><ul><li>funções</li><li>condicionais e laços</li><li>expressões aritméticas</li><li>filtros definidos pelo usuário</li><li>otimizações avançadas</li><li>interface gráfica</li></ul></div>
</div>

{: .atencao }
A confirmar com o professor: C como linguagem-alvo, uma representação
intermediária própria antes da geração de C e a data exata da entrega final
no repositório.

## Explore a documentação

<div class="il-grid">
  <a class="il-card" href="{{ '/secoes/linguagem/' | relative_url }}"><span class="il-eyebrow">Linguagem</span><h3>Especificação, tokens e gramática</h3><p>O que a ImagemLang aceita e por quê.</p><span class="il-card-more">Abrir →</span></a>
  <a class="il-card" href="{{ '/secoes/compilador/' | relative_url }}"><span class="il-eyebrow">Compilador</span><h3>Arquitetura e contratos</h3><p>Fases, módulos e o formato de cada saída.</p><span class="il-card-more">Abrir →</span></a>
  <a class="il-card" href="{{ '/secoes/projeto/' | relative_url }}"><span class="il-eyebrow">Projeto</span><h3>Compilar, testar e cronograma</h3><p>Como rodar e o andamento do grupo.</p><span class="il-card-more">Abrir →</span></a>
</div>
