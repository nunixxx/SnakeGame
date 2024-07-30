#include "raylib.h"
#include <stdio.h>

#define ALTURA 800
#define LARGURA 800
#define LADO_PERSONAGEM 20
#define LADO_COMIDA 20
#define MAX_COMIDA 10

//ESTRUTURA COMIDA
typedef struct strct_comida{ // declaração de tipo: é global
 int x, y;
 int visivel;
 }Comida;

//PROTOTIPOS DAS FUNCOES
void movePersonagem(int *x, int *y, int alt, int larg, int *dx, int *dy);
int principal(int x, int y);
int inimigo(int x, int y, int dx, int dy);
int comida(int x, int y, int visivel);
void iniComidas(Comida *com);
int comeu(struct strct_comida com[], int pos, int x, int y, int *cont);

//MAIN
int main(void){

//VARIAVEIS DO PERSONAGEM
 int x = LARGURA/2 - 20;
 int y = LARGURA/2 - 20;
 int dx = 1;
 int dy = 0;

//VARIAVEIS DA COMIDA
 int comX;
 int comY;
 Comida com[MAX_COMIDA];
 int quantCom = 0;

//CONTADOR NA TELA
 char text[20] = {"0"};

 //--------------------------------------------------------------------------------------
 //INICIALIZACOES
 srand(time(NULL));
 InitWindow(LARGURA, ALTURA, "SnakeGame");//Inicializa janela, com certo tamanho e título
 SetTargetFPS(10);// Ajusta a execução do jogo para 60 frames por segundo
 iniComidas(com);// Inicializa as Comidas


    //Laço principal do jogo
    while (!WindowShouldClose())
    {
    //Cuida da movimentacao da Cobra
    movePersonagem(&x, &y, ALTURA, LARGURA, &dx, &dy);

    //----------------------------------------------------------------------------------
    //Trata se o personagem passou pela comida
    for(int i = 0 ;  i < MAX_COMIDA ; i++){
        if(comeu(com, i, x, y, &quantCom)){
            comida(com[i].x, com[i].y, com[i].visivel);
            snprintf(text, 20, "%d", quantCom);
        }

    }

    //----------------------------------------------------------------------------------
    // Atualiza a representação visual a partir do estado do jogo
    BeginDrawing();
          for (int i = 0; i < ALTURA; i += 20) {
                for (int a = 0; a < LARGURA; a += 20) {
                    DrawRectangle(i, a, 19, 19, WHITE);
                }
            }
          ClearBackground(LIGHTGRAY);//Limpa a tela e define cor de fundo
          principal(x, y);//Desenha um Personagem, com posição, tamanho e cor
          for(int i = 0 ;  i < MAX_COMIDA ; i++){
            comida(com[i].x, com[i].y, com[i].visivel);//Desenha as comidas
          }
          DrawText(text, 600, 600, 20, BLUE);//Contador de comidas
          if(quantCom == MAX_COMIDA){
                DrawText("Jogo Encerrado", LARGURA/2, ALTURA/2, 20, RED);
          }
    EndDrawing();
    }
    CloseWindow();
    return 0;
}

//----------------------------------------------------------------------------------
//FUNCOES
int comeu(struct strct_comida com[], int pos, int x, int y, int *quantCom){
    if(com[pos].x == x && com[pos].y == y && com[pos].visivel == 1){
        com[pos].visivel = 0;
        *quantCom = *quantCom + 1;
        return 1;
    }
    return 0;
}
//Inicia todas as comidas, e suas posicoes
void iniComidas(struct strct_comida com[]){
    for(int i = 0 ; i < MAX_COMIDA ; i++){
        com[i].x = rand()%LARGURA/20 * LADO_COMIDA;
        com[i].y = rand()%ALTURA/20 * LADO_COMIDA;
        com[i].visivel = 1;
    }
}

void movePersonagem(int *x, int *y, int alt, int larg, int *dx, int *dy){

    int tempX, tempY; // Salvam temporariamente os valores das posicoes X, Y

    //IF para checar a validade do comando de movimento
    if ((IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) && *dx != -1) {
        *dx = 1;
        *dy = 0;
    }
    if ((IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) && *dx != 1) {
        *dx = -1;
        *dy = 0;
    }
    if ((IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) && *dy != 1) {
        *dx = 0;
        *dy = -1;
    }
    if ((IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) && *dy != -1) {
        *dx = 0;
        *dy = 1;
    }

    //calcula nova posicao
    tempX = *x +(20 * *dx);
    tempY = *y +(20 * *dy);

    //Checa se eh possivel se mover nessa posicao
    if (!(tempX >= larg || tempX < 0 || tempY >= alt || tempY < 0)) {
        *x = tempX;
        *y = tempY;
    }
}
//Desenha a cobra !!SEM O VETOR DE TAMANHO
int principal(int x, int y){
    DrawRectangle(x, y, LADO_PERSONAGEM, LADO_PERSONAGEM, GREEN);
}
//Desenha as comidas
int comida(int x, int y, int visivel){
    if(visivel == 1){
       DrawRectangle(x, y, LADO_PERSONAGEM, LADO_PERSONAGEM, RED);
    }
}
