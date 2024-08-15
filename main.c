#include "raylib.h"
#include <stdio.h>
#include <string.h>

#define ALTURA 800
#define LARGURA 800
#define MAX_COMIDA 10
#define MAX_MATRIZ 40
#define MAX_TUNEL 9
#define MAX_COBRA 20
#define MAX_INPUT_CHARS 10

typedef struct strct_prop {
    int x, y;
} Proporcao;

//ESTRUTURAS
typedef struct strct_pos {
    int x, y;
} Posicao;

//COBRA
typedef struct strct_cobra{
    int tam;
    Posicao pos[MAX_COBRA];
} Cobra;

//BUTTON
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

void updateFrame(char mat[][MAX_MATRIZ], Proporcao prop); //atualiza algumas funcoes da tela para manter mais limpa

void iniButton(Button *button, Rectangle rect, Color cor, int acao, char *texto); //Inicializa os botoes

int checkButton(Button button); //Retorna se o mouse esta em cima do botao

void buttonsConfig(Button button, int *func); //Cuida da tela Menu e as acoes ao clicar em cada

void movePersonagem(Posicao *pos, int alt, int larg, int *dx, int *dy, char mat[][MAX_MATRIZ], int lado); //gerencia a movimentacao da cobra

int principal(Posicao pos, int lado); // desenha a cobra

void comida(int x, int y, int visivel, int lado); //desenha as comidas

void iniComidas(Comida *com, int lado, char mat[][MAX_MATRIZ]); // inicializa as comidas

int comeu(struct strct_comida com[], int pos, Posicao posi, int *cont, int lado); //Retorna se a comida foi pega

int leMapa(char nome[], Posicao *pos, Tunel tunel[], int *var, char mat[][MAX_MATRIZ], Proporcao *prop); //le os dados do mapa

void centraliza(const char *texto, int posY, int fontSize, Color cor);

void menuPause(bool *pauseFlag, Button pauseButtons[], int *func);

