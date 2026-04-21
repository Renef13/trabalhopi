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
        printf("Uso: %s <imagem> <divisor>\n", argv[0]);
        return 1;
    }

    char *nome_sem_ext = extrair_nome_arquivo(argv[1]);
    double divisor = atof(argv[2]);

    char caminho_entrada[256];
    sprintf(caminho_entrada, "../data/%s", argv[1]);

    char caminho_saida[256];
    sprintf(caminho_saida, "../resultado/%s_resultado.jpg", nome_sem_ext);

    if (divisor == 0.0) {
        printf("nao eh possivel dividir por zero.\n");
        free(nome_sem_ext);
        return 1;
    }

    int largura = 0, altura = 0, canais = 0;
    unsigned char *imagem = stbi_load(caminho_entrada, &largura, &altura, &canais, 0);
    if (!imagem) {
        printf("nao foi possivel carregar a imagem '%s'.\n", caminho_entrada);
        free(nome_sem_ext);
        return 1;
    }

    int total_pixels = largura * altura * canais;
    for (int i = 0; i < total_pixels; i++) {
        int novo_valor = (int) (imagem[i] / divisor);
        imagem[i] = (unsigned char) clamp_int(novo_valor);
    }

    if (stbi_write_jpg(caminho_saida, largura, altura, canais, imagem, 100)) {
        printf("imagem salva '%s'\n", caminho_saida);
    } else {
        printf("nao foi possivel salvar.\n");
        stbi_image_free(imagem);
        free(nome_sem_ext);
        return 1;
    }

    stbi_image_free(imagem);
    free(nome_sem_ext);
    return 0;
}
