---
title: "Runtime e executável"
parent: Compilador
nav_order: 3
description: "Como o código C gerado vira um programa que transforma imagens de verdade, explicado passo a passo."
styles: [runtime]
scripts: [runtime]
---

<div class="rt-analogia">
  <div class="il-card"><span class="il-eyebrow">A receita</span><h4>Código C gerado</h4><p>Diz <strong>o que</strong> fazer e em que ordem: carregue, redimensione, gire, salve.</p></div>
  <div class="il-card"><span class="il-eyebrow">A cozinha</span><h4>Runtime</h4><p>Sabe <strong>como</strong> fazer cada passo e confere se ele é possível.</p></div>
  <div class="il-card"><span class="il-eyebrow">Os eletrodomésticos</span><h4>Bibliotecas stb</h4><p>Fazem o trabalho pesado: decodificar, reamostrar e gravar arquivos.</p></div>
  <div class="il-card"><span class="il-eyebrow">O prato pronto</span><h4>Executável</h4><p>Receita e cozinha juntas, em um programa que roda sozinho.</p></div>
</div>

**Runtime** é a biblioteca que acompanha o programa compilado. O compilador não
mexe em nenhum pixel: ele só escreve chamadas como `img_rotate(im_foto, 90)`.
Quem sabe girar uma imagem é o runtime.

## Do `.img` ao executável

<ol class="il-pipeline">
  <li class="il-ok"><div class="il-fase"><strong>Você escreve o programa</strong><span>Um arquivo <code>.img</code>, em português.</span></div></li>
  <li class="il-ok"><div class="il-fase"><strong>O <code>imagemc</code> gera C</strong><span>Verifica os erros e traduz cada comando para uma chamada <code>img_*</code>.</span></div></li>
  <li class="il-ok"><div class="il-fase"><strong>O GCC compila e liga</strong><span>Junta o C gerado com <code>runtime/runtime.c</code>, que inclui as três <code>stb</code>.</span></div></li>
  <li class="il-ok"><div class="il-fase"><strong>O executável roda</strong><span>Só agora a imagem é aberta, transformada e gravada.</span></div></li>
</ol>

Cada linha do programa vira uma ou duas linhas de C:

| ImagemLang | C gerado por `imagemc --c` |
|---|---|
| `imagem foto = "foto.jpg";` | `Imagem *im_foto = img_load("foto.jpg");` |
| `redimensionar foto para 750 por 800;` | `if (img_resize(im_foto, 750, 800) != 0) return 1;` |
| `tons_de_cinza foto;` | `if (img_grayscale(im_foto) != 0) return 1;` |
| `rotacionar foto 90;` | `if (img_rotate(im_foto, 90) != 0) return 1;` |
| `recortar foto de 10, 20 tamanho 300 por 200;` | `if (img_crop(im_foto, 10, 20, 300, 200) != 0) return 1;` |
| `salvar foto como "resultado.jpg";` | `if (img_save(im_foto, "resultado.jpg") != 0) return 1;` |
| *(fim do programa)* | `img_destroy(im_foto);` |

{: .nota }
Depois de cada chamada há um `if`. Se uma operação falha, o programa para ali
com código de saída `1`, sem gravar um resultado errado.

## Quem faz o quê

<div class="il-grid il-grid-2">
  <div class="il-card"><span class="il-eyebrow">Bibliotecas stb</span><h4>Formatos de arquivo e reamostragem</h4><p>Entender um JPG ou PNG e calcular pixels novos ao redimensionar são problemas grandes e já resolvidos. Usamos código de domínio público, testado há anos.</p><code class="il-card-ex">img_load · img_resize · img_save</code></div>
  <div class="il-card"><span class="il-eyebrow">Escrito pelo grupo</span><h4>Algoritmos sobre os pixels</h4><p>Cinza, rotação e recorte são curtos e percorrem o vetor de pixels diretamente, em C puro.</p><code class="il-card-ex">img_grayscale · img_rotate · img_crop</code></div>
</div>

