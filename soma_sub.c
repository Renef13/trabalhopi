#include <stdio.h>
#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

static int clamp_int(int v) {
    return v < 0 ? 0 : (v > 255 ? 255 : v);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        return 1;
    }

    const char *arquivo1 = argv[1];
    const char *arquivo2 = argv[2];

    int largura1 = 0, altura1 = 0, canais1 = 0;
    unsigned char *imagem1 = stbi_load(arquivo1, &largura1, &altura1, &canais1, 0);
    if (!imagem1) {
        printf("nao foi possivel carregar a imagem '%s'.\n", arquivo1);
        return 1;
    }

    int largura2 = 0, altura2 = 0, canais2 = 0;
    unsigned char *imagem2 = stbi_load(arquivo2, &largura2, &altura2, &canais2, 0);
    if (!imagem2) {
        printf("nao foi possivel carregar a imagem '%s'.\n", arquivo2);
        stbi_image_free(imagem1);
        return 1;
    }

    if (largura1 != largura2 || altura1 != altura2 || canais1 != canais2) {
        printf("as imagens devem ter as mesmas dimensoes e canais.\n");
        printf("imagem1: %dx%dx%d\n", largura1, altura1, canais1);
        printf("imagem2: %dx%dx%d\n", largura2, altura2, canais2);
        stbi_image_free(imagem1);
        stbi_image_free(imagem2);
        return 1;
    }

    int total_pixels = largura1 * altura1 * canais1;

    unsigned char *resultado_soma = malloc(total_pixels);
    unsigned char *resultado_subtracao = malloc(total_pixels);

    if (!resultado_soma || !resultado_subtracao) {
        printf("falha ao alocar memoria.\n");
        stbi_image_free(imagem1);
        stbi_image_free(imagem2);
        free(resultado_soma);
        free(resultado_subtracao);
        return 1;
    }

    for (int i = 0; i < total_pixels; i++) {
        int valor1 = (int) imagem1[i];
        int valor2 = (int) imagem2[i];

        int soma = valor1 + valor2;
        resultado_soma[i] = (unsigned char) clamp_int(soma);

        int subtracao = valor1 - valor2;
        resultado_subtracao[i] = (unsigned char) clamp_int(subtracao);
    }

    if (stbi_write_jpg("soma_output.jpg", largura1, altura1, canais1, resultado_soma, 100)) {
        printf("imagem salva 'soma_output.jpg'\n");
    } else {
        printf("nao foi possivel salvar soma_output.jpg.\n");
        stbi_image_free(imagem1);
        stbi_image_free(imagem2);
        free(resultado_soma);
        free(resultado_subtracao);
        return 1;
    }

    if (stbi_write_jpg("subtracao_output.jpg", largura1, altura1, canais1, resultado_subtracao, 100)) {
        printf("imagem salva 'subtracao_output.jpg'\n");
    } else {
        printf("nao foi possivel salvar subtracao_output.jpg.\n");
        stbi_image_free(imagem1);
        stbi_image_free(imagem2);
        free(resultado_soma);
        free(resultado_subtracao);
        return 1;
    }

    stbi_image_free(imagem1);
    stbi_image_free(imagem2);
    free(resultado_soma);
    free(resultado_subtracao);
    return 0;
}
