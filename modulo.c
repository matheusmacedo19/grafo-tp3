#include <tgmath.h>
#include "lib.h"
void menu(){
    Grafo* grafo;
    int op,fonte,sorvedouro;

    grafo=lerArquivo(grafo);
    do{
        puts("------------------------------DIGITE SUA OPCAO------------------------------\n\n");
        printf("\t1 - Exibir grafo\n\t2 - Exibir fluxo maximo\n\t0 - Sair\n");
        scanf("%d",&op);
        switch (op) {
            case 1:
                printGrafo(grafo);
                break;
            case 2:
                printf("Digite o vertice fonte: \n");
                scanf("%d",&fonte);
                printf("Digite o vertice sorvedouro: \n");
                scanf("%d",&sorvedouro);

                if(fonte>grafo->v || fonte<0 || sorvedouro>grafo->v || sorvedouro < 0){
                    printf("Vertice inserido nao existe!\n");
                    break;
                }

                float fluxoMaximo = fordFulkerson(grafo, fonte, sorvedouro);
                printf("O fluxo maximo: %.2f\n",fluxoMaximo);
                break;
            case 0:
                for (int i = 0; i < grafo->v; i++) {
                    liberarLista(grafo->listaAdj[i]);
                }
                free(grafo);
                break;
            default:
                printf("OPCAO INVALIDA!\n");
                break;
        }
    }while(op!=0);
}
Grafo* lerArquivo(Grafo* grafo){
    FILE *arquivo;
    char linha[100];
    int v, cont = 0;

    arquivo = fopen("C:\\Users\\Matheus\\Desktop\\Grafo.txt", "r");

    if (arquivo == NULL) {
        printf("Não foi possível abrir o arquivo.\n");
        return NULL;
    }

    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        cont += 1;
        if (cont == 1) {
            v = atoi(linha);
            grafo = criarGrafo(v);
        }
        else if(cont>2){
            int origem;
            int destino;
            float capacidade;
            char* token = strtok(linha, " ");
            origem = atoi(token);

            token = strtok(NULL, " ");
            destino = atoi(token);

            token = strtok(NULL, " ");
            capacidade=atof(token);

            addAresta(grafo, origem, destino,capacidade);
        }
    }
    fclose(arquivo);
    return grafo;
}



//FUNÇÃO QUE CRIA UM NO PARA A LISTA DE ADJACÊNCIA
Lista* criarNo(int num,float capacidade) {
    Lista* novoNo = (Lista*)malloc(sizeof(Lista));
    if (novoNo == NULL) {
        printf("Erro ao alocar memória.\n");
        exit(1);
    }
    novoNo->num = num;
    novoNo->capacidade=capacidade;
    novoNo->fluxo=0;
    novoNo->prox = NULL;
    return novoNo;
}

//FUNÇÃO QUE ALOCA O GRAFO COM TODOS OS SEUS VÉRTICES E SUAS LISTAS DE ADJACÊNCIA
Grafo* criarGrafo(int v){
    Grafo* grafo = (Grafo *)malloc(sizeof(Grafo));
    grafo->v = v;
    grafo->listaAdj = (Lista*)malloc(v * sizeof(Lista));
    for (int i = 0; i < v; i++) {
        grafo->listaAdj[i] = NULL;
    }
    return grafo;
}

//FUNÇÃO QUE ADICIONA UMA ARESTA NO GRAFO
void addAresta(Grafo* grafo, int origem, int fim,float capacidade) {
    Lista* novoNo1 = criarNo(fim,capacidade);
    novoNo1->prox = grafo->listaAdj[origem];
    grafo->listaAdj[origem] = novoNo1;
}

void printGrafo(Grafo* grafo) {
    system("cls");
    for (int i = 0; i < grafo->v; i++) {
        Lista* corrente = grafo->listaAdj[i];
        printf("Vertice %d: ", i);
        while (corrente != NULL) {
            printf("%d, ", corrente->num);
            printf("%.1f -> ", corrente->capacidade);
            printf("|");
            corrente = corrente->prox;
        }
        printf("NULL\n");
    }
}

