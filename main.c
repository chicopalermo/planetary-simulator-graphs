// Trabalho 4 da disciplina de Estrutura de Dados A
//
// Necessita ter instalado a biblioteca allegro5
//
// para compilar este programa manualmente em linux:
// gcc -Wall -o main main.c tela.c grafo.c -lallegro_font -lallegro_color -lallegro_ttf -lallegro_primitives -lallegro -lm
//

// ligacoes interplanetarias
#define LIGACOES

// o tamanho da tela, em pixels
#define LARGURA_TELA 860
#define ALTURA_TELA  640

#define _CRT_NONSTDC_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS

#include "tela.h"
#include "grafo.h"

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>

// algumas constantes
#define CONST_GRAVIDADE 1000.0
#define CONST_MOLA 500.0
#define CONST_NIVEL 40.0

// o tipo de dados para representar um vetor 2d

typedef struct {
  float x;
  float y;
} vet2d;

// calcula a distancia entre as pontas de dois vetores
float v2d_dist(vet2d p1, vet2d p2) {
  return hypotf(p1.x - p2.x, p1.y - p2.y);
}

// soma dois vetores
vet2d v2d_som(vet2d p1, vet2d p2) {
  return (vet2d) { p1.x + p2.x, p1.y + p2.y };
}

// subtrai dois vetores
vet2d v2d_sub(vet2d p1, vet2d p2) {
  return (vet2d) { p1.x - p2.x, p1.y - p2.y };
}

// multiplica um vetor por um escalar
vet2d v2d_mul_f(vet2d p, float f) {
  return (vet2d) { p.x* f, p.y* f };
}

// calcula o m?dulo (comprimento) de um vetor
float v2d_mod(vet2d p) {
  return hypotf(p.x, p.y);
}

// calcula o vetor unit?rio (mesma dire??o, m?dulo 1) de um vetor
vet2d v2d_uni(vet2d p) {
  return v2d_mul_f(p, 1 / v2d_mod(p));
}

// calcula a media ponderada entre 2 vetores
vet2d v2d_med_p(vet2d v1, float p1, vet2d v2, float p2) {
  vet2d vp1 = v2d_mul_f(v1, p1);
  vet2d vp2 = v2d_mul_f(v2, p2);
  vet2d v = v2d_som(vp1, vp2);
  v = v2d_mul_f(v, 1 / (p1 + p2));
  return v;
}

// o tipo de dados para representar um planeta
typedef struct {
  vet2d pos;
  vet2d vel;
  float massa;
  int cor;
  vet2d forca;  // calculado a cada vez
  float raio;   // calculado a partir da massa
} planeta;

planeta* planeta_cria(vet2d pos, vet2d vel, float massa, int cor){
  planeta* p = (planeta*)malloc(sizeof(planeta));
  p->pos = pos;
  p->vel = vel;
  p->massa = massa;
  p->cor = cor;
  p->raio = sqrtf(massa) * 2;  // a ?rea ? proporcional ? massa
  return p;
}

void planeta_destroi(planeta* p) {
  if (p != NULL) free(p);
}
// verifica se dois planetas est?o sobrepostos
bool planeta_bate(planeta* p1, planeta* p2) {
  return v2d_dist(p1->pos, p2->pos) < p1->raio + p2->raio;
}

// Calcula a nova posi??o do planeta *p, ap?s delta_t segundos
// Sabe-se a posic?o e velocidade atuais, e a for?a incidente sobre o planeta.
void planeta_move(planeta* p, float delta_t) {
    // f = m * a
  vet2d acel = v2d_mul_f(p->forca, 1 / p->massa);
    // v = v + a * dt
  vet2d dv = v2d_mul_f(acel, delta_t);
  p->vel = v2d_som(p->vel, dv);
    // p = p + v * dt
  vet2d dp = v2d_mul_f(p->vel, delta_t);
  p->pos = v2d_som(p->pos, dp);
}

// retorna um planeta que ? a fus?o dos dois planetas fornecidos
// calcula posi??o e velocidade como a m?dia ponderada pela massa
// a cor resultante ? a cor do primeiro planeta (? meio chato de fundir as cores com a representa??o usada
planeta* planeta_funde(planeta* p1, planeta* p2) {
  return planeta_cria(
    v2d_med_p(p1->pos, p1->massa, p2->pos, p2->massa),
    v2d_med_p(p1->vel, p1->massa, p2->vel, p2->massa),
    p1->massa + p2->massa,
    p1->cor
  );
}

