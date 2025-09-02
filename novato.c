#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Estrutura que representa uma sala da mansão
typedef struct Sala {
    char nome[50];
    struct Sala* esquerda;
    struct Sala* direita;
} Sala;

/**
 * Função: criarSala
 * -----------------
 * Cria dinamicamente uma sala com um nome fornecido.
 */
Sala* criarSala(const char* nome) {
    Sala* novaSala = (Sala*) malloc(sizeof(Sala));
    if (novaSala == NULL) {
        printf("Erro ao alocar memória!\n");
        exit(1);
    }
    strcpy(novaSala->nome, nome);
    novaSala->esquerda = NULL;
    novaSala->direita = NULL;
    return novaSala;
}

/**
 * Função: explorarSalas
 * ---------------------
 * Permite que o jogador explore a mansão de forma interativa,
 * escolhendo caminhos até chegar a um nó folha.
 */
void explorarSalas(Sala* atual) {
    char escolha;

    while (atual != NULL) {
        printf("\nVocê está em: %s\n", atual->nome);

        // Caso seja um nó folha (sem caminhos à esquerda ou direita)
        if (atual->esquerda == NULL && atual->direita == NULL) {
            printf("Você chegou ao fim da exploração!\n");
            break;
        }

        printf("Escolha um caminho:\n");
        if (atual->esquerda != NULL) printf(" (e) Ir para a esquerda -> %s\n", atual->esquerda->nome);
        if (atual->direita != NULL)  printf(" (d) Ir para a direita -> %s\n", atual->direita->nome);
        printf(" (s) Sair do jogo\n");
        printf("Sua escolha: ");
        scanf(" %c", &escolha);

        if (escolha == 'e' && atual->esquerda != NULL) {
            atual = atual->esquerda;
        } else if (escolha == 'd' && atual->direita != NULL) {
            atual = atual->direita;
        } else if (escolha == 's') {
            printf("Você decidiu encerrar a exploração.\n");
            break;
        } else {
            printf("Opção inválida. Tente novamente.\n");
        }
    }
}

/**
 * Função: main
 * ------------
 * Monta o mapa da mansão manualmente (em forma de árvore binária),
 * e inicia a exploração pelo Hall de entrada.
 */
int main() {
    // Criando as salas (árvore fixa)
    Sala* hall       = criarSala("Hall de Entrada");
    Sala* salaEstar  = criarSala("Sala de Estar");
    Sala* cozinha    = criarSala("Cozinha");
    Sala* biblioteca = criarSala("Biblioteca");
    Sala* jardim     = criarSala("Jardim");
    Sala* quarto     = criarSala("Quarto");
    Sala* adega      = criarSala("Adega");

    // Montando a árvore manualmente
    hall->esquerda = salaEstar;
    hall->direita = cozinha;

    salaEstar->esquerda = biblioteca;
    salaEstar->direita = jardim;

    cozinha->esquerda = quarto;
    cozinha->direita = adega;

    // Início da exploração
    explorarSalas(hall);

    // Liberação da memória
    free(hall);
    free(salaEstar);
    free(cozinha);
    free(biblioteca);
    free(jardim);
    free(quarto);
    free(adega);

    return 0;
}
