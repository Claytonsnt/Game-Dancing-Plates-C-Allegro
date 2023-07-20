#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <stdlib.h>
#include <time.h>

#define NUM_PRATOS 8
const float FPS = 60;

const int SCREEN_W = 960;
const int SCREEN_H = 540;

const int JOGADOR_H = 100;
const int JOGADOR_W = 50;

const int DIS_LINE_X = 106;
const int DIS_LINE_Y = 106;

const int DIS_PRATO_X = 30;

int emPartida = 1; 

typedef struct Jogador{
	float x;
	int equilibrando;
	int mov_esq, mov_dir, vel;
	ALLEGRO_COLOR cor;
} Jogador;

typedef struct Prato{
	float x;
	float energia;  // alterar o rgb do prato
	int tempoParaAparecer;
	ALLEGRO_COLOR cor;
} Prato;

int random(int min, int max){
	return min + rand()%(max - min + 1);
}

float geraTempoPrato(int i) {
	if(i==3)
		return random(1, 3);
	if(i==4)
		return random(1, 3);
	if(i==2)
		return random(7, 11);
	if(i==5)
		return random(7, 11);	
	if(i==1)
		return random(16, 20);	
	if(i==6)
		return random(16, 20);
	if(i==0)
		return random(20, 24);	
	if(i==7)
		return random(20, 24);
}

void iniciaPratos(Prato pratos[]){
	int i=0;
	for(i = 0;i<NUM_PRATOS;i++){
		pratos[i].x = (i+1)*DIS_LINE_X; //Posição dos pratos e linhas
		pratos[i].tempoParaAparecer = geraTempoPrato(i);
		pratos[i].energia = 255.0;
		pratos[i].cor = al_map_rgb(255, pratos[i].energia, pratos[i].energia);  //Branco			
	}
	printf("Iniciou os Pratos!!!!");
}

void desenhaPratos(Prato pratos[], ALLEGRO_TIMER *contador){
	int i;
	//int r = 255, b = 255, g = 255;
	//DESENHA AS LINHAS
	for(i=0; i<NUM_PRATOS;i++){
		al_draw_line(pratos[i].x, DIS_LINE_Y, pratos[i].x,SCREEN_H - DIS_LINE_Y,
					pratos[i].cor, 2);
	}
	//DESENHA OS PRATOS
	for(i=0;i<NUM_PRATOS;i++){
		if(al_get_timer_count(contador) > pratos[i].tempoParaAparecer && pratos[i].energia > 4.0){
			pratos[i].cor = al_map_rgb(255, pratos[i].energia, pratos[i].energia);  //Cor do prato de acordo com a energia 
			al_draw_line(pratos[i].x - DIS_PRATO_X, DIS_LINE_Y, pratos[i].x + DIS_PRATO_X, DIS_LINE_Y,
					pratos[i].cor, 10);
			if(pratos[i].energia < 256.0){				
				pratos[i].energia = pratos[i].energia - 0.3;
				//printf("%d = %f\n", i, pratos[i].energia);	
			}
		}
		if(pratos[i].energia > 0.0 && pratos[i].energia < 4.0){
			pratos[i].cor = al_map_rgb(255, pratos[i].energia, pratos[i].energia);  //Branco
			al_draw_line(pratos[i].x - DIS_PRATO_X, SCREEN_H - DIS_LINE_Y, pratos[i].x + DIS_PRATO_X, SCREEN_H - DIS_LINE_Y,
				pratos[i].cor, 10);
			pratos[i].energia = pratos[i].energia - 0.3;
			//printf("%d = %f\n", i, pratos[i].energia);	
		}
				
		if(pratos[i].energia <= 0.0){			
			emPartida = 0;			
			printf("Acabou o jogo!!!!!!!!!!!!!\n");											
		}			
	}		
}	

void equilibra(Prato pratos[],Jogador j){
	int i=0;	
	for(i=0;i<NUM_PRATOS;i++){
		if(j.x > (pratos[i].x-5) && j.x < (pratos[i].x+5) && j.equilibrando == 1 && pratos[i].energia < 253.0) //Mais ou menos 5 em baixo e max de energia
		{
			pratos[i].energia = pratos[i].energia + 2;
			printf("MODIFICOU A ENERGIA DO PRATO [%d] PARA %f\n", i, pratos[i].energia);
		}		
	}
}

void desenhaCenario(){
	ALLEGRO_COLOR BKG_COLOR = al_map_rgb(65, 105, 225); // Azul real
	al_clear_to_color(BKG_COLOR);
}