bool BFS(Grafo* grafo, int fonte, int sorvedouro, int pai[]) {
    bool visitado[grafo->v];
    for (int i = 0; i < grafo->v; ++i) {
        visitado[i] = false;
    }

    Lista** fila = (Lista*)malloc(grafo->v * sizeof(Lista));
    int frente = 0, tras = 0;

    visitado[fonte] = true;
    pai[fonte] = -1;
    fila[tras++] = criarNo(fonte, 0);

    while (frente != tras) {
        int u = fila[frente]->num;
        float capacidade = fila[frente]->capacidade;
        liberarLista(fila[frente]);
        frente++;

        for (Lista* v = grafo->listaAdj[u]; v != NULL; v = v->prox) {
            if (!visitado[v->num] && v->capacidade - v->fluxo > 0) {
                fila[tras++] = criarNo(v->num, v->capacidade);
                pai[v->num] = u;
                visitado[v->num] = true;

                if (v->num == sorvedouro) {
                    free(fila);
                    return true;  // Caminho aumentativo encontrado
                }
            }
        }
    }

    free(fila);
    return false;  // Não há mais caminhos aumentativos
}

// Função Ford-Fulkerson para encontrar o fluxo máximo
float fordFulkerson(Grafo* grafo, int fonte, int sorvedouro) {
    int* pai = (int*)malloc(grafo->v * sizeof(int));
    float fluxoMaximo = 0;

    while (BFS(grafo, fonte, sorvedouro, pai)) {
        float fluxoCaminho = FLT_MAX;

        // Encontrar a capacidade residual mínima ao longo do caminho aumentativo
        for (int v = sorvedouro; v != fonte; v = pai[v]) {
            int u = pai[v];
            for (Lista* lista = grafo->listaAdj[u]; lista != NULL; lista = lista->prox) {
                if (lista->num == v) {
                    fluxoCaminho = (fluxoCaminho < lista->capacidade - lista->fluxo) ? fluxoCaminho : (lista->capacidade - lista->fluxo);
                    break;
                }
            }
        }

        // Atualizar o fluxo ao longo do caminho aumentativo
        for (int v = sorvedouro; v != fonte; v = pai[v]) {
            int u = pai[v];
            for (Lista* lista = grafo->listaAdj[u]; lista != NULL; lista = lista->prox) {
                if (lista->num == v) {
                    lista->fluxo += fluxoCaminho;
                    break;
                }
            }

            // Adicionar aresta reversa (aresta residual)
            int encontrado = 0;
            for (Lista* lista = grafo->listaAdj[v]; lista != NULL; lista = lista->prox) {
                if (lista->num == u) {
                    lista->fluxo -= fluxoCaminho;
                    encontrado = 1;
                    break;
                }
            }

            if (!encontrado) {
                addAresta(grafo, v, u, fluxoCaminho);
            }
        }

        fluxoMaximo += fluxoCaminho;
    }

    gerarArquivo(grafo);
    free(pai);
    return fluxoMaximo;
}

void gerarArquivo(Grafo* grafo){
    int V = grafo->v,contaAresta=0;

    FILE *arquivo; // Ponteiro para o arquivo

    arquivo = fopen("C:\\Users\\Matheus\\Desktop\\Grafo novo.txt", "w");

    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return;
    }

    fprintf(arquivo,"%d\n",V);
    fprintf(arquivo,"%d\n",contaAresta);
    for (int i = 0; i < V; i++) {
        Lista* corrente = grafo->listaAdj[i];
        while (corrente != NULL) {
            fprintf(arquivo, "%d %d %.1f\n",i,corrente->num,corrente->capacidade);
            corrente = corrente->prox;
        }
    }
    // Fecha o arquivo
    fclose(arquivo);

    printf("Arquivo criado com sucesso.\n");
}

void liberarLista(Lista* lista){
    while (lista != NULL) {
        Lista* temp = lista;
        lista = lista->prox;
        free(temp);
    }
}