// estrutura de dados que cont?m todo o necess?rio para o jogo.
// a main declara uma vari?vel desse tipo, e
// um ponteiro para essa estrutura ? passado entre as fun??es
#define MAX_PLANETAS 4
typedef struct {
  grafo* grafo;
  // verdadeiro se ? para terminar o programa
  bool terminou;
  // entradas do teclado
  int entradas[3];
  // qual comando deve procurar
  int comando;
  // tamanho do universo conhecido, valores minimos e maximos de coordenadas onde ja foi visto um planeta
  vet2d mini;
  vet2d maxi;
  // escala do desenho para que o universo caiba na tela
  float escala;
  double pontuacao;
} jogo;

typedef struct {
  double pontuacao;
  char nome[10];
} ranking;

// inicializa a estrutura do jogo
void jogo_ini(jogo* j) {
  j->terminou = false;
  j->grafo = grafo_cria();
  j->comando = 1;
  j->pontuacao = 0;

  srand(time(NULL));
  int num_planetas = 3 + (rand() % 4); // 8 e 26

  for (int i = 0; i < num_planetas; i++) {
    int cor;
    do {
      cor = 1 + (rand() % 9);
    } while (cor == 5); // ignora a cor do universo (preto)
    float massa = 5 + (rand() % 60); 
    float pos_x = rand() % LARGURA_TELA;
    float pos_y = rand() % ALTURA_TELA;
    float vel_x = (rand() % 15) - 10;
    float vel_y = (rand() % 15) - 10;
    grafo_insere_no(j->grafo, planeta_cria((vet2d) { pos_x, pos_y }, (vet2d) { vel_x, vel_y }, massa, cor));
  }

  // inicializa o tamanho do universo como um ponto no centro da tela (vai ser alterado depois)
  j->mini = j->maxi = (vet2d){ LARGURA_TELA / 2, ALTURA_TELA / 2 };
  j->escala = 1;
}

ranking* cria_ranking() {
  ranking* rank = (ranking*)malloc(10 * sizeof(ranking));
  for (int i = 0; i < 10; i++) {
    rank[i].pontuacao = 0;
  }
  return rank;
}

// fun??o auxiliares usada por main, definidas abaixo
void desenha_tela(jogo* j);
void processa_entradas(jogo* j);
void avanca_jogo(jogo* j, double delta_t);
void fim(jogo* j, ranking* ranking);
void tela_final(jogo* j, ranking* ranking);
// A fun??o principal

int main(void) {
  printf("tecle enter\n");
  while (getchar() != '\n');

  // inicializa a tela gr?fica
  tela_inicio(LARGURA_TELA, ALTURA_TELA, "Jogo");

  // inicializa a estrutura que representa o estado do jogo
  jogo j;
  ranking* ranking = cria_ranking();
  jogo_ini(&j);

  // la?o principal
  while (!j.terminou) {
    // le entradas do usu?rio
    processa_entradas(&j);

    // evolui o estado do jogo
    avanca_jogo(&j, SEGUNDOS_POR_QUADRO);

    // desenha um quadro
    desenha_tela(&j);
  }
  // encerra a tela gr?fica
  fim(&j, ranking);
  while (tela_le_tecla() == '\0') {
    tela_final(&j, ranking);
  }
  tela_fim();

  return 0;
}

void tela_final(jogo* j, ranking* ranking) {
  int x1 = 0, y1 = 0, x2 = LARGURA_TELA, y2 = ALTURA_TELA;
    
  char pontos[20], texto[50];
  tela_retangulo(x1, y1, x2, y2, 1, branco, branco);

  sprintf(pontos, "%.2f", j->pontuacao);
  sprintf(texto, "Seus Pontos");

  tela_texto((x1 + x2) / 2, 40, 26, preto, texto);
  tela_texto((x1 + x2) / 2, 75, 26, preto, pontos);

  sprintf(texto, "Ranking");
  tela_texto((x1 + x2) / 2, 130, 26, preto, texto);

  int posicao = 165;
  for (int i = 0; i < 9; i++) {
    sprintf(pontos, "%.2f", ranking[i].pontuacao);
    if (ranking[i].pontuacao != 0) {
      tela_texto_dir((x1 + x2) / 5, posicao, 26, preto, ranking[i].nome);
      tela_texto((x1 + x2) / 2, posicao + 15, 26, preto, pontos);
      posicao += 40;
    }
  }
  tela_atualiza();
}

