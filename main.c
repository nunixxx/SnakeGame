#include "raylib.h"
#include <stdio.h>
#include <string.h>

#define ALTURA 800
#define LARGURA 800
#define MAX_COMIDA 20
#define MAX_MATRIZ 40
#define MAX_TUNEL 9
#define MAX_COBRA 20
#define MAX_INPUT_CHARS 10
#define MAX_INPUT_NUMS 3

//ESTRUTURAS

//PROPORCAO DO MAPA
typedef struct strct_prop {
    int x, y;
} Proporcao;

//POSICOES DENTRO DA MATRIZ
typedef struct strct_pos {
    int x, y;
} Posicao;

//COBRA
typedef struct strct_cobra{
    int tam; //Tamanho do corpo
    Posicao pos[MAX_COBRA]; // corpo
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
    bool iniFlag; //Se ja foi iniciado o tunel[x]
    Posicao pos[2]; //Posicoes dos tuneis com valor [x]
    int dir[2]; //direcoes que cada tunel tem
 } Tunel;

//PROTOTIPOS DAS FUNCOES

void updateFrame(char mat[][MAX_MATRIZ], Proporcao prop, Tunel tunel[], int quant); //atualiza algumas funcoes da tela para manter mais limpa

void iniButton(Button *button, Rectangle rect, Color cor, int acao, char *texto); //Inicializa os botoes

int checkButton(Button button); //Retorna se o mouse esta em cima do botao

void buttonsConfig(Button button, int *func); //Cuida da tela Menu e as acoes ao clicar em cada

int movePersonagem(Posicao *pos, int alt, int larg, int *dx, int *dy, char mat[][MAX_MATRIZ], int lado, Cobra corpo); //gerencia a movimentacao da cobra

int principal(Posicao pos, int lado); // desenha a cobra

void comida(int x, int y, int visivel, Texture2D comidaImg); //desenha as comidas

void iniComidas(Comida *com, int lado, char mat[][MAX_MATRIZ]); // inicializa as comidas

int comeu(struct strct_comida com[], int pos, Posicao posi, int *cont, int lado); //Retorna se a comida foi pega

int leMapa(char nome[], Posicao *pos, Tunel tunel[], int *quant, char mat[][MAX_MATRIZ], Proporcao *prop); //le os dados do mapa

void centraliza(const char *texto, int posY, int fontSize, Color cor); //centraliza textos na tela

void menuPause(bool *pauseFlag, Button pauseButtons[], int *func); //Carrega e gerencia o menu de pause

void entrouTunel(int quant, Tunel tunel[], Posicao *pos, char mat[][MAX_MATRIZ], int lado, int *dx, int *dy); //Gerencia a entrada no tunel

void formaNome(char mapaNome[], char mapaNum[]); //Concatena strings e forma o nome do mapa

