/// grafo.h
/// API simples para grafos n�o orientados, ed2021a, benhur, dlsc, ufsm

#ifndef _GRAFO_H_   // evita insercao multipla
#define _GRAFO_H_

/// ------------------------------------------------------------ tipos e constantes

typedef struct grafo grafo;
// um define para poder representar o peso de uma aresta que n�o existe
#include <stdbool.h>
#include <limits.h>
#define GRAFO_ARESTA_INEXISTENTE INT_MIN
#define TAM 8

/// ---------------------------------------------------------- cria��o e destrui��o

// cria um novo grafo n�o orientado, vazio.
// retorna um ponteiro para o grafo criado.
grafo* grafo_cria(void);

// destroi o grafo, liberando a mem�ria ocupada por ele.
// o grafo n�o gerencia o uso de mem�ria pelos n�s, caso esse controle seja necess�rio, 
//   deve ser feito externo ao grafo.
void grafo_destroi(grafo* g);

/// ---------------------------------------------------------------- acesso aos n�s

// retorna o n�mero de n�s atualmente no grafo
int grafo_num_nos(grafo* g);

// retorna o n-�simo n� do grafo g
// n deve estar entre 0 e um a menos que o n�mero de n�s do grafo (retorna NULL se n n�o for v�lido)
void* grafo_no(grafo* g, int n);

// insere um novo n� no grafo
// retorna a identifica��o do n� inserido
// sua identifica��o ser� igual ao n�mero de n�s no grafo antes da inser��o
//   (se o grafo tem 3 n�s, a identifica��o do novo n� � 3)
int grafo_insere_no(grafo* g, void* no);

// remove o n-�simo n� do grafo g
// retorna o n� removido
// n deve estar entre 0 e um a menos que o n�mero de n�s no grafo (retorna NULL se n n�o for v�lido)
// os n�s ap�s n ser�o renomeados (n+1 passa a ser n, n+2 passa a ser n+1 etc)
// as arestas ligadas ao n� removido devem ser removidas do grafo
// as arestas que permanecem no grafo devem continuar ligando aos mesmos n�s que antes, mesmo que 
// tenham sido renomeados.
void* grafo_remove_no(grafo* g, int n);


/// ------------------------------------------------------------- acesso �s arestas
bool no_valido(grafo* g, int n);

// insere uma aresta entre o n� n1 e o n� n2, com o peso fornecido
// se j� existir tal aresta, seu peso � alterado para o fornecido
// se n1 ou n2 n�o forem identifica��es v�lidas de n�s, a aresta n�o � inserida
// se o peso for GRAFO_ARESTA_INEXISTENTE, a aresta � removida, se existir
void grafo_insere_aresta(grafo* g, int n1, int n2, int peso);

// retorna o peso da aresta entre os n�s n1 e n2
// se n�o existir tal aresta ou tais n�s, retorna GRAFO_ARESTA_INEXISTENTE
int grafo_aresta(grafo* g, int n1, int n2);

// remove a aresta entre os n�s n1 e n2, e retorna seu peso
// se n�o existir tal aresta ou tais n�s, retorna GRAFO_ARESTA_INEXISTENTE
int grafo_remove_aresta(grafo* g, int n1, int n2);

// inicia o percurso das arestas que partem do n� n no grafo g
// caso n seja -1, o percurso incluir� todas as arestas do grafo
// o acesso �s arestas se d� por meio de chamadas a grafo_proxima_aresta
// caso n n�o tenha arestas incidentes ou seja inv�liso, a pr�xima chamada
//   a grafo_proxima_aresta deve retornar GRAFO_ARESTA_INEXISTENTE
void grafo_percorre_arestas(grafo* g, int n);

// retorna a pr�xima aresta do percurso iniciado por uma chamada a grafo_percorre_arestas
// coloca em *n1 e *n2 os n�s que essa aresta interliga
// retorna o peso da aresta
// caso n�o existam mais arestas no percurso, n�o altera *n1 nem *n2 e retorna GRAFO_ARESTA_INEXISTENTE
// esta fun��o n�o deve ser chamada sem ter havido uma chamada anterior � grafo_percorre_arestas
// esta fun��o n�o deve ser chamada ap�s uma chamada a ela retornar GRAFO_ARESTA_INEXISTENTE nem
//   ap�s alguma chamada que altere o grafo, sem antes chamar grafo_percorre_arestas
int grafo_proxima_aresta(grafo* g, int* n1, int* n2);

#endif   // _GRAFO_H_