void calcula_pontos(jogo* j, ranking* ranking) {
  char nome[20], aux_nome[20];
  int tam = 0, i = 0, aux_pontos = 0, player = 0;
  FILE* file;

  file = fopen("ranking.txt", "a");
  if (file == NULL) {
    printf("N�o foi possivel abrir o arquivo. \n");
    exit(1);
  }

  printf("\nDigite seu nome: ");
  scanf("%s", nome);
  fprintf(file, "%s %lf", nome, j->pontuacao);
  fclose(file);

  file = fopen("ranking.txt", "r");
  do {
    fscanf(file, "%s %lf", ranking[i].nome, &ranking[i].pontuacao);
    tam++;
    i++;
  } while (!feof(file));
  fclose(file);

  while (player + 1 != tam) {
    for (int i = player + 1; i <= tam; i++) {
      if (ranking[player].pontuacao < ranking[i].pontuacao) {
        aux_pontos = ranking[player].pontuacao;
        strcpy(aux_nome, ranking[player].nome);
        ranking[player].pontuacao = ranking[i].pontuacao;
        strcpy(ranking[player].nome, ranking[i].nome);
        ranking[i].pontuacao = aux_pontos;
        strcpy(ranking[i].nome, aux_nome);
      }
    }
    player++;
  }
  file = fopen("ranking.txt", "w");
  for (int j = 0; j < tam; j++) {
    fprintf(file, "%s %lf\n", ranking[j].nome, ranking[j].pontuacao);
  }
  fclose(file);
  return;
}

void fim(jogo* j, ranking* ranking) {
  for (int i = 0; i < grafo_num_nos(j->grafo); i++) {
    planeta* copia = grafo_remove_no(j->grafo, i);
    planeta_destroi(copia);
  }
  calcula_pontos(j, ranking);
  grafo_destroi(j->grafo);
}

void verifica_planeta(jogo* j, int c) {
  if (c >= 0 && c <= 26) {
    j->entradas[j->comando - 1] = c;
    j->comando++;
  } else if (c != -97) {
    j->comando = 1;
  }
}

void verifica_peso(jogo* j, int c) {
  if (c >= 0 && c < 10) {
    if (c == 0) {
      grafo_remove_aresta(j->grafo, j->entradas[0], j->entradas[1]);
    } else {
      grafo_insere_aresta(j->grafo, j->entradas[0], j->entradas[1], c);
    }
    j->comando = 1;
  } else if (c != -48) {
    j->comando = 1;
  }
}

// verifica entradas
void processa_entradas(jogo* j) {
  char comando = tela_le_tecla();
  switch (j->comando) {
  case 1:
    verifica_planeta(j, comando - 'a');
    break;
  case 2:
    verifica_planeta(j, comando - 'a');
    break;
  case 3:
    verifica_peso(j, comando - '0');
    break;
  }
  // termina depois de um minuto
  if (relogio() > 60) {
    j->terminou -= true;
  }
}

void verifica_colisoes(jogo* j) {
de_novo:
  for (int p1 = 0; p1 < grafo_num_nos(j->grafo); p1++) {
    planeta* pl1 = grafo_no(j->grafo, p1);
    for (int p2 = p1 + 1; p2 < grafo_num_nos(j->grafo); p2++) {
      planeta* pl2 = grafo_no(j->grafo, p2);
      if (!planeta_bate(pl1, pl2)) continue;
        // catastrofa!
        planeta* p = planeta_funde(pl1, pl2);
        // tem que remover de tras pra diante, pela forma como a remocao ? feita
        planeta* copia = grafo_remove_no(j->grafo, p2);
        planeta_destroi(copia);
        copia = grafo_remove_no(j->grafo, p1);
        planeta_destroi(copia);
        grafo_insere_no(j->grafo, p);
        goto de_novo; // recome?a, tem que testar tudo de novo
        // ex. 1: faz sem goto - o codigo fica melhor?
        // ex. 2: chamar verifica_colisoes recursivamente seria equivalente?
      }
  }
}