//MAIN
int main(){

 //--------------------------------------------------------------------------------------
 //INICIALIZACOES
 srand(time(NULL));
 InitWindow(LARGURA, ALTURA, "SnakeGame");//Inicializa janela, com certo tamanho e título
 SetTargetFPS(10);// Ajusta a execução do jogo para 60 frames por segundo


//VARIAVEIS DO PERSONAGEM
 Cobra corpo; //corpo da cobra
 Posicao pos; //posicao da cabeca da cobra
 int dx = 0; //direção inicial   -1 esquerda 0 parado 1 direita
 int dy = 0; //direção inicial    -1 sobe 0 parado 1 desce
 int lado; //tamanho de 1 bloco

//VARIAVEIS DA COMIDA
 int comX; //posicao X da comida visivel
 int comY; //posicao Y da comida visivel
 Comida com[MAX_COMIDA]; //todas as comidas do mapa
 int quantCom = 0; //quantidade ja comida
 Texture2D comidaImg; //imagem da comida

//CONTADOR NA TELA
 char text[20] = {"0"}; //Contador de comidas/pontuacao
 int func = 0; //funcao que sera exibida na tela
 bool carregado; //flag para saber se o mapa foi carregado

 //BUTTONS
 Button menuButtons[4]; //botoes menu principal
 Button mapaInput; //escrita do mapa
 Button selectMapa; //envio do mapa
 Button pauseButtons[2]; //botes menu pause

 //MENU PAUSE
 bool pauseFlag; //jogo pausado?

//MAPA CARREGADO
 Tunel tunel[MAX_TUNEL] = {0}; //tuneis do mapa
 int quant; // quantia de tuneis
 char mat[MAX_MATRIZ][MAX_MATRIZ]; //matriz do mapa
 Proporcao prop; // proporcoes do mapa
 char fase[MAX_MATRIZ];

//MAPA INPUT
 char mapaNome[MAX_INPUT_CHARS + 1] = "\0"; //Nome do mapa carregado
 char mapaNum[MAX_INPUT_NUMS + 1] = "\0"; //Numero do mapa carregado
 int contaLetras = 0; //numero de char digitados
 bool mouseNoTexto = false; //flag para posicao do mouse
 int framesCounter; //estilizacao do input

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
        switch(func){// de acordo com a funcao selecionada abre uma tela

            case 0: //Menu
                strcpy(fase, "Fase ");
                quantCom = 0;
                dx = 0;
                dy = 0;
                pauseFlag = false;
                carregado = false;
                BeginDrawing();
                    ClearBackground(RAYWHITE);
                    centraliza("Snake Game", 100, 40, RED);
                    centraliza("...............................", 120, 40, GREEN);
                    for(int i = 0 ; i < 4 ; i++) buttonsConfig(menuButtons[i], &func);
                EndDrawing();
            break;

            case 1: //Jogo Rodando
                //Jogo Pausado
                menuPause(&pauseFlag, pauseButtons, &func);
                if(!pauseFlag)
                {
                //CarregaMapa
                    if(!carregado)//caso mapa nao estaja carregado
                    {
                        if(strlen(mapaNome) == 0){ //carrega mapa padrao
                            strcpy(mapaNome, "Mapa1.txt");
                            mapaNum[0] = '1';
                        }
                        leMapa(mapaNome, &pos, tunel, &quant, mat, &prop); //carrega o mapa selecionado
                        lado = LARGURA/prop.x; //quanto vale cada quadrante
                        corpo.pos[0].x = pos.x;
                        corpo.pos[0].y = pos.y;
                        iniComidas(com, lado, mat); // Inicializa as Comidas
                        strcat(fase, mapaNum);
                        carregado = true;
                        if(lado == 40) comidaImg = LoadTexture("images/maca40.png");
                        else comidaImg = LoadTexture("images/maca20.png");
                    }

                //Cuida da movimentacao da Cobra
                    int aux_x[MAX_COMIDA];
                    int aux_y[MAX_COMIDA];
                    for(int i= 0 ; i <quantCom; i++){
                        aux_x[i] = corpo.pos[i].x;
                        aux_y[i] = corpo.pos[i].y;
                    }
                    for(int i= 1 ; i <quantCom; i++){
                        corpo.pos[i].x= aux_x[i-1];
                        corpo.pos[i].y = aux_y[i-1];
                    }
                    corpo.pos[0].x = pos.x;
                    corpo.pos[0].y = pos.y;

                //Trata se a cobra entrou no tunel
                entrouTunel(quant, tunel, &pos, mat, lado, &dx, &dy);

                //----------------------------------------------------------------------------------

                //Trata se a cobra passou pela comida
                if(comeu(com, quantCom, pos, &quantCom, lado)) snprintf(text, 20, "%d", quantCom);



                //----------------------------------------------------------------------------------
                // Atualiza a representação visual a partir do estado do jogo
                if(movePersonagem(&pos, ALTURA, LARGURA, &dx, &dy, mat, lado, corpo))
                    {
                        for(int i = 0 ;  i < MAX_COMIDA ; i++){
                        if(pos.x == corpo.pos[i].x && pos.y == corpo.pos[i].y ){
                            printf("COLISAO");
                        }

                    }
                    BeginDrawing();

                          updateFrame(mat, prop, tunel, quant); //desenha o mapa

                          principal(pos, lado);//Desenha a cabeca da cobra

                          for(int i= 0 ; i <quantCom; i++){
                             principal(corpo.pos[i], lado); //desenha corpo da cobra

                          }

                          for(int i = 0 ;  i < MAX_COMIDA ; i++){
                            comida(com[i].x, com[i].y, com[i].visivel, comidaImg);//Desenha as comidas visiveis
                          }
                          DrawText(text, 605, 0, lado, BLUE);//Contador de comidas/pontuacao
                          DrawText(fase, 205, 0, lado, WHITE);
                          if(quantCom == MAX_COMIDA){
                                DrawText("Jogo Encerrado", LARGURA/2, ALTURA/2, 20, RED); //Mensagem fim de jogo
                          }
                    EndDrawing();
                    } else {
                        func = 0;
                        BeginDrawing();
                            ClearBackground(RAYWHITE);

                            centraliza("FIM DE JOGO", LARGURA/2 - 20, 40, RED);
                        EndDrawing();
                        WaitTime(3);
                    }
                }
            break;

            case 2: //Carregar fases
                if(checkButton(mapaInput)) mouseNoTexto = true; //mouse sobre Input
                else mouseNoTexto = false; //mouse nao esta no input

                if(mouseNoTexto){
                    SetMouseCursor(MOUSE_CURSOR_IBEAM);
                    int tecla = GetCharPressed(); //pega a tecla precionada

                    while ( tecla > 0)
                    {
                        if((tecla >=48) && (tecla <= 57) && (contaLetras < MAX_INPUT_NUMS)) //aceita apenas as teclas numericas 0 - 9
                        {
                            mapaNum[contaLetras] = (char)tecla;
                            mapaNum[contaLetras+1] = '\0';
                            contaLetras++; //salva ela na string
                        }
                        tecla = GetCharPressed(); //pega a proxima precionada
                    }
                    if(IsKeyPressed(KEY_BACKSPACE))//deleta ultimo caracter
                    {
                        contaLetras--;
                        if(contaLetras < 0) contaLetras = 0;
                        mapaNum[contaLetras] = '\0';
                    }
                }
                else SetMouseCursor(MOUSE_CURSOR_DEFAULT);

                if (mouseNoTexto) framesCounter++; //diminui incrementa frame quando em cima do input
                else framesCounter = 0;
                formaNome(mapaNome, mapaNum); //formata nome do mapa

                BeginDrawing();//desenha a tela
                    ClearBackground(RAYWHITE);
                    centraliza("PONHA O MOUSE SOBRE A CAIXA DE TEXTO!", 300, 20,GRAY);
                    centraliza("ESCOLHA O MAPA. EX:", ALTURA/2 - 75, 20, GRAY);
                    centraliza("Mapa4.txt", ALTURA/2 - 50, 20,GRAY);
                    buttonsConfig(mapaInput, &func);

                    if(mouseNoTexto) DrawRectangleLines((int)mapaInput.rect.x, (int)mapaInput.rect.y, (int)mapaInput.rect.width, (int)mapaInput.rect.height, RED); //borda vermelha quando cursor em cima
                    else DrawRectangleLines((int)mapaInput.rect.x, (int)mapaInput.rect.y, (int)mapaInput.rect.width, (int)mapaInput.rect.height, DARKGRAY); //volta para default

                    DrawText(mapaNome, (int)mapaInput.rect.x + 5,(int)mapaInput.rect.y + 8, 30, MAROON); // nome do mapa dentro do input

                    if (mouseNoTexto)
                    {
                        if (contaLetras < MAX_INPUT_CHARS)
                        {
                            if (((framesCounter/20)%2) == 0) DrawText("_", (int)mapaInput.rect.x + 8 + MeasureText(mapaNome, 30), (int)mapaInput.rect.y + 12, 30, MAROON); //fica piscando a _ enquanto escreve
                        }
                        else centraliza("Tamanho MAXIMO de caracteres. Precione BACKSPACE", ALTURA/2 +30, 15, BLACK);
                    }
                    buttonsConfig(selectMapa, &func);
                EndDrawing();
                if(mapaNome != NULL){
                    if(!leMapa(mapaNome, &pos, tunel, &quant, mat, &prop)){
                        centraliza("Mapa nao existe", ALTURA/2 + 100, 15, RED);
                    }
                }
            break;

            case 3: //HighScores

            break;

            case 4: //Sair

                BeginDrawing();
                    ClearBackground(RAYWHITE);
                    centraliza("Fechando...", ALTURA/2 - 20, 40, RED);
                EndDrawing();
                WaitTime(3);
                CloseWindow();

            break;

            case 5: // retomar o jogo e fechar menu pause
                func = 1;
                pauseFlag = false;
            break;


        }
    }
    UnloadTexture(comidaImg);
    CloseWindow();
    return 0;
}


