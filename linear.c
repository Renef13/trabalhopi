#include <stdio.h>
#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

int largura, altura, canais;
unsigned char *imagem = NULL;

void quantizar_imagem_linear(int fator_quantizacao) {
    int fator_reducao = 256 / fator_quantizacao;
    int total_pixels = largura * altura * canais;

    for (int i = 0; i < total_pixels; i++) {
        imagem[i] = (imagem[i] / fator_reducao) * fator_reducao;
    }
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("entrada insuficiete.\n");
        return 1;
    }

    char *nome_arquivo = argv[1];
    int fator_quantizacao = atoi(argv[2]);

    if (fator_quantizacao <= 0 || fator_quantizacao > 256) {
        printf("fator de quatizacao deve estar entre 1 e 256.\n");
        return 1;
    }

    imagem = stbi_load(nome_arquivo, &largura, &altura, &canais, 0);

    if (imagem == NULL) {
        printf("nao foi possivel carregar a imagem '%s'.\n", nome_arquivo);
        return 1;
    }

    printf("imagem carregada: %dx%d com %d canais de cor\n", largura, altura, canais);
    printf("aplicando quantizacao com fator %d   \n", fator_quantizacao);

    quantizar_imagem_linear(fator_quantizacao);

    if (stbi_write_jpg("resultado.jpg", largura, altura, canais, imagem, 100)) {
        printf("imagem salva como 'resultado.jpg'\n");
    } else {
        printf("nao foi possivel salvar a imagem.\n");
        stbi_image_free(imagem);
        return 1;
    }

    stbi_image_free(imagem);
    return 0;
}