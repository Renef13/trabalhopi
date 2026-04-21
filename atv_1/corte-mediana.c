#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STB_IMAGE_IMPLEMENTATION
#include "../stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../stb_image_write.h"

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

char* extrair_nome_arquivo(const char *caminho) {
    const char *nome = strrchr(caminho, '/');
    if (!nome) nome = strrchr(caminho, '\\');
    if (!nome) nome = caminho;
    else nome++;

    char *resultado = malloc(strlen(nome) + 1);
    strcpy(resultado, nome);

    char *ponto = strrchr(resultado, '.');
    if (ponto) *ponto = '\0';

    return resultado;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Uso: %s <imagem.jpg> <num_cores>\n", argv[0]);
        return 1;
    }

    char *nome_sem_ext = extrair_nome_arquivo(argv[1]);
    int quantidade_cores = atoi(argv[2]);

    char caminho_entrada[256];
    sprintf(caminho_entrada, "../data/%s", argv[1]);

    char caminho_saida[256];
    sprintf(caminho_saida, "../resultado/%s_resultado.jpg", nome_sem_ext);

    int largura, altura, canais;
    unsigned char *imagem = stbi_load(caminho_entrada, &largura, &altura, &canais, 1);

    if (!imagem) {
        printf("erro ao carregar imagem.\n");
        free(nome_sem_ext);
        return 1;
    }

    unsigned char mapa[256];
    aplicar_corte_mediana(imagem, largura * altura, quantidade_cores, mapa);

    for (int i = 0; i < largura * altura; i++) {
        imagem[i] = mapa[imagem[i]];
    }

    if (stbi_write_jpg(caminho_saida, largura, altura, 1, imagem, 100)) {
        printf("imagem salva como '%s' com %d cores.\n", caminho_saida, quantidade_cores);
    }

    stbi_image_free(imagem);
    free(nome_sem_ext);
    return 0;
}