//----------------------------------------------------------------------------------
//FUNCOES
void formaNome(char mapaNome[], char mapaNum[]){
    strcpy(mapaNome, "mapa");

    // Concatena a string variável
    strcat(mapaNome, mapaNum);

    // Concatena o sufixo
    strcat(mapaNome, ".txt");
}

void entrouTunel(int quant, Tunel tunel[], Posicao *pos, char mat[][MAX_MATRIZ], int lado, int *dx, int *dy){

     for(int i = 0 ; i < quant ; i++)
     {
        if(pos->x == tunel[i].pos[0].x && pos->y == tunel[i].pos[0].y)
            {
            pos->x = tunel[i].pos[1].x;
            pos->y = tunel[i].pos[1].y;
            switch(tunel[i].dir[1]){
                case 1:
                    *dy = -1;
                    *dx = 0;
                break;
                case 2:
                    *dy = 1;
                    *dx = 0;
                break;
                case 3:
                    *dy = 0;
                    *dx = -1;
                break;
                case 4:
                    *dy = 0;
                    *dx = 1;
                break;
            }
        }else if(pos->x == tunel[i].pos[1].x && pos->y == tunel[i].pos[1].y){
            pos->x = tunel[i].pos[0].x;
            pos->y = tunel[i].pos[0].y;
            switch(tunel[i].dir[0]){
                case 1:
                    *dy = -1;
                    *dx = 0;
                break;
                case 2:
                    *dy = 1;
                    *dx = 0;
                break;
                case 3:
                    *dy = 0;
                    *dx = -1;
                break;
                case 4:
                    *dy = 0;
                    *dx = 1;
                break;
            }
        }
    }
}