void verifica_distancia(jogo* j) {
  planeta* pl1, * pl2;
  int distancia = 0, m_distancia = 0;
  bool primeira = false;

  for (int p = 0; p < grafo_num_nos(j->grafo); p++) {
    //Pega um planeta para analisar
    pl1 = grafo_no(j->grafo, p);
    //Verifica seus vizinhos
    for (int i = p + 1; i < grafo_num_nos(j->grafo); i++) {
      pl2 = grafo_no(j->grafo, i); //Pegou o planeta do grafo, agora deve comparar com o nosso outro

      distancia = v2d_dist(pl1->pos, pl2->pos);
      //pega a menor distancia
      if (primeira == false) {
        m_distancia = distancia;
        primeira = true;
      } else if (distancia < m_distancia) {
        m_distancia = distancia;
      }
      //verifica se o planeta deve morrer
      if (m_distancia > 600) {
        grafo_remove_no(j->grafo, p);
      }
    }
  }
  if (grafo_num_nos(j->grafo) == 1) {
    grafo_remove_no(j->grafo, 0);
    j->terminou -= true;
  }
}

// retorna um vetor com a for?a resultante da gravidade entre os 2 planetas
// o vetor ? a for?a que "puxa" pl2, deve ser invertido para pl1
vet2d calcula_gravidade(planeta* pl1, planeta* pl2) {
  // lei da gravitacao universal de Newton
  float f = CONST_GRAVIDADE * pl1->massa * pl2->massa
      / pow(v2d_dist(pl1->pos, pl2->pos), 2);
  // transforma em vetor, multiplicando pelo vetor unitario das difs de pos
  return v2d_mul_f(v2d_uni(v2d_sub(pl1->pos, pl2->pos)), f);
}

// retorna um vetor com a for?a resultante de uma ligacao interplanet?ria
// de n?vel n (entre 1 e 9)
// o vetor ? a for?a que atua sobre pl2, deve ser invertido para pl1
vet2d calcula_forca_interplanetaria(planeta* pl1, planeta* pl2, int n) {
  // a forca interplanetaria ? como uma mola.
  // o tamanho da mola ? proporcional ao n?vel
  // a constante da mola ? inversamente proporcional ao n?vel.
  // se a distancia entre os planetas for maior que a mola, a for?a aproxima
  // os planetas; se for menor, afasta
  float tamanho_mola = CONST_NIVEL * n;
  float constante_mola = CONST_MOLA / n;
  float d = v2d_dist(pl1->pos, pl2->pos) - tamanho_mola;
  if (d <= -CONST_NIVEL / 2) d += CONST_NIVEL / 2;
  else if (d >= CONST_NIVEL / 2) d -= CONST_NIVEL / 2;
  else d = 0;
  float f = d * constante_mola;
  // transforma em vetor, multiplicando pelo vetor unitario das difs de pos
  return v2d_mul_f(v2d_uni(v2d_sub(pl1->pos, pl2->pos)), f);
}

void calcula_forcas(jogo* j) {
  for (int p = 0; p < grafo_num_nos(j->grafo); p++) {
    planeta* a = grafo_no(j->grafo, p);
    a->forca = (vet2d){ 0,0 };
  }
  // calcula gravidade
  for (int p1 = 0; p1 < grafo_num_nos(j->grafo); p1++) {
    planeta* pl1 = grafo_no(j->grafo, p1);
    for (int p2 = p1 + 1; p2 < grafo_num_nos(j->grafo); p2++) {
      planeta* pl2 = grafo_no(j->grafo, p2);
      vet2d forca = calcula_gravidade(pl1, pl2);
      pl1->forca = v2d_sub(pl1->forca, forca);
      pl2->forca = v2d_som(pl2->forca, forca);
    }
  }
#ifdef LIGACOES
  for (int p1 = 0; p1 < grafo_num_nos(j->grafo); p1++) {
    planeta* pl1 = grafo_no(j->grafo, p1);
    for (int p2 = p1 + 1; p2 < grafo_num_nos(j->grafo); p2++) {
      planeta* pl2 = grafo_no(j->grafo, p2);
      int peso = grafo_aresta(j->grafo, p1, p2);
      if (peso != GRAFO_ARESTA_INEXISTENTE) {
        vet2d forca = calcula_forca_interplanetaria(pl1, pl2, peso);
        pl1->forca = v2d_sub(pl1->forca, forca);
        pl2->forca = v2d_som(pl2->forca, forca);
      }
    }
  }
#endif
}

