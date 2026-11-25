#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wall"
#pragma GCC diagnostic ignored "-Wextra"
#endif

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize2.h"

#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

#include "runtime.h"

#define QUALIDADE_JPG 90
#define LADO_MAXIMO_JPG 65535

static int imagem_valida(const Imagem *img, const char *operacao)
{
    if (img != NULL && img->dados != NULL)
        return 1;
    fprintf(stderr, "erro de execucao: %s: imagem invalida\n", operacao);
    return 0;
}

static unsigned char *alocar_pixels(int largura, int altura, int canais,
                                    const char *operacao)
{
    unsigned char *dados = malloc((size_t) largura * altura * canais);

    if (dados == NULL)
        fprintf(stderr, "erro de execucao: %s: memoria insuficiente\n",
                operacao);
    return dados;
}

static void trocar_pixels(Imagem *img, unsigned char *dados, int largura,
                          int altura)
{
    free(img->dados);
    img->dados = dados;
    img->largura = largura;
    img->altura = altura;
}

static unsigned char *pixel(unsigned char *dados, int largura, int canais,
                            int x, int y)
{
    return dados + ((size_t) y * largura + x) * canais;
}

Imagem *img_load(const char *caminho)
{
    int largura, altura, canais;

    if (caminho == NULL) {
        fprintf(stderr, "erro de execucao: img_load: caminho ausente\n");
        return NULL;
    }

    if (!stbi_info(caminho, &largura, &altura, &canais)) {
        fprintf(stderr, "erro de execucao: nao foi possivel carregar '%s': %s\n",
                caminho, stbi_failure_reason());
        return NULL;
    }
    canais = (canais == 2 || canais == 4) ? 4 : 3;

    unsigned char *dados = stbi_load(caminho, &largura, &altura, NULL, canais);
    if (dados == NULL) {
        fprintf(stderr, "erro de execucao: nao foi possivel carregar '%s': %s\n",
                caminho, stbi_failure_reason());
        return NULL;
    }

    Imagem *img = malloc(sizeof *img);
    if (img == NULL) {
        fprintf(stderr, "erro de execucao: img_load: memoria insuficiente\n");
        stbi_image_free(dados);
        return NULL;
    }
    img->dados = dados;
    img->largura = largura;
    img->altura = altura;
    img->canais = canais;
    return img;
}

int img_resize(Imagem *img, int largura, int altura)
{
    if (!imagem_valida(img, "redimensionar"))
        return 1;
    if (largura <= 0 || altura <= 0
        || (long long) largura * altura > INT_MAX / img->canais) {
        fprintf(stderr,
                "erro de execucao: redimensionar: dimensoes invalidas %dx%d\n",
                largura, altura);
        return 1;
    }

    unsigned char *dados = alocar_pixels(largura, altura, img->canais,
                                         "redimensionar");
    if (dados == NULL)
        return 1;

    if (stbir_resize_uint8_linear(img->dados, img->largura, img->altura, 0,
                                  dados, largura, altura, 0,
                                  (stbir_pixel_layout) img->canais) == NULL) {
        fprintf(stderr, "erro de execucao: redimensionar: falha ao gerar "
                        "imagem %dx%d\n", largura, altura);
        free(dados);
        return 1;
    }
    trocar_pixels(img, dados, largura, altura);
    return 0;
}

int img_grayscale(Imagem *img)
{
    if (!imagem_valida(img, "tons_de_cinza"))
        return 1;

    size_t total = (size_t) img->largura * img->altura;
    for (size_t i = 0; i < total; i++) {
        unsigned char *p = img->dados + i * img->canais;

        p[0] = p[1] = p[2] =
            (unsigned char) ((299 * p[0] + 587 * p[1] + 114 * p[2] + 500) / 1000);
    }
    return 0;
}

static int rotacionar_90(Imagem *img)
{
    int largura = img->altura;
    int altura = img->largura;
    unsigned char *dados = alocar_pixels(largura, altura, img->canais,
                                         "rotacionar");
    if (dados == NULL)
        return 1;

    for (int y = 0; y < img->altura; y++)
        for (int x = 0; x < img->largura; x++)
            memcpy(pixel(dados, largura, img->canais, img->altura - 1 - y, x),
                   pixel(img->dados, img->largura, img->canais, x, y),
                   img->canais);

    trocar_pixels(img, dados, largura, altura);
    return 0;
}

int img_rotate(Imagem *img, int angulo)
{
    if (!imagem_valida(img, "rotacionar"))
        return 1;
    if (angulo != 90 && angulo != 180 && angulo != 270) {
        fprintf(stderr, "erro de execucao: rotacionar: angulo deve ser 90, 180 "
                        "ou 270, mas vale %d\n", angulo);
        return 1;
    }

    for (int i = 0; i < angulo / 90; i++)
        if (rotacionar_90(img) != 0)
            return 1;
    return 0;
}

int img_crop(Imagem *img, int x, int y, int largura, int altura)
{
    if (!imagem_valida(img, "recortar"))
        return 1;
    if (largura <= 0 || altura <= 0) {
        fprintf(stderr, "erro de execucao: recortar: dimensoes invalidas %dx%d\n",
                largura, altura);
        return 1;
    }
    if (x < 0 || y < 0
        || x > img->largura - largura || y > img->altura - altura) {
        fprintf(stderr, "erro de execucao: recortar: area de %d, %d tamanho "
                        "%dx%d fora da imagem %dx%d\n",
                x, y, largura, altura, img->largura, img->altura);
        return 1;
    }

    unsigned char *dados = alocar_pixels(largura, altura, img->canais,
                                         "recortar");
    if (dados == NULL)
        return 1;

    for (int linha = 0; linha < altura; linha++)
        memcpy(pixel(dados, largura, img->canais, 0, linha),
               pixel(img->dados, img->largura, img->canais, x, y + linha),
               (size_t) largura * img->canais);

    trocar_pixels(img, dados, largura, altura);
    return 0;
}

int img_save(const Imagem *img, const char *caminho)
{
    if (!imagem_valida(img, "salvar"))
        return 1;
    if (caminho == NULL) {
        fprintf(stderr, "erro de execucao: salvar: caminho ausente\n");
        return 1;
    }

    const char *nome = strrchr(caminho, '/');
    const char *ext = strrchr(nome != NULL ? nome : caminho, '.');
    int gravou;

    if (ext != NULL && strcasecmp(ext, ".png") == 0) {
        gravou = stbi_write_png(caminho, img->largura, img->altura, img->canais,
                                img->dados, 0);
    } else if (ext != NULL && (strcasecmp(ext, ".jpg") == 0
                               || strcasecmp(ext, ".jpeg") == 0)) {
        if (img->largura > LADO_MAXIMO_JPG || img->altura > LADO_MAXIMO_JPG) {
            fprintf(stderr, "erro de execucao: salvar: imagem %dx%d grande "
                            "demais para JPG\n", img->largura, img->altura);
            return 1;
        }
        gravou = stbi_write_jpg(caminho, img->largura, img->altura, img->canais,
                                img->dados, QUALIDADE_JPG);
    } else {
        fprintf(stderr, "erro de execucao: salvar: extensao nao suportada em "
                        "'%s' (use .png ou .jpg)\n", caminho);
        return 1;
    }

    if (!gravou) {
        fprintf(stderr, "erro de execucao: nao foi possivel salvar '%s'\n",
                caminho);
        return 1;
    }
    return 0;
}

void img_destroy(Imagem *img)
{
    if (img == NULL)
        return;
    free(img->dados);
    free(img);
}