void iniciaJogador(Jogador *j){
	j->x = SCREEN_W/2;
	j->equilibrando = 0;
	j->cor = al_map_rgb(176, 224, 230); //Azul polvora
	j->mov_esq = 0;
	j->mov_dir = 0;
	j->vel = 3;
}

void desenhaJogador(Jogador j){

	al_draw_filled_triangle(j.x, SCREEN_H - JOGADOR_H, j.x - JOGADOR_W/2, SCREEN_H,
   							j.x + JOGADOR_W/2,SCREEN_H, j.cor);
}

void movJogador(Jogador *j){

	int LIMITE_ESQ = j->x - JOGADOR_W/2;
	int LIMITE_DIR = j->x + JOGADOR_W/2;

	
	if(j->mov_esq){
		if(LIMITE_ESQ - j->vel > 0)
			j->x = j->x - j->vel;
			j->equilibrando = 0;
			printf("Andou esquerda\n");
	}
	if(j->mov_dir){
		if(LIMITE_DIR + j->vel < SCREEN_W)
			j->x = j->x + j->vel;
			j->equilibrando = 0;
			printf("Andou direita\n");
	}	
}

void pontuacaoRecorde(int pontuacao){

	FILE *arq;
	int recorde;
	ALLEGRO_FONT *size_32 = al_load_font("arial.ttf", 32, 1);

	arq = fopen("Recorde.txt", "r");
	fscanf(arq, "%d", &recorde);

	if(recorde > pontuacao){
		al_draw_textf(size_32, al_map_rgb(218, 217, 47), SCREEN_W/2, (SCREEN_H/4)+210, ALLEGRO_ALIGN_RIGHT, "RECORDE: %d", recorde); //Amarelo
	}
	else if(recorde < pontuacao){
		recorde = pontuacao;
		al_draw_textf(size_32, al_map_rgb(255, 0, 0), SCREEN_W - 300, (SCREEN_H-100), ALLEGRO_ALIGN_RIGHT, "NOVO RECORDE!"); //Vermelho
		al_draw_textf(size_32, al_map_rgb(218, 217, 47), SCREEN_W/2, (SCREEN_H/4)+210, ALLEGRO_ALIGN_RIGHT, "RECORDE: %d", recorde); //Amarelo
		arq = fopen("Recorde.txt", "w");
		fprintf(arq, "%d", recorde);
	}
	fclose(arq);
}


