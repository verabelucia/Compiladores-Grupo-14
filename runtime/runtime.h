#ifndef IMAGEMLANG_RUNTIME_H
#define IMAGEMLANG_RUNTIME_H

typedef struct {
    unsigned char *dados;
    int largura;
    int altura;
    int canais;
} Imagem;

Imagem *img_load(const char *caminho);

int img_resize(Imagem *img, int largura, int altura);

int img_grayscale(Imagem *img);

int img_rotate(Imagem *img, int angulo);

int img_crop(Imagem *img, int x, int y, int largura, int altura);

int img_save(const Imagem *img, const char *caminho);

void img_destroy(Imagem *img);

#endif