void menuPause(bool *pauseFlag, Button pauseButtons[], int *func){
    if(IsKeyPressed(KEY_TAB)){
        if(*pauseFlag) *pauseFlag = false;
        else *pauseFlag = true;
    }
    if(*pauseFlag){
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

int leMapa(char nome[], Posicao *pos, Tunel tunel[], int *quant, char mat[][MAX_MATRIZ], Proporcao *prop){
    FILE *arq;
    int matX = 0, matY = 0;
    char lido = 'a', destino[100];
    int quantTunel, idTunel;

    for(int i = 0 ;  i < *quant ; i++){
        tunel[i].iniFlag = false;
    }

    sprintf(destino, "Mapas/%s", nome);

    if(!(arq = fopen(destino,"r"))){ // abre para escrita modo texto
        printf("Erro na abertura\n");
        return 0;
    }
    else{
        //LEITURA TAMANHO MAPA
        for(int i = 0 ; i < 1; i++){
            fscanf(arq, "%d %d\n", &prop->x, &prop->y);
            printf("x = %d \ny = %d\n", prop->x, prop->y);
            fscanf(arq, "%d\n", &quantTunel);
        }
        *quant = quantTunel;
        printf("%d\n", *quant);
        //LEITURA POSICOES TUNEIS
        for(int i = 0; i < *quant; i++){
            fscanf(arq, "%d %d\n", &tunel[i].dir[0], &tunel[i].dir[1]);
            printf("dir1 = %d \ndir2 = %d\n", tunel[i].dir[0], tunel[i].dir[1]);
        }
        //LEITURA MAPA
        while(lido != EOF){
            lido = fgetc(arq);
            if(lido != '\n'){
                if(lido == 'S'){
                    pos->x = matX;
                    pos->y = matY;
                }else if(lido != '#' && lido != ' '){
                    idTunel = lido - '0';
                    if(!tunel[idTunel].iniFlag){
                        tunel[idTunel].pos[0].x = matX;
                        tunel[idTunel].pos[0].y = matY;
                        tunel[idTunel].iniFlag = true;
                    }else{
                        tunel[idTunel].pos[1].x = matX;
                        tunel[idTunel].pos[1].y = matY;
                    }
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
    return 1;
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

void updateFrame(char mat[][MAX_MATRIZ], Proporcao prop, Tunel tunel[], int quant){
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

    for(int i = 0 ; i < quant ; i++){
        DrawRectangle((tamX * tunel[i].pos[0].x) , (tamY * tunel[i].pos[0].y), tamX - 1, tamY - 1, DARKGREEN);
        DrawRectangle((tamX * tunel[i].pos[1].x) , (tamY * tunel[i].pos[1].y), tamX - 1, tamY - 1, DARKGREEN);
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

int movePersonagem(Posicao *pos, int alt, int larg, int *dx, int *dy, char mat[][MAX_MATRIZ], int lado, Cobra corpo){

    int tempX, tempY; // Salvam temporariamente os valores das posicoes X, Y
    bool tocou = true;

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

    for(int i = 0 ;  i < MAX_COMIDA ; i++){
        if(pos->x == corpo.pos[i].x && pos->y == corpo.pos[i].y && (*dx != 0 && *dy != 0)){
                printf("coli");
            tocou = false;
            i = MAX_COMIDA;
        }

    }

    //Checa se eh possivel se mover nessa posicao
    if (!(tempX >= larg || tempX < 0 || tempY >= alt || tempY < 0) && mat[pos->x][pos->y] != '#' && tocou) {
        pos->x = pos->x + *dx;
        pos->y = pos->y + *dy;
        return 1;
    } else return 0;
}
//Desenha a cobra !!SEM O VETOR DE TAMANHO
int principal(Posicao pos, int lado){
    DrawRectangle((pos.x * lado), (pos.y * lado), lado, lado, GREEN);
}
//Desenha as comidas
void comida(int x, int y, int visivel, Texture2D comidaImg){
    if(visivel == 1){
        DrawTexture(comidaImg, x, y , WHITE);
       //DrawTexture(comidaImg, x, y, RED);
    }
}