//MAIN
int main(void){

//VARIAVEIS DO PERSONAGEM
 Posicao pos;
 int dx = 0; //direção inicial   -1 esquerda 0 parado 1 direita
 int dy = 0; //direção inicial    -1 sobe 0 parado 1 desce
 int lado;

//VARIAVEIS DA COMIDA
 int comX;
 int comY;
 Comida com[MAX_COMIDA];
 int quantCom = 0;

//CONTADOR NA TELA
 char text[20] = {"0"};
 int func = 0;
 bool carregado; //flag para saber se o mapa foi carregado

 //BUTTONS
 Button menuButtons[4];
 Button mapaInput;
 Button selectMapa;
 Button pauseButtons[2];

 //MENU PAUSE
 bool pauseFlag;

//MAPA CARREGADO
    Tunel tunel[MAX_TUNEL];
    int quant;
    char mat[MAX_MATRIZ][MAX_MATRIZ];
    Proporcao prop;

//MAPA INPUT
 char mapaNome[MAX_INPUT_CHARS + 1] = "\0";      // NOTE: One extra space required for null terminator char '\0'
 int contaLetras = 0;
 bool mouseNoTexto = false;
 int framesCounter;
 /*
 menuButtons
 [0] - Novo Jogo
 [1] - Carregar fases
 [2] - HighScores
 [3] - Sair
 -----
 pauseButtons
 [0] - Salvar
 [1] - Volta Menu
 */

 //--------------------------------------------------------------------------------------
 //INICIALIZACOES
 srand(time(NULL));
 InitWindow(LARGURA, ALTURA, "SnakeGame");//Inicializa janela, com certo tamanho e título
 SetTargetFPS(10);// Ajusta a execução do jogo para 60 frames por segundo

 //menuButtons

 iniButton(&menuButtons[0], (Rectangle){LARGURA/2 - 125, ALTURA/2 - 200, 250, 50}, DARKBLUE, 1, "Novo Jogo");
 iniButton(&menuButtons[1], (Rectangle){LARGURA/2 - 125, ALTURA/2 - 100, 250, 50}, DARKBLUE, 2, "Fases");
 iniButton(&menuButtons[2], (Rectangle){LARGURA/2 - 125, ALTURA/2, 250, 50}, DARKBLUE, 3, "HighScores");
 iniButton(&menuButtons[3], (Rectangle){LARGURA/2 - 125, ALTURA/2 + 100, 250, 50}, RED, 4, "Sair");

 //pauseButtons

 iniButton(&pauseButtons[0], (Rectangle){LARGURA/2 - 125, ALTURA/2 - 100, 250, 50}, RED, 5, "Continuar");
 iniButton(&pauseButtons[1], (Rectangle){LARGURA/2 - 125, ALTURA/2, 250, 50}, RED, 0, "Voltar");


 //text inputs

 iniButton(&mapaInput, (Rectangle){LARGURA/2 - 125, ALTURA/2 - 25, 250, 50}, GRAY, 10, "");
 iniButton(&selectMapa, (Rectangle){LARGURA/2 - 100, ALTURA/2 + 50, 200, 50}, GRAY, 1, "Selecionar");

    //Laço principal do jogo
    while (!WindowShouldClose() && !IsKeyPressed(KEY_ESCAPE))
    {
        switch(func){

            case 0: //Menu
                quantCom = 0;
                dx = 0;
                dy = 0;
                pauseFlag = false;
                carregado = false;
            BeginDrawing();
                ClearBackground(RAYWHITE);
                for(int i = 0 ; i < 4 ; i++) buttonsConfig(menuButtons[i], &func);
            EndDrawing();
            break;

            case 1: //Jogo Rodando
                //Jogo Pausado
                menuPause(&pauseFlag, pauseButtons, &func);
                if(!pauseFlag){
                //CarregaMapa
                    if(!carregado){
                        if(strlen(mapaNome) == 0){
                            strcpy(mapaNome, "Mapa1.txt");
                        }
                        leMapa(mapaNome, &pos, tunel, &quant, mat, &prop);
                        lado = LARGURA/prop.x;
                        iniComidas(com, lado, mat);// Inicializa as Comidas
                        carregado = true;
                    }

                //Cuida da movimentacao da Cobra
                movePersonagem(&pos, ALTURA, LARGURA, &dx, &dy, mat, lado);

                //----------------------------------------------------------------------------------

                //Trata se o personagem passou pela comida
                if(comeu(com, quantCom, pos, &quantCom, lado)) snprintf(text, 20, "%d", quantCom);



                //----------------------------------------------------------------------------------
                // Atualiza a representação visual a partir do estado do jogo
                BeginDrawing();

                      updateFrame(mat, prop);
                      principal(pos, lado);//Desenha um Personagem, com posição, tamanho e cor
                      for(int i = 0 ;  i < MAX_COMIDA ; i++){
                        comida(com[i].x, com[i].y, com[i].visivel, lado);//Desenha as comidas
                      }
                      DrawText(text, 600, 600, 20, BLUE);//Contador de comidas
                      if(quantCom == MAX_COMIDA){
                            DrawText("Jogo Encerrado", LARGURA/2, ALTURA/2, 20, RED);
                      }
                EndDrawing();
                }
            break;

            case 2: //Carregar fases
                if(checkButton(mapaInput)) mouseNoTexto = true;
                else mouseNoTexto = false;

                if(mouseNoTexto){
                    SetMouseCursor(MOUSE_CURSOR_IBEAM);
                    int tecla = GetCharPressed();

                    while ( tecla > 0)
                    {
                        if((tecla >=32) && (tecla <= 125) && (contaLetras < MAX_INPUT_CHARS))
                        {
                            mapaNome[contaLetras] = (char)tecla;
                            mapaNome[contaLetras+1] = '\0';
                            contaLetras++;
                        }
                        tecla = GetCharPressed();
                    }
                    if(IsKeyPressed(KEY_BACKSPACE))
                    {
                        contaLetras--;
                        if(contaLetras < 0) contaLetras = 0;
                        mapaNome[contaLetras] = '\0';
                    }
                }
                else SetMouseCursor(MOUSE_CURSOR_DEFAULT);

                if (mouseNoTexto) framesCounter++;
                else framesCounter = 0;

                BeginDrawing();

                    centraliza("PONHA O MOUSE SOBRE A CAIXA DE TEXTO!", 300, 20,GRAY);
                    centraliza("ESCOLHA O MAPA. EX:", ALTURA/2 - 75, 20, GRAY);
                    centraliza("Mapa4.txt", ALTURA/2 - 50, 20,GRAY);
                    buttonsConfig(mapaInput, &func);

                    if(mouseNoTexto) DrawRectangleLines((int)mapaInput.rect.x, (int)mapaInput.rect.y, (int)mapaInput.rect.width, (int)mapaInput.rect.height, RED);
                    else DrawRectangleLines((int)mapaInput.rect.x, (int)mapaInput.rect.y, (int)mapaInput.rect.width, (int)mapaInput.rect.height, DARKGRAY);

                    DrawText(mapaNome, (int)mapaInput.rect.x + 5,(int)mapaInput.rect.y + 8, 30, MAROON);

                    if (mouseNoTexto)
                    {
                        if (contaLetras < MAX_INPUT_CHARS)
                        {
                            if (((framesCounter/20)%2) == 0) DrawText("_", (int)mapaInput.rect.x + 8 + MeasureText(mapaNome, 30), (int)mapaInput.rect.y + 12, 30, MAROON);
                        }
                        else centraliza("Tamanho MAXIMO de caracteres. Precione BACKSPACE", ALTURA/2 +30, 15, BLACK);
                    }
                    buttonsConfig(selectMapa, &func);
                EndDrawing();
            break;

            case 3: //HighScores

            break;

            case 4: //Sair

                BeginDrawing();
                    ClearBackground(RAYWHITE);
                    DrawText("Fechando...", LARGURA/2 - 40, ALTURA/2, 40, RED);
                EndDrawing();
                WaitTime(3);
                CloseWindow();

            break;

            case 5:
                func = 1;
                pauseFlag = false;
            break;


        }
    }
    CloseWindow();
    return 0;
}


