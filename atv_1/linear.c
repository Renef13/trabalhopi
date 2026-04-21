#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STB_IMAGE_IMPLEMENTATION
#include "../stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../stb_image_write.h"

int largura, altura, canais;
unsigned char *imagem = NULL;

void quantizar_imagem_linear(int fator_quantizacao) {
    int fator_reducao = 256 / fator_quantizacao;
    int total_pixels = largura * altura * canais;

    for (int i = 0; i < total_pixels; i++) {
        imagem[i] = (imagem[i] / fator_reducao) * fator_reducao;
    }
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
        printf("entrada insuficiete.\n");
        return 1;
    }

    char *nome_sem_ext = extrair_nome_arquivo(argv[1]);
    int fator_quantizacao = atoi(argv[2]);

    char caminho_entrada[256];
    sprintf(caminho_entrada, "../data/%s", argv[1]);

    char caminho_saida[256];
    sprintf(caminho_saida, "../resultado/%s_resultado.jpg", nome_sem_ext);


    if (fator_quantizacao <= 0 || fator_quantizacao > 256) {
        printf("fator de quatizacao deve estar entre 1 e 256.\n");
        free(nome_sem_ext);
        return 1;
    }

    imagem = stbi_load(caminho_entrada, &largura, &altura, &canais, 0);

    if (imagem == NULL) {
        printf("nao foi possivel carregar a imagem '%s'.\n", caminho_entrada);
        free(nome_sem_ext);
        return 1;
    }

    printf("imagem carregada: %dx%d com %d canais de cor\n", largura, altura, canais);
    printf("aplicando quantizacao com fator %d   \n", fator_quantizacao);

    quantizar_imagem_linear(fator_quantizacao);

    if (stbi_write_jpg(caminho_saida, largura, altura, canais, imagem, 100)) {
        printf("imagem salva como '%s'\n", caminho_saida);
    } else {
        printf("nao foi possivel salvar a imagem.\n");
        stbi_image_free(imagem);
        free(nome_sem_ext);
        return 1;
    }

    stbi_image_free(imagem);
    free(nome_sem_ext);
    return 0;
}