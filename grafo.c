#include "grafo.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

typedef struct {
  int no;
  int peso;
  struct aresta* prox;
} aresta;

typedef struct {
  aresta* base;
  void* no;
  int id;
  struct vertice* prox;
} vertice;

struct grafo {
  vertice* nos;
  int n_planetas;
  int n_conexoes;
};

grafo* grafo_cria(void) {
  grafo* gr = (grafo*)malloc(sizeof(grafo));
  gr->n_planetas = 0;
  gr->n_conexoes = 0;
  gr->nos = NULL;
  return gr;
}

void grafo_destroi(grafo* g) {
  if (g != NULL) free(g);
}

int grafo_num_nos(grafo* g) {
  return g->n_planetas;
}

int grafo_insere_no(grafo* g, void* no) {
  int pos = grafo_num_nos(g);

  vertice* temp = (vertice*)malloc(sizeof(vertice));
    
  temp->no = no;
  temp->id = pos;
  temp->base = NULL;

  temp->prox = NULL;

  temp->prox = g->nos;
  g->nos = temp;

  g->n_planetas++;

  return pos;
}

void* grafo_no(grafo* g, int n) {
  vertice* list = g->nos;

  if (!no_valido(g, n))
    return NULL;

  while (list != NULL) {
    if (list->id == n) {
      return list->no;
    }
    list = list->prox;
  }
  return NULL;
}

bool no_valido(grafo* g, int n) {
  return (n >= 0 && n < grafo_num_nos(g));
}

void* grafo_remove_no(grafo* g, int n) {
  if (g->nos == NULL)
    return NULL;
  if (!no_valido(g, n))
    return NULL;

  vertice* anterior = NULL;
  vertice* temp = g->nos;

  while (temp != NULL && temp->id != n) {
    anterior = temp;
    temp = temp->prox;
  }
  if (temp == NULL)
    return NULL;
  aresta* aux = temp->base;

  // remove aresta do n�
  while (aux != NULL) {
    aresta* aux2 = aux->prox;
    grafo_remove_aresta(g, n, aux->no);
    aux = aux2;
  }

  vertice* temp2 = g->nos;
  while (temp2 != NULL && temp2->id != temp->id) {
    temp2->id -= 1;
    temp2 = temp2->prox;
  }
  if (anterior == NULL)
    g->nos = temp->prox;
  else
    anterior->prox = temp->prox;

  void* copia = temp->no;
  g->n_planetas--;
  free(temp);

  vertice* temp3 = g->nos;
  while (temp3 != NULL) {
    aresta* a = temp3->base;
    while (a != NULL && a->no > n) {
      a->no -= 1;
      a = a->prox;
    }
    temp3 = temp3->prox;
  }

  return copia;
}

void grafo_insere_aresta(grafo* g, int n1, int n2, int peso) {
  vertice* a = g->nos;
  vertice* b = g->nos;

  while (a != NULL) {
    if (a->id == n1) {
      break;
    }
    a = a->prox;
  }

  while (b != NULL) {
    if (b->id == n2) {
      break;
    }
    b = b->prox;
  }
  if (a == NULL || b == NULL)
    return;

  if (peso == GRAFO_ARESTA_INEXISTENTE) {
    if (grafo_aresta(g, n1, n2) != GRAFO_ARESTA_INEXISTENTE)
      grafo_remove_aresta(g, n1, n2);
      return;
  }

  aresta* list1 = a->base;
  aresta* list2 = b->base;

  while (list1 != NULL) {
    if (list1->no == n2) {
      list1->peso = peso;
      while (list2 != NULL) {
        if (list2->no == n1) {
          list2->peso = peso;
          break;
        }
        list2 = list2->prox;
      }
      return;
    }
    list1 = list1->prox;
  }

  aresta* temp = (aresta*)malloc(sizeof(aresta));
  aresta* temp2 = (aresta*)malloc(sizeof(aresta));

  temp->no = n1;
  temp2->no = n2;

  temp->peso = peso;
  temp2->peso = peso;

  temp->prox = NULL;
  temp2->prox = NULL;

  temp->prox = b->base;
  b->base = temp;

  temp2->prox = a->base;
  a->base = temp2;

  g->n_conexoes++;
}

int grafo_aresta(grafo* g, int n1, int n2) {
  vertice* a = g->nos;
  vertice* b = g->nos;

  while (a != NULL) {
    if (a->id == n1) {
      break;
    }
    a = a->prox;
  }

  while (b != NULL) {
    if (b->id == n2) {
      break;
    }
    b = b->prox;
  }

  if (a == NULL || b == NULL)
    return GRAFO_ARESTA_INEXISTENTE;

  aresta* list1 = a->base;

  while (list1 != NULL) {
    if (list1->no == n2) {
      return list1->peso;
    }
    list1 = list1->prox;
  }
  return GRAFO_ARESTA_INEXISTENTE;
}

int grafo_remove_aresta(grafo* g, int n1, int n2) {
  vertice* a = g->nos;
  vertice* b = g->nos;

  while (a != NULL) {
    if (a->id == n1) {
      break;
    }
    a = a->prox;
  }

  while (b != NULL) {
    if (b->id == n2) {
      break;
    }
    b = b->prox;
  }

  if (a == NULL || b == NULL)
    return;

  aresta* anterior = NULL;
  aresta* temp1 = a->base;
  aresta* temp2 = b->base;

  while (temp1 != NULL && temp1->no != n2) {
    anterior = temp1;
    temp1 = temp1->prox;
  }

  if (temp1 == NULL)
    return GRAFO_ARESTA_INEXISTENTE;

  int peso = temp1->peso;
  if (anterior == NULL)
    a->base = temp1->prox;
  else
    anterior->prox = temp1->prox;

  free(temp1);

  anterior = NULL;

  while (temp2 != NULL && temp2->no != n1) {
    anterior = temp2;
    temp2 = temp2->prox;
  }

  if (temp2 == NULL)
    return GRAFO_ARESTA_INEXISTENTE;

  if (anterior == NULL)
    b->base = temp2->prox;
  else
    anterior->prox = temp2->prox;

  free(temp2);

  return peso;
}