//----------------------------------------------------------------------------------
//FUNCOES
void menuPause(bool *pauseFlag, Button pauseButtons[], int *func){
    if(IsKeyPressed(KEY_TAB)){
        if(*pauseFlag) *pauseFlag = false;
        else *pauseFlag = true;
    }
    if(*pauseFlag){
            printf("%p \n", func);
            printf("%d \n", *func);
            BeginDrawing();
            DrawRectangle(LARGURA/2- 150, ALTURA/2 - 150, 300, 250, DARKGRAY);
            for(int i  = 0 ; i < 2 ; i++) buttonsConfig(pauseButtons[i], func);
            EndDrawing();
        }
    }

void centraliza(const char *texto, int posY, int fontSize, Color cor) {
    int larguraTexto = MeasureText(texto, fontSize);  // Calcula a largura do texto
    int posX = (LARGURA / 2) - (larguraTexto / 2);  // Calcula a posição X para centralizar

    // Desenha o texto na posição calculada
    DrawText(texto, posX, posY, fontSize, cor);
}

int leMapa(char nome[], Posicao *pos, Tunel tunel[], int *var, char mat[][MAX_MATRIZ], Proporcao *prop){
    FILE *arq;
    int matX = 0, matY = 0;
    char lido = 'a', destino[100];

    sprintf(destino, "Mapas/%s", nome);

    if(!(arq = fopen(destino,"r"))){ // abre para escrita modo texto
        printf("Erro na abertura\n");
        return 1;
    }
    else{
        //LEITURA TAMANHO MAPA
        for(int i = 0 ; i < 1; i++){
            fscanf(arq, "%d %d\n", &prop->x, &prop->y);
            printf("x = %d \ny = %d\n", prop->x, prop->y);
            fscanf(arq, "%d\n", &*var);
        }
        printf("%d\n", *var);
        //LEITURA POSICOES TUNEIS
        for(int i = 0; i < *var; i++){
            fscanf(arq, "%d %d\n", &tunel[i].dir1, &tunel[i].dir2);
            printf("dir1 = %d \ndir2 = %d\n", tunel[i].dir1, tunel[i].dir2);
        }
        //LEITURA MAPA
        while(lido != EOF){
            lido = fgetc(arq);
            if(lido != '\n'){
                if(lido == 'S'){
                    pos->x = matX;
                    pos->y = matY;
                }
                mat[matX][matY] = lido;
                printf("%c", mat[matX][matY]);
                matX++;
            }else{
                mat[matX][matY] = lido;
                printf("%c", mat[matX][matY]);
                matY++;
                matX = 0;
            }
        }

        fclose(arq);
    }
    return 0;
}

