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

    const char *nome_arquivo = argv[1];
    double fator_escalar = atof(argv[2]);

    int largura = 0, altura = 0, canais = 0;
    unsigned char *imagem = stbi_load(nome_arquivo, &largura, &altura, &canais, 0);
    if (!imagem) {
        printf("nao foi possivel carregar a imagem '%s'.\n", nome_arquivo);
        return 1;
    }

    int total_pixels = largura * altura * canais;
    for (int i = 0; i < total_pixels; i++) {
        int novo_valor = (int) (imagem[i] * fator_escalar);
        imagem[i] = (unsigned char) clamp_int(novo_valor);
    }

    if (stbi_write_jpg("resultado_multiplicacao.jpg", largura, altura, canais, imagem, 100)) {
        printf("imagem salva 'resultado_multiplicacao.jpg'\n");
    } else {
        printf("nao foi possivel salvar.\n");
        stbi_image_free(imagem);
        return 1;
    }

    stbi_image_free(imagem);
    return 0;
}
