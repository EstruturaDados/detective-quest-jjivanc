// detective_quest.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* ============================================================
   ESTRUTURA 1: Mansão (Árvore Binária)
   Cada sala tem APENAS nome; a pista é definida por lógica no código.
   ============================================================ */
typedef struct Sala {
    char nome[64];
    struct Sala *esquerda, *direita;
} Sala;

/* ------------------------------------------------------------
 * criarSala() – cria dinamicamente um cômodo
 * ------------------------------------------------------------ */
Sala* criarSala(const char* nome) {
    Sala* s = (Sala*) malloc(sizeof(Sala));
    if (!s) { fprintf(stderr, "Falha ao alocar Sala.\n"); exit(1); }
    strncpy(s->nome, nome, sizeof(s->nome)-1);
    s->nome[sizeof(s->nome)-1] = '\0';
    s->esquerda = s->direita = NULL;
    return s;
}

/* ============================================================
   ESTRUTURA 2: Pistas coletadas (BST)
   Nós guardam apenas o texto da pista (string).
   ============================================================ */
typedef struct PistaNode {
    char pista[128];
    struct PistaNode *esq, *dir;
} PistaNode;

/* ------------------------------------------------------------
 * criarPistaNode() – nó de pista para BST
 * ------------------------------------------------------------ */
PistaNode* criarPistaNode(const char* pista) {
    PistaNode* n = (PistaNode*) malloc(sizeof(PistaNode));
    if (!n) { fprintf(stderr, "Falha ao alocar PistaNode.\n"); exit(1); }
    strncpy(n->pista, pista, sizeof(n->pista)-1);
    n->pista[sizeof(n->pista)-1] = '\0';
    n->esq = n->dir = NULL;
    return n;
}

/* ------------------------------------------------------------
 * inserirPista() – insere a pista coletada na BST (ordem alfabética)
 * Evita duplicatas: se já existir, não insere novamente.
 * ------------------------------------------------------------ */
PistaNode* inserirPista(PistaNode* raiz, const char* pista) {
    if (!pista || !*pista) return raiz;
    if (raiz == NULL) return criarPistaNode(pista);

    int cmp = strcmp(pista, raiz->pista);
    if (cmp < 0) {
        raiz->esq = inserirPista(raiz->esq, pista);
    } else if (cmp > 0) {
        raiz->dir = inserirPista(raiz->dir, pista);
    } // igual: ignora duplicata
    return raiz;
}

/* ------------------------------------------------------------
 * exibirPistas() – imprime a BST em ordem alfabética
 * ------------------------------------------------------------ */
void exibirPistas(const PistaNode* r) {
    if (!r) return;
    exibirPistas(r->esq);
    printf("- %s\n", r->pista);
    exibirPistas(r->dir);
}

/* ============================================================
   ESTRUTURA 3: Tabela Hash (pista -> suspeito)
   Implementação com encadeamento separado (listas).
   ============================================================ */
typedef struct HashNode {
    char chavePista[128];   // key
    char suspeito[64];      // value
    struct HashNode* prox;
} HashNode;

typedef struct HashTable {
    HashNode** buckets;
    size_t capacidade;
} HashTable;

/* ------------------------------------------------------------
 * hash_djb2() – hash simples para strings
 * ------------------------------------------------------------ */
static unsigned long hash_djb2(const char* str) {
    unsigned long h = 5381;
    int c;
    while ((c = (unsigned char)*str++)) {
        h = ((h << 5) + h) + c; // h*33 + c
    }
    return h;
}

/* ------------------------------------------------------------
 * criarHash() – cria tabela hash com capacidade dada
 * ------------------------------------------------------------ */
HashTable* criarHash(size_t capacidade) {
    HashTable* ht = (HashTable*) malloc(sizeof(HashTable));
    if (!ht) { fprintf(stderr, "Falha ao alocar HashTable.\n"); exit(1); }
    ht->capacidade = capacidade;
    ht->buckets = (HashNode**) calloc(capacidade, sizeof(HashNode*));
    if (!ht->buckets) { fprintf(stderr, "Falha ao alocar buckets.\n"); exit(1); }
    return ht;
}

/* ------------------------------------------------------------
 * inserirNaHash() – insere associação pista/suspeito na hash
 * (se a chave já existir, atualiza o suspeito)
 * ------------------------------------------------------------ */