void buttonsConfig(Button button, int *func){

    if(checkButton(button) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
        *func = button.acao; //caso precionado sua devida acao eh colocada na variavel func do main
    }
        DrawRectangleRec(button.rect, button.cor); //desenha o botao
        DrawText(button.texto, button.rect.x + button.rect.width / 2 - MeasureText(button.texto, 20) / 2, button.rect.y + button.rect.height / 2 - 20 / 2, 20, WHITE);//desenha o texto do botao
        if(checkButton(button)) DrawRectangleLines((int)button.rect.x, (int)button.rect.y, (int)button.rect.width, (int)button.rect.height, RED);//caso selecionado bordas vermelhas
            else DrawRectangleLines((int)button.rect.x, (int)button.rect.y, (int)button.rect.width + 3, (int)button.rect.height + 3, DARKGRAY);

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

void updateFrame(char mat[][MAX_MATRIZ], Proporcao prop){
    ClearBackground(LIGHTGRAY);// Limpa a tela e define cor de fundo
    int tamX = LARGURA/prop.x;
    int tamY = ALTURA/prop.y;

    for (int i = 0; i < prop.x; i ++) {
        for (int a = 0; a < prop.y; a ++) {
            if (mat[i][a] == '#') {
                DrawRectangle((tamX * i) , (tamY * a), tamX - 1, tamY - 1, RED);
            } else if (mat[i][a] != '\0') {
                DrawRectangle((tamX * i) , (tamY * a), tamX - 1, tamY - 1, WHITE);
            }
        }
    }

}

int comeu(struct strct_comida com[], int pos, Posicao posi, int *quantCom, int lado){
    com[pos].visivel = 1;
    if(com[pos].x == (posi.x * lado) && com[pos].y == (posi.y * lado) && com[pos].visivel == 1){
        com[pos].visivel = 0;
        *quantCom = *quantCom + 1;
        return 1;
    }
    return 0;
}
//Inicia todas as comidas, e suas posicoes
void iniComidas(struct strct_comida com[], int lado, char mat[][MAX_MATRIZ]){
    int auxX, auxY, flag;
    for(int i = 0 ; i < MAX_COMIDA ; i++){
        flag = 1;
        while(flag){ //para impedir que duas comidas tenham a mesma posição
            flag =0;
            do{
                auxX = rand()%LARGURA/lado * lado;
                auxY = rand()%ALTURA/lado * lado;
            } while(mat[auxX/lado][auxY/lado] != ' '); //Comidas so nascerao em posicoes livres do mapa
            for(int j=0; j<i;j++){
                if(auxX==com[j].x && auxY == com[j].y){
                    flag=1;
                    break;
                }
            }
        }
            com[i].x = auxX;
            com[i].y = auxY;
            com[i].visivel = 0;
    }
}

void movePersonagem(Posicao *pos, int alt, int larg, int *dx, int *dy, char mat[][MAX_MATRIZ], int lado){

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
    tempX = (pos->x * lado) + (lado * *dx);
    tempY = (pos->y * lado) + (lado * *dy);

    //Checa se eh possivel se mover nessa posicao
    if (!(tempX >= larg || tempX < 0 || tempY >= alt || tempY < 0) && mat[pos->x][pos->y] != '#') {
        pos->x = pos->x + *dx;
        pos->y = pos->y + *dy;
    }
}
//Desenha a cobra !!SEM O VETOR DE TAMANHO
int principal(Posicao pos, int lado){
    DrawRectangle((pos.x * lado), (pos.y * lado), lado, lado, GREEN);
}
//Desenha as comidas
void comida(int x, int y, int visivel, int lado){
    if(visivel == 1){
       DrawRectangle(x, y, lado, lado, RED);
    }
}
