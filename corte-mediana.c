#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

static int comparar_valores(const void *a, const void *b) {
    return (*(unsigned char*)a - *(unsigned char*)b);
}

static void aplicar_corte_mediana(unsigned char *pixels, int total_pixels, int total_cores, unsigned char *mapa) {
    unsigned char *copia_pixels = malloc(total_pixels);
    memcpy(copia_pixels, pixels, total_pixels);
    qsort(copia_pixels, total_pixels, sizeof(unsigned char), comparar_valores);

    int *intervalos = malloc((total_cores + 1) * sizeof(int));
    intervalos[0] = 0;
    intervalos[total_cores] = total_pixels;

    for (int i = 1; i < total_cores; i++) {
        intervalos[i] = (total_pixels / total_cores) * i;
    }

    for (int i = 0; i < total_cores; i++) {
        int soma = 0;
        int contagem = intervalos[i + 1] - intervalos[i];

        for (int j = intervalos[i]; j < intervalos[i + 1]; j++) {
            soma += copia_pixels[j];
        }

        unsigned char cor_media = (unsigned char)(soma / contagem);

        unsigned char valor_min = copia_pixels[intervalos[i]];
        unsigned char valor_max = copia_pixels[intervalos[i + 1] - 1];

        for (int v = valor_min; v <= valor_max; v++) {
            mapa[v] = cor_media;
        }
    }

    free(copia_pixels);
    free(intervalos);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Uso: %s <imagem.jpg> <num_cores>\n", argv[0]);
        return 1;
    }

    int largura, altura, canais;
    unsigned char *imagem = stbi_load(argv[1], &largura, &altura, &canais, 1);
    int quantidade_cores = atoi(argv[2]);

    if (!imagem) {
        printf("erro ao carregar imagem.\n");
        return 1;
    }

    unsigned char mapa[256];
    aplicar_corte_mediana(imagem, largura * altura, quantidade_cores, mapa);

    for (int i = 0; i < largura * altura; i++) {
        imagem[i] = mapa[imagem[i]];
    }

    if (stbi_write_jpg("resultado_corte_mediana.jpg", largura, altura, 1, imagem, 100)) {
        printf("imagem salva como 'resultado_corte_mediana.jpg' com %d cores.\n", quantidade_cores);
    }

    stbi_image_free(imagem);
    return 0;
}