void inserirNaHash(HashTable* ht, const char* pista, const char* suspeito) {
    if (!ht || !pista || !suspeito) return;
    unsigned long h = hash_djb2(pista) % ht->capacidade;
    HashNode* cur = ht->buckets[h];

    // Atualiza se já existe
    while (cur) {
        if (strcmp(cur->chavePista, pista) == 0) {
            strncpy(cur->suspeito, suspeito, sizeof(cur->suspeito)-1);
            cur->suspeito[sizeof(cur->suspeito)-1] = '\0';
            return;
        }
        cur = cur->prox;
    }

    // Insere novo
    HashNode* novo = (HashNode*) malloc(sizeof(HashNode));
    if (!novo) { fprintf(stderr, "Falha ao alocar HashNode.\n"); exit(1); }
    strncpy(novo->chavePista, pista, sizeof(novo->chavePista)-1);
    novo->chavePista[sizeof(novo->chavePista)-1] = '\0';
    strncpy(novo->suspeito, suspeito, sizeof(novo->suspeito)-1);
    novo->suspeito[sizeof(novo->suspeito)-1] = '\0';
    novo->prox = ht->buckets[h];
    ht->buckets[h] = novo;
}

/* ------------------------------------------------------------
 * encontrarSuspeito() – retorna suspeito associado à pista (ou "")
 * ------------------------------------------------------------ */
const char* encontrarSuspeito(HashTable* ht, const char* pista) {
    if (!ht || !pista) return "";
    unsigned long h = hash_djb2(pista) % ht->capacidade;
    for (HashNode* cur = ht->buckets[h]; cur; cur = cur->prox) {
        if (strcmp(cur->chavePista, pista) == 0) {
            return cur->suspeito;
        }
    }
    return "";
}

/* ------------------------------------------------------------
 * util: lower-case comparador de strings (case-insensitive)
 * ------------------------------------------------------------ */
int str_ieq(const char* a, const char* b) {
    if (!a || !b) return 0;
    while (*a && *b) {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b)) return 0;
        a++; b++;
    }
    return *a == '\0' && *b == '\0';
}

/* ------------------------------------------------------------
 * contarPistasParaSuspeito() – varre a BST e conta quantas
 * pistas mapeiam para o suspeito acusado (usando a hash)
 * ------------------------------------------------------------ */
int contarPistasParaSuspeito(const PistaNode* r, HashTable* ht, const char* acusado) {
    if (!r) return 0;
    int left = contarPistasParaSuspeito(r->esq, ht, acusado);
    int mid  = str_ieq(encontrarSuspeito(ht, r->pista), acusado) ? 1 : 0;
    int right= contarPistasParaSuspeito(r->dir, ht, acusado);
    return left + mid + right;
}

/* ------------------------------------------------------------
 * liberarBST() / liberarHash() / liberarMansao()
 * ------------------------------------------------------------ */
void liberarBST(PistaNode* r) {
    if (!r) return;
    liberarBST(r->esq);
    liberarBST(r->dir);
    free(r);
}

void liberarHash(HashTable* ht) {
    if (!ht) return;
    for (size_t i = 0; i < ht->capacidade; ++i) {
        HashNode* cur = ht->buckets[i];
        while (cur) {
            HashNode* nxt = cur->prox;
            free(cur);
            cur = nxt;
        }
    }
    free(ht->buckets);
    free(ht);
}

void liberarMansao(Sala* r) {
    if (!r) return;
    liberarMansao(r->esquerda);
    liberarMansao(r->direita);
    free(r);
}

/* ============================================================
   Pistas estáticas por sala (lógica no código)
   ============================================================ */
const char* pistaPorSala(const char* nomeSala) {
    // mapeamento "fixo" (pode ajustar conforme a história)
    if (strcmp(nomeSala, "Hall de Entrada") == 0)  return "Pegada com lama no tapete";
    if (strcmp(nomeSala, "Sala de Estar") == 0)    return "Copo quebrado no chão";
    if (strcmp(nomeSala, "Cozinha") == 0)          return "Faca com marcas recentes";
    if (strcmp(nomeSala, "Biblioteca") == 0)       return "Livro raro fora da estante";
    if (strcmp(nomeSala, "Jardim") == 0)           return "Terra revirada junto ao canteiro";
    if (strcmp(nomeSala, "Quarto") == 0)           return "Bilhete rasgado sob a cama";
    if (strcmp(nomeSala, "Adega") == 0)            return "Chave antiga com ferrugem";
    // sem pista explícita
    return "";
}

/* ============================================================
   Popular a tabela hash (pista -> suspeito)
   (feito uma vez no início)
   ============================================================ */
void carregarAssociacoes(HashTable* ht) {
    // Exemplo de relações (ajuste à narrativa):
    inserirNaHash(ht, "Pegada com lama no tapete",        "Jardineiro");
    inserirNaHash(ht, "Copo quebrado no chão",            "Mordomo");
    inserirNaHash(ht, "Faca com marcas recentes",         "Cozinheira");
    inserirNaHash(ht, "Livro raro fora da estante",       "Bibliotecária");
    inserirNaHash(ht, "Terra revirada junto ao canteiro", "Jardineiro");
    inserirNaHash(ht, "Bilhete rasgado sob a cama",       "Mordomo");
    inserirNaHash(ht, "Chave antiga com ferrugem",        "Bibliotecária");
}

/* ============================================================
   Interface / Fluxo do jogo
   ============================================================ */