void movimenta_planetas(jogo* j, double delta_t) {
  planeta* pl1, * pl2;
  for (int p = 0; p < grafo_num_nos(j->grafo); p++) {
    planeta_move(grafo_no(j->grafo, p), delta_t);
    int distancia = 0;
    pl1 = grafo_no(j->grafo, p);

    for (int i = p + 1; i < grafo_num_nos(j->grafo); i++) {
      pl2 = grafo_no(j->grafo, i); 
      distancia = v2d_dist(pl1->pos, pl2->pos);
      j->pontuacao += ((distancia * relogio()) / 100000);
    }
  }
}

// fun??o que implementa a passagem do tempo -- atualiza a posicao da bolinha
void avanca_jogo(jogo* j, double delta_t) {
  verifica_colisoes(j);
  calcula_forcas(j);
  movimenta_planetas(j, delta_t);
  verifica_distancia(j);
}


// verifica se algum planeta est? fora do universo conhecido, e expande o universo se for o caso
// calcula a escala que deve ser usada nos desenhos para que o universo caiba na tela
void expande_universo(jogo* j){
  for (int p = 0; p < grafo_num_nos(j->grafo); p++) {
    planeta* a = grafo_no(j->grafo, p);
    if (a->pos.x < j->mini.x) j->mini.x = a->pos.x;
    if (a->pos.x > j->maxi.x) j->maxi.x = a->pos.x;
    if (a->pos.y < j->mini.y) j->mini.y = a->pos.y;
    if (a->pos.y > j->maxi.y) j->maxi.y = a->pos.y;
  }
  // escolhe a menor escala entre x e y, mantendo uma margem de 50 pixels
  float sx = (LARGURA_TELA - 2 * 50) / (j->maxi.x - j->mini.x);
  float sy = (ALTURA_TELA - 2 * 50) / (j->maxi.y - j->mini.y);
  j->escala = fminf(sx, sy);
}

// converte uma posicao no universo para coordenadas na tela
vet2d escala_pos(jogo* j, vet2d pos) {
  return (vet2d) {
    (pos.x - (j->maxi.x + j->mini.x) / 2)* j->escala + LARGURA_TELA / 2,
    (pos.y - (j->maxi.y + j->mini.y) / 2)* j->escala + ALTURA_TELA / 2
  };
}

// converte uma tamanho no universo para tamanho em pixels na tela
float escala_tam(jogo* j, float tam) {
  return tam * j->escala;
}

// fun??o que desenha uma tela
void desenha_tela(jogo* j) {
  expande_universo(j);

#ifdef LIGACOES
  for (int p1 = 0; p1 < grafo_num_nos(j->grafo); p1++) {
    planeta* pl1 = grafo_no(j->grafo, p1);
    for (int p2 = p1 + 1; p2 < grafo_num_nos(j->grafo); p2++) {
      planeta* pl2 = grafo_no(j->grafo, p2);
      int peso = grafo_aresta(j->grafo, p1, p2);
      if (peso != GRAFO_ARESTA_INEXISTENTE) {
        vet2d pos1 = escala_pos(j, pl1->pos);
        vet2d pos2 = escala_pos(j, pl2->pos);
        tela_linha(pos1.x, pos1.y, pos2.x, pos2.y, 2, branco);
      }
    }
  }
#endif
  // desenha os planetas (deveria obt?-los do grafo)
  for (int p = 0; p < grafo_num_nos(j->grafo); p++) {
    planeta* a = grafo_no(j->grafo, p);
    vet2d pos = escala_pos(j, a->pos);
    float raio = escala_tam(j, a->raio);

    tela_circulo(pos.x, pos.y, raio, 1, a->cor, a->cor);
    // o nome do planeta devia tar dentro da estrutura!
    char n[2] = "a";
    n[0] += p;
    tela_texto(pos.x, pos.y, 12, preto, n);
  }
  int cronometro = 60 - relogio();
  char tempo_string[10];
  char pontos[20];

  // snprintf(tempo_string, 10, "%s\n", cronometro);
  sprintf(pontos, "%.2f", j->pontuacao);

  tela_texto(LARGURA_TELA / 2, ALTURA_TELA - 610, 14, branco, tempo_string);
  tela_texto(LARGURA_TELA / 2, ALTURA_TELA - 30, 14, branco, pontos);

  // fim do desenho, faz ele aparecer na tela
  tela_atualiza();
}