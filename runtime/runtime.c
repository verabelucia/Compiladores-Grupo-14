/* FGA0003 - Compiladores 1 */
/* Curso de Engenharia de Software */
/* Universidade de Brasília (UnB) */
/* ImagemLang - runtime de imagens em C (dono: Pessoa 3) */

/* A FAZER (Pessoa 3): versionar stb_image.h, stb_image_write.h e
   stb_image_resize2.h em runtime/ e implementar as 7 funções conforme
   runtime/runtime.h e docs/modulos.md. Os corpos abaixo são só um esqueleto
   para que o código C gerado pelo codegen já compile em paralelo. */

#include <stdio.h>
#include <stdlib.h>

#include "runtime.h"

Imagem *img_load(const char *caminho)
{
    /* A FAZER: carregar com stbi_load e preencher a struct Imagem. */
    fprintf(stderr, "aviso: img_load('%s') ainda nao implementado\n",
            caminho != NULL ? caminho : "");
    return NULL;
}

int img_resize(Imagem *img, int largura, int altura)
{
    /* A FAZER: redimensionar com stbir_resize_uint8_linear. */
    (void) img;
    (void) largura;
    (void) altura;
    return 0;
}

int img_grayscale(Imagem *img)
{
    /* A FAZER: converter pixels para luminância mantendo o número de canais. */
    (void) img;
    return 0;
}

int img_rotate(Imagem *img, int angulo)
{
    /* A FAZER: implementar rotação de 90 graus e chamar em loop (2x para 180,
       3x para 270). */
    (void) img;
    (void) angulo;
    return 0;
}

int img_crop(Imagem *img, int x, int y, int largura, int altura)
{
    /* A FAZER: validar x + largura <= img->largura e y + altura <= img->altura
       em tempo de execução e copiar o retângulo para um novo buffer. */
    (void) img;
    (void) x;
    (void) y;
    (void) largura;
    (void) altura;
    return 0;
}

int img_save(const Imagem *img, const char *caminho)
{
    /* A FAZER: detectar extensão .png ou .jpg/.jpeg e gravar com
       stbi_write_png / stbi_write_jpg. */
    (void) img;
    (void) caminho;
    return 0;
}

void img_destroy(Imagem *img)
{
    if (img == NULL)
        return;
    free(img->dados);
    free(img);
}
