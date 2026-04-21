#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define STB_IMAGE_IMPLEMENTATION
#include "../stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../stb_image_write.h"

static int clamp_int(int v) {
	return v < 0 ? 0 : (v > 255 ? 255 : v);
}

static void inicializar_centroides(unsigned char *img, int total_pixels, int canais, int k, unsigned char *centroides, int usar_aleatorio) {
	if (usar_aleatorio) {
		for (int c = 0; c < k; c++) {
			int idx = rand() % total_pixels;
			for (int ch = 0; ch < canais; ch++) {
				centroides[c * canais + ch] = img[idx * canais + ch];
			}
		}
	} else {
		int limite = k < total_pixels ? k : total_pixels;
		for (int c = 0; c < limite; c++) {
			for (int ch = 0; ch < canais; ch++) {
				centroides[c * canais + ch] = img[c * canais + ch];
			}
		}
		for (int c = limite; c < k; c++) {
			for (int ch = 0; ch < canais; ch++) {
				centroides[c * canais + ch] = centroides[(c - 1) * canais + ch];
			}
		}
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
		printf("Uso: %s <imagem> <k> [iteracoes] [seed]\n", argv[0]);
		return 1;
	}

	char *nome_sem_ext = extrair_nome_arquivo(argv[1]);
	int k = atoi(argv[2]);
	int max_iter = argc >= 4 ? atoi(argv[3]) : 10;
	int usar_aleatorio = argc >= 5;

	char caminho_entrada[256];
	sprintf(caminho_entrada, "../data/%s", argv[1]);

	char caminho_saida[256];
	sprintf(caminho_saida, "../resultado/%s_quant_k-means.jpg", nome_sem_ext);

	if (k <= 0) {
		printf("k deve ser maior que zero.\n");
		return 1;
	}
	if (max_iter <= 0) {
		printf("iteracoes deve ser maior que zero.\n");
		return 1;
	}
	if (usar_aleatorio) {
		int seed = atoi(argv[4]);
		srand((unsigned)seed);
	} else {
		srand((unsigned)time(NULL));
	}

	int largura = 0, altura = 0, canais = 0;
	unsigned char *imagem = stbi_load(caminho_entrada, &largura, &altura, &canais, 0);
	if (!imagem) {
		printf("nao foi possivel carregar a imagem '%s'.\n", caminho_entrada);
		free(nome_sem_ext);
		return 1;
	}
	if (canais != 1 && canais != 3) {
		printf("apenas imagens em tons de cinza ou RGB.\n");
		stbi_image_free(imagem);
		return 1;
	}

	const int total_pixels = largura * altura;
	if (k > total_pixels) {
		printf("k maior que numero de pixels, ajustando para %d.\n", total_pixels);
		k = total_pixels;
	}

	unsigned char *centroides = malloc((size_t)k * canais);
	long long *novos_centroides = malloc((size_t)k * canais * sizeof(long long));
	int *contagens = malloc((size_t)k * sizeof(int));
	int *rotulos = malloc((size_t)total_pixels * sizeof(int));
	if (!centroides || !novos_centroides || !contagens || !rotulos) {
		printf("falha ao alocar memoria.\n");
		stbi_image_free(imagem);
		free(centroides);
		free(novos_centroides);
		free(contagens);
		free(rotulos);
		return 1;
	}

	for (int i = 0; i < total_pixels; i++) {
		rotulos[i] = -1;
	}

	inicializar_centroides(imagem, total_pixels, canais, k, centroides, usar_aleatorio);

	int mudou = 1;
	for (int iter = 0; iter < max_iter && mudou; iter++) {
		mudou = 0;
		for (int c = 0; c < k; c++) {
			contagens[c] = 0;
			for (int ch = 0; ch < canais; ch++) {
				novos_centroides[c * canais + ch] = 0;
			}
		}

		for (int i = 0; i < total_pixels; i++) {
			int melhor = 0;
			double melhor_dist = 1e18;
			for (int c = 0; c < k; c++) {
				double dist = 0.0;
				for (int ch = 0; ch < canais; ch++) {
					double diff = (double)imagem[i * canais + ch] - (double)centroides[c * canais + ch];
					dist += diff * diff;
				}
				if (dist < melhor_dist) {
					melhor_dist = dist;
					melhor = c;
				}
			}
			if (rotulos[i] != melhor) {
				mudou = 1;
				rotulos[i] = melhor;
			}
			contagens[melhor]++;
			for (int ch = 0; ch < canais; ch++) {
				novos_centroides[melhor * canais + ch] += imagem[i * canais + ch];
			}
		}

		for (int c = 0; c < k; c++) {
			if (contagens[c] == 0) {
				int idx = rand() % total_pixels;
				for (int ch = 0; ch < canais; ch++) {
					centroides[c * canais + ch] = imagem[idx * canais + ch];
				}
				continue;
			}
			for (int ch = 0; ch < canais; ch++) {
				centroides[c * canais + ch] = (unsigned char)(novos_centroides[c * canais + ch] / contagens[c]);
			}
		}
	}

	for (int i = 0; i < total_pixels; i++) {
		int c = rotulos[i];
		for (int ch = 0; ch < canais; ch++) {
			int valor = centroides[c * canais + ch];
			imagem[i * canais + ch] = (unsigned char)clamp_int(valor);
		}
	}

	if (stbi_write_jpg(caminho_saida, largura, altura, canais, imagem, 100)) {
		printf("imagem salva como '%s' com %d cores.\n", caminho_saida, k);
	} else {
		printf("nao foi possivel salvar a imagem.\n");
		stbi_image_free(imagem);
		free(nome_sem_ext);
		return 1;
	}

	stbi_image_free(imagem);
	free(nome_sem_ext);
	free(centroides);
	free(novos_centroides);
	free(contagens);
	free(rotulos);
	return 0;
}