| Arquivo | Versão | Para que serve |
|---|---|---|
| `stb_image.h` | [2.34](https://github.com/nothings/stb/tree/6b5799bae8a1fe3792691ef4b72ae26434c3e799) | ler arquivos de imagem |
| `stb_image_resize2.h` | [2.51](https://github.com/nothings/stb_image_resize/tree/a4f303e34a92d1410d510cbe85a1781ce4b8c9fd) | redimensionar |
| `stb_image_write.h` | [1.19](https://github.com/nothings/stb/tree/6b5799bae8a1fe3792691ef4b72ae26434c3e799) | gravar PNG e JPG |

Os três arquivos são cópias sem alterações, com licença MIT ou domínio público.
Eles são *header-only*: a biblioteca inteira é um `.h`, sem nada a instalar.

## Como uma imagem fica na memória

```c
typedef struct {
    unsigned char *dados;
    int largura;
    int altura;
    int canais;
} Imagem;
```

Na tela a imagem tem linhas e colunas, mas na memória ela é **uma fila única de
bytes**: a primeira linha inteira, depois a segunda, e assim por diante. Cada
pixel ocupa `canais` bytes: 3 em RGB (vermelho, verde, azul) ou 4 em RGBA, que
acrescenta a transparência (alfa).

Para achar o pixel `(x, y)`, pulamos `y` linhas completas e mais `x` pixels:

```text
índice = (y * largura + x) * canais
```

<div class="rt-demo" data-demo="memoria">
  <p class="rt-titulo">Experimente: escolha um pixel</p>
  <div class="rt-duas">
    <div class="rt-grade" data-grade></div>
    <div class="rt-painel" data-info aria-live="polite">Esta demonstração precisa de JavaScript.</div>
  </div>
  <div class="rt-fita" data-fita></div>
</div>

## As operações, uma a uma

### Carregar: `img_load`

1. `stbi_info` lê só o cabeçalho e diz quantos canais o arquivo tem.
2. O runtime escolhe RGB ou RGBA. Imagens em cinza são convertidas, e por isso
   as outras operações só precisam tratar 3 ou 4 canais.
3. `stbi_load` decodifica o arquivo para o vetor de bytes.

Se o arquivo não existe ou não é uma imagem, devolve `NULL`.

### Redimensionar: `img_resize`

<div class="rt-fotos">
  <figure><img src="{{ '/assets/img/runtime/original.jpg' | relative_url }}" alt="Paisagem original, 480 por 320 pixels"><figcaption>original (480x320)</figcaption></figure>
  <figure><img src="{{ '/assets/img/runtime/redimensionar.jpg' | relative_url }}" alt="Paisagem mais estreita, 240 por 320 pixels"><figcaption><code>redimensionar foto para 240 por 320;</code></figcaption></figure>
</div>

Redimensionar cria pixels que não existiam. A `stb_image_resize2` calcula cada
um a partir dos vizinhos na imagem original. A proporção não é mantida
automaticamente: a imagem assume exatamente a largura e a altura pedidas.

Em imagens RGBA, a cor é ponderada pelo alfa. Sem isso, a cor escondida em um
pixel transparente vazaria para as bordas.

### Tons de cinza: `img_grayscale`

<div class="rt-fotos">
  <figure><img src="{{ '/assets/img/runtime/original.jpg' | relative_url }}" alt="Paisagem colorida"><figcaption>original</figcaption></figure>
  <figure><img src="{{ '/assets/img/runtime/tons-de-cinza.jpg' | relative_url }}" alt="Paisagem em tons de cinza"><figcaption><code>tons_de_cinza foto;</code></figcaption></figure>
</div>

O olho humano percebe melhor o verde do que o azul. Por isso o cinza não é a
média simples das três cores, e sim a **luminância**, com um peso para cada:

```text
cinza = 0,299 * R + 0,587 * G + 0,114 * B
```

```c
p[0] = p[1] = p[2] =
    (unsigned char) ((299 * p[0] + 587 * p[1] + 114 * p[2] + 500) / 1000);
```

A conta usa só inteiros, e o `+ 500` arredonda para o valor mais próximo. Os
três canais recebem o mesmo valor, então a imagem continua RGB, e o alfa não
muda.

<div class="rt-demo" data-demo="cinza">
  <p class="rt-titulo">Experimente: misture uma cor</p>
  <div class="rt-duas">
    <div class="rt-controles">
      <label>R <input type="range" min="0" max="255" value="240" data-c="r"><output></output></label>
      <label>G <input type="range" min="0" max="255" value="138" data-c="g"><output></output></label>
      <label>B <input type="range" min="0" max="255" value="93" data-c="b"><output></output></label>
    </div>
    <div class="rt-cores"><div><i data-cor></i>cor</div><div><i data-cinza></i>luminância</div><div><i data-media></i>média simples</div></div>
  </div>
  <div class="rt-painel" data-info aria-live="polite">Esta demonstração precisa de JavaScript.</div>
</div>

### Rotacionar: `img_rotate`

<div class="rt-fotos rt-tres">
  <figure><img src="{{ '/assets/img/runtime/rotacionar-90.jpg' | relative_url }}" alt="Paisagem girada 90 graus no sentido horário"><figcaption><code>rotacionar foto 90;</code></figcaption></figure>
  <figure><img src="{{ '/assets/img/runtime/rotacionar-180.jpg' | relative_url }}" alt="Paisagem girada 180 graus"><figcaption><code>rotacionar foto 180;</code></figcaption></figure>
  <figure><img src="{{ '/assets/img/runtime/rotacionar-270.jpg' | relative_url }}" alt="Paisagem girada 270 graus no sentido horário"><figcaption><code>rotacionar foto 270;</code></figcaption></figure>
</div>

Só a rotação de 90° (sentido horário) está escrita. As outras são repetições
dela: duas vezes para 180° e três para 270°.

```c
for (int i = 0; i < angulo / 90; i++)
    if (rotacionar_90(img) != 0)
        return 1;
```

Em cada giro, largura e altura trocam de lugar, e o pixel `(x, y)` vai para
`(altura - 1 - y, x)`.

<div class="rt-demo" data-demo="rotacao">
  <p class="rt-titulo">Experimente: gire e acompanhe o pixel A</p>
  <div class="rt-botoes"><button type="button" data-ang="90">90° (1 giro)</button><button type="button" data-ang="180">180° (2 giros)</button><button type="button" data-ang="270">270° (3 giros)</button><button type="button" data-ang="0">Recomeçar</button></div>
  <div class="rt-duas">
    <div class="rt-grade" data-grade></div>
    <div class="rt-painel" data-info aria-live="polite">Esta demonstração precisa de JavaScript.</div>
  </div>
</div>

### Recortar: `img_crop`

<div class="rt-fotos">
  <figure><img src="{{ '/assets/img/runtime/original.jpg' | relative_url }}" alt="Paisagem original"><figcaption>original (480x320)</figcaption></figure>
  <figure><img src="{{ '/assets/img/runtime/recortar.jpg' | relative_url }}" alt="Recorte da casa, 160 por 120 pixels"><figcaption><code>recortar foto de 30, 160 tamanho 160 por 120;</code></figcaption></figure>
</div>

O recorte copia, linha por linha, o retângulo pedido para um buffer menor. Antes
disso, confere se o retângulo cabe na imagem:

```text
x + largura do recorte <= largura da imagem
y + altura do recorte  <= altura da imagem
```

No código, a mesma regra aparece com subtração. Somar dois números muito grandes
poderia estourar o `int`; subtrair, não.

```c
if (x < 0 || y < 0
    || x > img->largura - largura || y > img->altura - altura) {
```

<div class="rt-demo" data-demo="recorte">
  <p class="rt-titulo">Experimente: recorte uma imagem de 12x8</p>
  <div class="rt-duas">
    <div class="rt-grade rt-mini" data-grade></div>
    <div class="rt-controles">
      <label>x <input type="range" min="0" max="12" value="2" data-c="x"><output></output></label>
      <label>y <input type="range" min="0" max="8" value="1" data-c="y"><output></output></label>
      <label>larg. <input type="range" min="1" max="12" value="5" data-c="l"><output></output></label>
      <label>alt. <input type="range" min="1" max="8" value="4" data-c="a"><output></output></label>
    </div>
  </div>
  <div class="rt-painel" data-info aria-live="polite">Esta demonstração precisa de JavaScript.</div>
</div>

### Salvar: `img_save`

O formato é escolhido pela extensão, sem diferenciar maiúsculas de minúsculas:

| Extensão | Formato | Observação |
|---|---|---|
| `.png` | PNG | sem perda; guarda a transparência |
| `.jpg`, `.jpeg` | JPG | qualidade 90; descarta a transparência; lado máximo de 65535 pixels |
| outra | nenhum | erro de execução |

### Liberar: `img_destroy`

Devolve a memória dos pixels e da struct. O compilador emite essa chamada
sozinho no fim do programa, uma para cada imagem; por isso a ImagemLang não tem
um comando para liberar memória.

## Erro de compilação ou de execução?

O compilador encontra tudo o que dá para saber **lendo o programa**. O runtime
encontra o que só se descobre **abrindo a imagem**.

| Situação | Quem percebe | Por quê |
|---|---|---|
| `rotacionar foto 45;` | compilador | o ângulo está escrito no programa |
| variável não declarada | compilador | basta olhar as declarações |
| arquivo que não existe | runtime | depende do disco na hora de rodar |
| recorte maior que a imagem | runtime | o tamanho só é conhecido depois de abrir o arquivo |

Todas as mensagens abaixo saem em `stderr` e terminam o programa com código `1`:

```saida
erro de execucao: nao foi possivel carregar 'sumiu.jpg': can't fopen
erro de execucao: nao foi possivel carregar 'lixo.txt': unknown image type
erro de execucao: recortar: area de 800, 0 tamanho 300x200 fora da imagem 960x640
erro de execucao: redimensionar: dimensoes invalidas 60000x60000
erro de execucao: salvar: extensao nao suportada em 'saida.bmp' (use .png ou .jpg)
erro de execucao: salvar: imagem 70000x10 grande demais para JPG
erro de execucao: nao foi possivel salvar 'pasta/saida.png'
```

O detalhe depois de `carregar` vem da `stb_image`, por isso está em inglês.

## Rode você mesmo

O `make demo` usa `examples/programa_basico.img`, que abre `foto.jpg` na raiz do
repositório. Use uma foto sua ou a desta página:

```sh
cp docs/assets/img/runtime/original.jpg foto.jpg
make demo
```

O resultado é `resultado.jpg`, com 300x200 pixels. Passo a passo, o mesmo que o
`make demo` faz:

```sh
./build/imagemc --c examples/programa_basico.img > build/programa_basico.c
gcc -Wall -Wextra -g -Iruntime -o build/programa_basico build/programa_basico.c runtime/runtime.c -lm
./build/programa_basico
```

{: .dica }
Todas as fotos desta página foram geradas assim: um programa `.img`, o
`imagemc` e o runtime.

## Como foi verificado

| Verificação | Resultado |
|---|---|
| GCC e Clang com `-Wall -Wextra` | nenhum aviso |
| `make test` | 40 passou, 0 falhou |
| Testes do runtime com AddressSanitizer e UBSan | 114 verificações, 0 falhas |
| Valgrind na demonstração | nenhum erro ou vazamento |
| Rotação e recorte comparados ao ImageMagick | idênticos, pixel a pixel |
| Tons de cinza comparados ao ImageMagick | diferença máxima de 1 nível em 255 |

## Perguntas rápidas

<details markdown="0"><summary>Por que o compilador não processa a imagem direto?</summary><p>Porque isso seria um interpretador. O <code>imagemc</code> traduz o programa; o executável resultante roda sem o compilador e aceita qualquer <code>foto.jpg</code> que estiver na pasta.</p></details>
<details markdown="0"><summary>Por que <code>stb</code>, e não MagickWand?</summary><p>MagickWand precisa ser instalada em cada máquina. As <code>stb</code> vêm no repositório, e o projeto compila só com Flex, Bison, GCC e Make.</p></details>
<details markdown="0"><summary>Por que as variáveis aparecem como <code>im_foto</code>?</summary><p>Para não colidir com palavras do C. Uma imagem chamada <code>int</code>, por exemplo, vira <code>im_int</code>.</p></details>
<details markdown="0"><summary>Por que girar em laço, em vez de escrever as três rotações?</summary><p>Um algoritmo só significa um lugar só para errar e testar. O custo é copiar a imagem até três vezes, aceitável aqui.</p></details>
<details markdown="0"><summary>O que acontece com a memória se der erro no meio?</summary><p>O programa encerra com <code>return 1</code>, e o sistema operacional recupera toda a memória do processo.</p></details>
