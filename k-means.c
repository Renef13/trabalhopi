#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

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

int main(int argc, char *argv[]) {
	if (argc < 3) {
		printf("Uso: %s <imagem> <k> [iteracoes] [seed]\n", argv[0]);
		return 1;
	}

	const char *nome_arquivo = argv[1];
	int k = atoi(argv[2]);
	int max_iter = argc >= 4 ? atoi(argv[3]) : 10;
	int usar_aleatorio = argc >= 5;

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
	unsigned char *imagem = stbi_load(nome_arquivo, &largura, &altura, &canais, 0);
	if (!imagem) {
		printf("nao foi possivel carregar a imagem '%s'.\n", nome_arquivo);
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

	if (stbi_write_jpg("resultado_kmeans.jpg", largura, altura, canais, imagem, 100)) {
		printf("imagem salva como 'resultado_kmeans.jpg' com %d cores.\n", k);
	} else {
		printf("nao foi possivel salvar a imagem.\n");
	}

	stbi_image_free(imagem);
	free(centroides);
	free(novos_centroides);
	free(contagens);
	free(rotulos);
	return 0;
}

