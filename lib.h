#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <float.h>

typedef struct lista {
    int num;
    float capacidade,fluxo;
    int pai;
    struct lista* prox;
}Lista;

typedef struct grafo{
    int v;  //quantidade de vertices
    Lista **listaAdj;
}Grafo;


void menu();
Grafo* lerArquivo(Grafo* grafo);
Lista* criarNo(int num,float fluxoMax);
Grafo* criarGrafo(int v);
void addAresta(Grafo* grafo, int origem, int fim,float fluxoMax);
void printGrafo(Grafo* grafo);
bool BFS(Grafo* grafo, int source, int sink, int parent[]);
float fordFulkerson(Grafo* grafo, int source, int sink);
void liberarLista(Lista* lista);
void gerarArquivo(Grafo* grafo);