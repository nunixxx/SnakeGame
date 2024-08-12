#include "raylib.h"
#include <stdio.h>
#include <string.h>

#define ALTURA 600
#define LARGURA 600
#define LADO_PERSONAGEM 20
#define LADO_COMIDA 20
#define MAX_COMIDA 10
#define MAX_MATRIZ 40
#define MAX_TUNEL 9

//ESTRUTURAS
//Button
typedef struct strct_button{
    Rectangle rect; // Tamanho e posicao do botao
    Color cor; // Cor do botao
    int acao; // sua acao dentro do main
    char texto[30]; // Texto dentro do botao
} Button;

//COMIDA
typedef struct strct_comida{ // declaração de tipo: é global
 int x, y; // posicao
 int visivel; // estado
 }Comida;

 //TUNEL
 typedef struct strct_tunel{
    int dir1, dir2; //direcoes que cada tunel tem
 } Tunel;

//PROTOTIPOS DAS FUNCOES

void updateFrame(); //atualiza algumas funcoes da tela para manter mais limpa

void iniButton(Button *button, Rectangle rect, Color cor, int acao, char *texto); //Inicializa os botoes

int checkButton(Button button); //Retorna se o mouse esta em cima do botao

void buttonsState(Button button, int *func); //Cuida da tela Menu e as acoes ao clicar em cada

void movePersonagem(int *x, int *y, int alt, int larg, int *dx, int *dy); //gerencia a movimentacao da cobra

int principal(int x, int y); // desenha a cobra

int comida(int x, int y, int visivel); //desenha as comidas

void iniComidas(Comida *com); // inicializa as comidas

int comeu(struct strct_comida com[], int pos, int x, int y, int *cont); //Retorna se a comida foi pega

//int leMapa(char nome[], Posicao *pos, Tunel tunel[], int *var, char mat[][MAX]); //le os dados do mapa

//MAIN
int main(void){

//VARIAVEIS DO PERSONAGEM
 int x = LARGURA/2 - 20; //para iniciar no meio da tela
 int y = LARGURA/2 - 20; //para iniciar no meio da tela
 int dx = 0; //direção inicial   -1 esquerda 0 parado 1 direita
 int dy = -1; //direção inicial    -1 sobe 0 parado 1 desce

//VARIAVEIS DA COMIDA
 int comX;
 int comY;
 Comida com[MAX_COMIDA];
 int quantCom = 0;

//CONTADOR NA TELA
 char text[20] = {"0"};
 int func = 0;
 //BUTTONS
 Button buttons[4];
 /*
 [0] - Novo Jogo
 [1] - Carregar fases
 [2] - HighScores
 [3] - Sair
 */

 //--------------------------------------------------------------------------------------
 //INICIALIZACOES
 srand(time(NULL));
 InitWindow(LARGURA, ALTURA, "SnakeGame");//Inicializa janela, com certo tamanho e título
 SetTargetFPS(10);// Ajusta a execução do jogo para 60 frames por segundo
 iniComidas(com);// Inicializa as Comidas

 //buttons

 iniButton(&buttons[0], (Rectangle){LARGURA/2 - 125, ALTURA/2 - 200, 250, 50}, DARKBLUE, 1, "Novo Jogo");
 iniButton(&buttons[1], (Rectangle){LARGURA/2 - 125, ALTURA/2 - 100, 250, 50}, DARKBLUE, 2, "Fases");
 iniButton(&buttons[2], (Rectangle){LARGURA/2 - 125, ALTURA/2, 250, 50}, DARKBLUE, 3, "HighScores");
 iniButton(&buttons[3], (Rectangle){LARGURA/2 - 125, ALTURA/2 + 100, 250, 50}, RED, 4, "Sair");

    //Laço principal do jogo
    while (!WindowShouldClose())
    {
        switch(func){

            case 0: //Menu
            BeginDrawing();
                for(int i = 0 ; i < 4 ; i++) buttonsState(buttons[i], &func);
            EndDrawing();
            break;

            case 1: //Jogo Rodando
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
                      updateFrame();
                      principal(x, y);//Desenha um Personagem, com posição, tamanho e cor
                      for(int i = 0 ;  i < MAX_COMIDA ; i++){
                        comida(com[i].x, com[i].y, com[i].visivel);//Desenha as comidas
                      }
                      DrawText(text, 600, 600, 20, BLUE);//Contador de comidas
                      if(quantCom == MAX_COMIDA){
                            DrawText("Jogo Encerrado", LARGURA/2, ALTURA/2, 20, RED);
                      }
                EndDrawing();
            break;

            case 2: //Carregar fases

            break;

            case 3: //HighScores

            break;

            case 4: //Sair

            break;


        }
    }
    CloseWindow();
    return 0;
}


//----------------------------------------------------------------------------------
//FUNCOES

void buttonsState(Button button, int *func){

    if(checkButton(button) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
        *func = button.acao; //caso precionado sua devida acao eh colocada na variavel func do main
    }
        ClearBackground(RAYWHITE);
        DrawRectangleRec(button.rect, button.cor); //desenha o botao
        DrawText(button.texto, button.rect.x + button.rect.width / 2 - MeasureText(button.texto, 20) / 2, button.rect.y + button.rect.height / 2 - 20 / 2, 20, WHITE);//desenha o texto do botao

}

int checkButton(Button button){
    return CheckCollisionPointRec(GetMousePosition(), button.rect); //retorna se o mouse esta em cima do botao
}

void iniButton(Button *button, Rectangle rect, Color cor, int acao, char *texto){
    button->rect = rect;
    button->cor = cor;
    button->acao = acao;
    strncpy(button->texto, texto, sizeof(button->texto) - 1);
}

void updateFrame(){

    for (int i = 0; i < ALTURA; i += 20) {
        for (int a = 0; a < LARGURA; a += 20) {
            DrawRectangle(i, a, 19, 19, WHITE); //desenha os quadrantes na tela
        }
    }
    ClearBackground(LIGHTGRAY);// Limpa a tela e define cor de fundo

}

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
    int auxX, auxY, flag;
    for(int i = 0 ; i < MAX_COMIDA ; i++){
        flag = 1;
        while(flag){ //para impedir que duas comidas tenham a mesma posição
            flag =0;
            auxX = rand()%LARGURA/20 * LADO_COMIDA;
            auxY = rand()%ALTURA/20 * LADO_COMIDA;
            for(int j=0; j<i;j++){
                if(auxX==com[j].x && auxY == com[j].y){
                    flag=1;
                    break;
                }
            }
        }
        com[i].x = auxX;
        com[i].y = auxY;
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