int main(int argc, char **argv){


	
	ALLEGRO_DISPLAY *display = NULL;	
	ALLEGRO_TIMER *timer = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *contador = 0;

	srand(time(NULL));



	
	//inicializa o Allegro
	if(!al_init()) {
		fprintf(stderr, "failed to initialize allegro!\n");
		return -1;
	}
	
	//cria um temporizador que incrementa uma unidade a cada 1.0/FPS segundos
    timer = al_create_timer(1.0 / FPS);
    if(!timer) {
		fprintf(stderr, "failed to create timer!\n");
		return -1;
	}	

	//cria um contador de segundos.
	contador = al_create_timer(1.0);
	if(!contador){
		fprintf(stderr, "failed to create timer!\n");
		return -1;
	}	
	
    //inicializa o módulo de primitivas do Allegro
    if(!al_init_primitives_addon()){
		fprintf(stderr, "failed to initialize primitives!\n");
        return -1;
    }	
	
	//inicializa o modulo que permite carregar imagens no jogo
	if(!al_init_image_addon()){
		fprintf(stderr, "failed to initialize image module!\n");
		return -1;
	}	
	
	//cria uma tela com dimensoes de SCREEN_W, SCREEN_H pixels
	display = al_create_display(SCREEN_W, SCREEN_H);
	if(!display) {
		fprintf(stderr, "failed to create display!\n");
		al_destroy_timer(timer);
		return -1;
	}	
	
	//instala o teclado
	if(!al_install_keyboard()) {
		fprintf(stderr, "failed to install keyboard!\n");
		return -1;
	}
	
	//inicializa o modulo allegro que carrega as fontes
	al_init_font_addon();

	//inicializa o modulo allegro que entende arquivos tff de fontes
	if(!al_init_ttf_addon()) {
		fprintf(stderr, "failed to load tff font module!\n");
		return -1;
	}
	
	//carrega o arquivo arial.ttf da fonte Arial e define que sera usado o tamanho 32 (segundo parametro)
    ALLEGRO_FONT *size_32 = al_load_font("arial.ttf", 32, 1);   
	if(size_32 == NULL) {
		fprintf(stderr, "font file does not exist or cannot be accessed!\n");
	}	
	
	
 	//cria a fila de eventos
	event_queue = al_create_event_queue();
	if(!event_queue) {
		fprintf(stderr, "failed to create event_queue!\n");
		al_destroy_display(display);
		al_destroy_timer(timer);
		return -1;
	}	
	
	//registra na fila os eventos de tela (ex: clicar no X na janela)
	al_register_event_source(event_queue, al_get_display_event_source(display));
	//registra na fila os eventos de tempo: quando o tempo altera de t para t+1
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	//registra na fila os eventos de teclado (ex: pressionar uma tecla)
	al_register_event_source(event_queue, al_get_keyboard_event_source()); 	



	//JOGADOR
	Jogador jogador;
	iniciaJogador(&jogador);
	//PRATOS		
	Prato pratos[NUM_PRATOS];	
	iniciaPratos(pratos);
	
	
	int playing=1;	
	
	//inicia o temporizador
	al_start_timer(timer);
	al_start_timer(contador);	
	
	while(playing) {

		ALLEGRO_EVENT ev;

		//espera por um evento e o armazena na variavel de evento ev
			al_wait_for_event(event_queue, &ev);
			

		while(emPartida){

			//espera por um evento e o armazena na variavel de evento ev
			al_wait_for_event(event_queue, &ev);
						
			//se o tipo de evento for um evento do temporizador, ou seja, se o tempo passou de t para t+1
			if(ev.type == ALLEGRO_EVENT_TIMER) {

				desenhaCenario();

				desenhaJogador(jogador);			

				movJogador(&jogador);

				desenhaPratos(pratos, contador);

				equilibra(pratos, jogador);	

				al_draw_textf(size_32, al_map_rgb(255, 255, 255), 150, 10, ALLEGRO_ALIGN_RIGHT, "Tempo: %d", al_get_timer_count(contador)); 

				al_draw_textf(size_32, al_map_rgb(255, 255, 255), SCREEN_W-50, 10, ALLEGRO_ALIGN_RIGHT, "Pontuação: %d", al_get_timer_count(timer)); 

				if(emPartida == 0){

					desenhaCenario();

					al_draw_textf(size_32, al_map_rgb(0, 0, 0), SCREEN_W/2, SCREEN_H/4, ALLEGRO_ALIGN_RIGHT, "GAME OVER!"); 

					al_draw_textf(size_32, al_map_rgb(255, 255, 255), SCREEN_W/2, (SCREEN_H/4)+70 , ALLEGRO_ALIGN_RIGHT, "TEMPO TOTAL: %d", al_get_timer_count(contador)); 

					al_draw_textf(size_32, al_map_rgb(255, 255, 255), SCREEN_W/2, (SCREEN_H/4)+140, ALLEGRO_ALIGN_RIGHT, "PONTUAÇÃO: %d", al_get_timer_count(timer));
					
					pontuacaoRecorde(al_get_timer_count(timer));
				}				

				//atualiza a tela (quando houver algo para mostrar)
				al_flip_display();
				
				
				if(al_get_timer_count(timer)%(int)FPS == 0)
					printf("\n%d segundos se passaram...", (int)(al_get_timer_count(timer)/FPS));
			}

			//se o tipo de evento for o fechamento da tela (clique no x da janela)
			else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
				emPartida = 0;
				playing = 0;
			}		
			//se o tipo de evento for um pressionar de uma tecla
			else if(ev.type == ALLEGRO_EVENT_KEY_DOWN){
				//imprime qual tecla foi
				//printf("\ncodigo tecla: %d", ev.keyboard.keycode);
				
				if(ev.keyboard.keycode == ALLEGRO_KEY_A){
					jogador.mov_esq = 1;
				}
				else if(ev.keyboard.keycode == ALLEGRO_KEY_D) {
					jogador.mov_dir = 1;
				}
				if(ev.keyboard.keycode == ALLEGRO_KEY_SPACE){
					jogador.equilibrando = 1;
					printf("EQUILIBRANDO\n");			
				}
			}
			//se o tipo de evento for um pressionar de uma tecla
			else if(ev.type == ALLEGRO_EVENT_KEY_UP) {

				if(ev.keyboard.keycode == ALLEGRO_KEY_A) {
					jogador.mov_esq = 0;
				}
				else if(ev.keyboard.keycode == ALLEGRO_KEY_D) {
					jogador.mov_dir = 0;
				}
				if(ev.keyboard.keycode == ALLEGRO_KEY_SPACE){
					jogador.equilibrando = 0;			
				}
			}
		}

		if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
			playing = 0;
		}			
		
		
	} //FIM DO WHILE(PLAYING)
	
	al_destroy_timer(timer);
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);
	
 
	return 0;
}