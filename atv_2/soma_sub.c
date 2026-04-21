#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STB_IMAGE_IMPLEMENTATION
#include "../stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../stb_image_write.h"

static int clamp_int(int v) {
    return v < 0 ? 0 : (v > 255 ? 255 : v);
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
        printf("Uso: %s <imagem1> <imagem2>\n", argv[0]);
        return 1;
    }

    char *nome1 = extrair_nome_arquivo(argv[1]);
    char *nome2 = extrair_nome_arquivo(argv[2]);

    char caminho_entrada1[256];
    char caminho_entrada2[256];
    sprintf(caminho_entrada1, "../data/%s", argv[1]);
    sprintf(caminho_entrada2, "../data/%s", argv[2]);

    char caminho_soma[256];
    char caminho_subtracao[256];
    sprintf(caminho_soma, "../resultado/%s_%s_soma.jpg", nome1, nome2);
    sprintf(caminho_subtracao, "../resultado/%s_%s_subtracao.jpg", nome1, nome2);
    int largura1 = 0, altura1 = 0, canais1 = 0;
    unsigned char *imagem1 = stbi_load(caminho_entrada1, &largura1, &altura1, &canais1, 0);
    if (!imagem1) {
        printf("nao foi possivel carregar a imagem '%s'.\n", caminho_entrada1);
        free(nome1);
        free(nome2);
        return 1;
    }

    int largura2 = 0, altura2 = 0, canais2 = 0;
    unsigned char *imagem2 = stbi_load(caminho_entrada2, &largura2, &altura2, &canais2, 0);
    if (!imagem2) {
        printf("nao foi possivel carregar a imagem '%s'.\n", caminho_entrada2);
        stbi_image_free(imagem1);
        free(nome1);
        free(nome2);
        return 1;
    }

    if (largura1 != largura2 || altura1 != altura2 || canais1 != canais2) {
        printf("as imagens devem ter as mesmas dimensoes e canais.\n");
        printf("imagem1: %dx%dx%d\n", largura1, altura1, canais1);
        printf("imagem2: %dx%dx%d\n", largura2, altura2, canais2);
        stbi_image_free(imagem1);
        stbi_image_free(imagem2);
        free(nome1);
        free(nome2);
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
        free(nome1);
        free(nome2);
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

    if (stbi_write_jpg(caminho_soma, largura1, altura1, canais1, resultado_soma, 100)) {
        printf("imagem salva '%s'\n", caminho_soma);
    } else {
        printf("nao foi possivel salvar soma.\n");
        stbi_image_free(imagem1);
        stbi_image_free(imagem2);
        free(resultado_soma);
        free(resultado_subtracao);
        free(nome1);
        free(nome2);
        return 1;
    }

    if (stbi_write_jpg(caminho_subtracao, largura1, altura1, canais1, resultado_subtracao, 100)) {
        printf("imagem salva '%s'\n", caminho_subtracao);
    } else {
        printf("nao foi possivel salvar subtracao.\n");
        stbi_image_free(imagem1);
        stbi_image_free(imagem2);
        free(resultado_soma);
        free(resultado_subtracao);
        free(nome1);
        free(nome2);
        return 1;
    }

    stbi_image_free(imagem1);
    stbi_image_free(imagem2);
    free(resultado_soma);
    free(resultado_subtracao);
    free(nome1);
    free(nome2);
    return 0;
}
