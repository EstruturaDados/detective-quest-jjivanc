#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ------------------------------
// Estrutura da Mansão (Árvore Binária)
// ------------------------------
typedef struct Sala {
    char nome[50];
    char pista[100];  // pista opcional associada à sala
    struct Sala* esquerda;
    struct Sala* direita;
} Sala;

// ------------------------------
// Estrutura da Árvore de Pistas (BST)
// ------------------------------
typedef struct PistaNode {
    char pista[100];
    struct PistaNode* esquerda;
    struct PistaNode* direita;
} PistaNode;

// ------------------------------
// Função: criarSala
// Cria dinamicamente uma sala da mansão com ou sem pista
// ------------------------------
Sala* criarSala(const char* nome, const char* pista) {
    Sala* novaSala = (Sala*) malloc(sizeof(Sala));
    if (!novaSala) {
        printf("Erro ao alocar memória!\n");
        exit(1);
    }
    strcpy(novaSala->nome, nome);
    if (pista != NULL)
        strcpy(novaSala->pista, pista);
    else
        strcpy(novaSala->pista, "");
    novaSala->esquerda = NULL;
    novaSala->direita = NULL;
    return novaSala;
}

// ------------------------------
// Função: criarPistaNode
// Cria um nó para a árvore de pistas
// ------------------------------
PistaNode* criarPistaNode(const char* pista) {
    PistaNode* novo = (PistaNode*) malloc(sizeof(PistaNode));
    if (!novo) {
        printf("Erro ao alocar memória!\n");
        exit(1);
    }
    strcpy(novo->pista, pista);
    novo->esquerda = NULL;
    novo->direita = NULL;
    return novo;
}

// ------------------------------
// Função: inserirPista
// Insere uma pista na árvore BST em ordem alfabética
// ------------------------------
PistaNode* inserirPista(PistaNode* raiz, const char* pista) {
    if (raiz == NULL) return criarPistaNode(pista);

    if (strcmp(pista, raiz->pista) < 0) {
        raiz->esquerda = inserirPista(raiz->esquerda, pista);
    } else if (strcmp(pista, raiz->pista) > 0) {
        raiz->direita = inserirPista(raiz->direita, pista);
    }
    // se for igual, não insere novamente (evita duplicata)
    return raiz;
}

// ------------------------------
// Função: exibirPistas
// Percorre a BST em ordem e exibe as pistas coletadas
// ------------------------------
void exibirPistas(PistaNode* raiz) {
    if (raiz != NULL) {
        exibirPistas(raiz->esquerda);
        printf("- %s\n", raiz->pista);
        exibirPistas(raiz->direita);
    }
}

// ------------------------------
// Função: explorarSalasComPistas
// Controla a exploração da mansão e coleta as pistas
// ------------------------------
void explorarSalasComPistas(Sala* atual, PistaNode** arvorePistas) {
    char escolha;

    while (atual != NULL) {
        printf("\nVocê está em: %s\n", atual->nome);

        // Coleta pista (se existir)
        if (strlen(atual->pista) > 0) {
            printf("Você encontrou uma pista: %s\n", atual->pista);
            *arvorePistas = inserirPista(*arvorePistas, atual->pista);
        }

        printf("\nEscolha um caminho:\n");
        if (atual->esquerda != NULL) printf(" (e) Ir para a esquerda -> %s\n", atual->esquerda->nome);
        if (atual->direita != NULL)  printf(" (d) Ir para a direita -> %s\n", atual->direita->nome);
        printf(" (s) Sair da exploração\n");
        printf("Sua escolha: ");
        scanf(" %c", &escolha);

        if (escolha == 'e' && atual->esquerda != NULL) {
            atual = atual->esquerda;
        } else if (escolha == 'd' && atual->direita != NULL) {
            atual = atual->direita;
        } else if (escolha == 's') {
            printf("\nVocê decidiu encerrar a exploração.\n");
            break;
        } else {
            printf("Opção inválida. Tente novamente.\n");
        }
    }
}

// ------------------------------
// Função principal (main)
// ------------------------------
int main() {
    // Criando salas com pistas
    Sala* hall       = criarSala("Hall de Entrada", "Pegada suspeita no tapete");
    Sala* salaEstar  = criarSala("Sala de Estar", "Copo quebrado");
    Sala* cozinha    = criarSala("Cozinha", "Faca ensanguentada");
    Sala* biblioteca = criarSala("Biblioteca", "Livro fora de lugar");
    Sala* jardim     = criarSala("Jardim", "");
    Sala* quarto     = criarSala("Quarto", "Carta misteriosa");
    Sala* adega      = criarSala("Adega", "Chave antiga");

    // Montando o mapa da mansão (árvore fixa)
    hall->esquerda = salaEstar;
    hall->direita = cozinha;

    salaEstar->esquerda = biblioteca;
    salaEstar->direita = jardim;

    cozinha->esquerda = quarto;
    cozinha->direita = adega;

    // Árvore de pistas inicialmente vazia
    PistaNode* arvorePistas = NULL;

    // Início da exploração
    explorarSalasComPistas(hall, &arvorePistas);

    // Exibe pistas coletadas
    printf("\n--- Pistas coletadas (em ordem alfabética) ---\n");
    exibirPistas(arvorePistas);

    // Liberação da memória (simplificação: não estou liberando recursivamente)
    free(hall);
    free(salaEstar);
    free(cozinha);
    free(biblioteca);
    free(jardim);
    free(quarto);
    free(adega);

    return 0;
}