/* ------------------------------------------------------------
 * explorarSalas() – navega pela árvore e ativa o sistema de pistas
 * - a cada sala visitada: mostra a sala, revela a pista (se houver)
 *   e insere na BST de pistas coletadas.
 * - navegação: (e) esquerda, (d) direita, (s) sair
 * ------------------------------------------------------------ */
void explorarSalas(Sala* inicio, PistaNode** pistasBST) {
    Sala* atual = inicio;
    char op;

    while (atual) {
        printf("\nVocê está em: %s\n", atual->nome);

        const char* pista = pistaPorSala(atual->nome);
        if (pista && *pista) {
            printf("Pista encontrada: %s\n", pista);
            *pistasBST = inserirPista(*pistasBST, pista);
        } else {
            printf("Nenhuma pista aqui.\n");
        }

        printf("\nEscolha um caminho:\n");
        if (atual->esquerda) printf(" (e) Esquerda -> %s\n", atual->esquerda->nome);
        if (atual->direita)  printf(" (d) Direita  -> %s\n", atual->direita->nome);
        printf(" (s) Sair da exploração\n");
        printf("Sua escolha: ");
        if (scanf(" %c", &op) != 1) { printf("Entrada inválida.\n"); return; }

        if (op == 'e' && atual->esquerda) {
            atual = atual->esquerda;
        } else if (op == 'd' && atual->direita) {
            atual = atual->direita;
        } else if (op == 's') {
            printf("\nExploração encerrada pelo jogador.\n");
            break;
        } else {
            printf("Opção inválida, tente novamente.\n");
        }
    }
}

/* ------------------------------------------------------------
 * verificarSuspeitoFinal() – julgamento final:
 * - lista pistas coletadas
 * - pede acusação
 * - verifica se ≥ 2 pistas apontam para o acusado
 * ------------------------------------------------------------ */
void limparEntrada() {
    int c; while ((c = getchar()) != '\n' && c != EOF) {}
}

void verificarSuspeitoFinal(PistaNode* pistasBST, HashTable* ht) {
    printf("\n==============================\n");
    printf("Pistas coletadas (ordem A-Z):\n");
    printf("==============================\n");
    if (!pistasBST) {
        printf("(nenhuma pista coletada)\n");
    } else {
        exibirPistas(pistasBST);
    }

    printf("\nSuspeitos possíveis: Mordomo, Cozinheira, Jardineiro, Bibliotecária\n");
    printf("Quem você acusa? ");
    limparEntrada(); // limpa \n deixado por scanf anterior
    char acusado[64];
    if (!fgets(acusado, sizeof(acusado), stdin)) {
        printf("Não foi possível ler a acusação.\n");
        return;
    }
    // remove \n
    size_t len = strlen(acusado);
    if (len && acusado[len-1] == '\n') acusado[len-1] = '\0';

    int qnt = contarPistasParaSuspeito(pistasBST, ht, acusado);

    printf("\nResultado do julgamento:\n");
    if (qnt >= 2) {
        printf("Acusação de \"%s\" SUSTENTADA por %d pistas. Caso encerrado!\n", acusado, qnt);
    } else {
        printf("Acusação de \"%s\" NÃO sustentada (apenas %d pista(s)). Investigue mais!\n", acusado, qnt);
    }
}

/* ============================================================
   main() – monta o mapa fixo e roda o jogo
   ============================================================ */
int main(void) {
    // 1) Monta a mansão (árvore binária fixa)
    Sala* hall       = criarSala("Hall de Entrada");
    Sala* salaEstar  = criarSala("Sala de Estar");
    Sala* cozinha    = criarSala("Cozinha");
    Sala* biblioteca = criarSala("Biblioteca");
    Sala* jardim     = criarSala("Jardim");
    Sala* quarto     = criarSala("Quarto");
    Sala* adega      = criarSala("Adega");

    // Ligações (pode ajustar a estrutura à vontade)
    hall->esquerda = salaEstar;
    hall->direita  = cozinha;

    salaEstar->esquerda = biblioteca;
    salaEstar->direita  = jardim;

    cozinha->esquerda = quarto;
    cozinha->direita  = adega;

    // 2) Cria BST de pistas (vazia no início)
    PistaNode* pistasBST = NULL;

    // 3) Cria e carrega a tabela hash (pista -> suspeito)
    HashTable* ht = criarHash(101);
    carregarAssociacoes(ht);

    // 4) Exploração interativa
    printf("=== Detective Quest: Julgamento Final ===\n");
    printf("Navegação: (e) esquerda, (d) direita, (s) sair\n");
    explorarSalas(hall, &pistasBST);

    // 5) Julgamento
    verificarSuspeitoFinal(pistasBST, ht);

    // 6) Limpeza
    liberarBST(pistasBST);
    liberarHash(ht);
    liberarMansao(hall);

    return 0;
}
