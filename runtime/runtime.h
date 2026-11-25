/* FGA0003 - Compiladores 1 */
/* Curso de Engenharia de Software */
/* Universidade de Brasília (UnB) */
/* ImagemLang - runtime de imagens em C */

/* CONTRATO CONGELADO: as assinaturas abaixo foram combinadas pelo grupo.
   Mudar alguma delas exige combinar antes com quem a usa (codegen.c).
   Detalhes das operações e do uso das bibliotecas stb: docs/modulos.md. */

#ifndef IMAGEMLANG_RUNTIME_H
#define IMAGEMLANG_RUNTIME_H

/* Representa uma imagem carregada em memória. Os pixels ficam em formato
   intercalado linha a linha (RGB ou RGBA, conforme 'canais'). */
typedef struct {
    unsigned char *dados;
    int largura;
    int altura;
    int canais;
} Imagem;

/* Carrega uma imagem do disco (stb_image). Em caso de erro, imprime mensagem
   em stderr e devolve NULL. */
Imagem *img_load(const char *caminho);

/* Redimensiona 'img' no lugar para 'largura' x 'altura' (stb_image_resize2).
   Devolve 0 em sucesso e diferente de 0 em erro. */
int img_resize(Imagem *img, int largura, int altura);

/* Converte 'img' para tons de cinza no lugar, preservando o número de canais.
   Devolve 0 em sucesso e diferente de 0 em erro. */
int img_grayscale(Imagem *img);

/* Rotaciona 'img' no lugar em sentido horário (90, 180 ou 270 graus).
   Devolve 0 em sucesso e diferente de 0 em erro. */
int img_rotate(Imagem *img, int angulo);

/* Recorta 'img' no lugar a partir de (x, y) com dimensões largura x altura.
   Valida em tempo de execução se x + largura <= img->largura e
   y + altura <= img->altura. Devolve 0 em sucesso e diferente de 0 em erro. */
int img_crop(Imagem *img, int x, int y, int largura, int altura);

/* Salva 'img' em 'caminho' (extensões .png e .jpg/.jpeg via stb_image_write).
   Devolve 0 em sucesso e diferente de 0 em erro. */
int img_save(const Imagem *img, const char *caminho);

/* Libera o buffer de pixels e a struct Imagem. Aceita NULL. */
void img_destroy(Imagem *img);

#endif
