//BEGIN HEAD
#include<SDL2/SDL.h>
#include<SDL2/SDL_ttf.h>
#include<SDL2/SDL_mixer.h>

#include<stdlib.h>
#include<sstream>
#include<string.h>
#include<ctime>
#include<iostream>

#define WHITE 255,255,255,255
using namespace std;

//BEGIN GLOBALS
SDL_Window 	*Window  = NULL;
SDL_Renderer 	*Renderer= NULL;

TTF_Font 	*font 	 = NULL;
SDL_Surface 	*title	 = NULL;

SDL_Texture	*PS_tex  = NULL;
SDL_Texture	*AIS_tex = NULL;
SDL_Surface 	*PScore	 = NULL;
SDL_Surface 	*AIScore = NULL;
SDL_Color score_color = { WHITE };
SDL_Event 	event;
bool running 		= true;

SDL_Rect player_paddle; 		//Player paddle object
SDL_Rect ai_paddle; 			//AI_paddle object
SDL_Rect ball;  			//Ball object

SDL_Rect divider;

SDL_Rect player_score, ai_score, titlerect;

const int BALL_X = 390;
const int BALL_Y = 290;

int PAD_SPEED 	 = 4;
int PAD_SPEED_AI = 2;
int BALL_SPEED 	 = 3;

int playerscore;
int aiscore;

int choice = 1;

float xVel, yVel;

Mix_Chunk 	*hit 	= NULL;
Mix_Chunk 	*wall 	= NULL;
//END GLOBALS

//BEGIN FUNCTION PROTOTYPES
bool 	PointInRect	(int x, int y, SDL_Rect rec);
bool 	CheckCollision	(SDL_Rect r1, SDL_Rect r2);
int 	GetRandomNumber	(int high, int low);
void 	clean_up	(void);
void 	ResetBall	(void);
bool    init_game	(void);
bool 	load_game	(void);
void 	set_game	(void);
void 	reset_game	(void);
int 	update		(void);
void 	main_menu	(void);
void 	render		(void);
//END 	FUNCTION PROTOTYPES

//END HEAD

int main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;
	
	if (!init_game()){
		SDL_Log("Something went wrong in init");
		return EXIT_FAILURE;
	}else{
		SDL_Log("init success");
	}
	if (!load_game()){
		SDL_Log("Something went wrong in loading");
		return EXIT_FAILURE;
	}else{
		SDL_Log("load success");
	}
	set_game();
	reset_game();
	
	while(running)
	{
		update();
		render();
	}
	
	clean_up();
	return EXIT_SUCCESS;
} 

//BEGIN FUNCTIONS
bool init_game(void)
{

	SDL_Init(SDL_INIT_EVERYTHING);
	TTF_Init();
	
	Window  = SDL_CreateWindow("Pong",0,0,800,600,SDL_WINDOW_SHOWN);
	Renderer = SDL_CreateRenderer(Window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	
	TTF_Font  	*icon_font;
	SDL_Surface 	*icon;
	icon_font=TTF_OpenFont("./fontawesome-webfont.ttf", 50);
	SDL_Color font_color_icon = {WHITE};
	icon=TTF_RenderGlyph_Blended(icon_font, 0xf17b , font_color_icon);
	
	SDL_SetWindowIcon(Window, icon);
	SDL_FreeSurface(icon);
	TTF_CloseFont(icon_font);
	
	Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 );
	srand(time(NULL));
	
	return true;
}

bool load_game(void)
{
	
	hit  = Mix_LoadWAV("./hit.wav");
	wall = Mix_LoadWAV("./wall.wav");
	title = SDL_LoadBMP("sdl.bmp");
	font = TTF_OpenFont("8-bit.ttf", 28);
	return true;
}


void set_game(void)
{
	player_paddle.x = 20;
	player_paddle.y = 10;
	player_paddle.h = 45;
	player_paddle.w = 10;
	
	ai_paddle.x = 760;
	ai_paddle.y = 10;
	ai_paddle.h = 45;
	ai_paddle.w = 10;
	
	ball.w = 7;
	ball.h = 7;
	ball.x = BALL_X;
	ball.y = BALL_Y;
	
	divider.w = 4;
	divider.h = 10;
	divider.x = 398;
	divider.y = 5;
	
	xVel = 3;
	yVel = GetRandomNumber(BALL_SPEED,-BALL_SPEED);
	
	player_score.x = 50;
	player_score.y = 10;
	ai_score.x = 700;
	ai_score.y = 10;
}

void reset_game(void)
{
	player_paddle.x = 20;
	player_paddle.y = 10;

	
	ai_paddle.x = 760;
	ai_paddle.y = 10;
	
	xVel = 3;
	yVel = GetRandomNumber(BALL_SPEED,-BALL_SPEED);

	playerscore	=0;
	aiscore		=0;

}

int update(void)
{
	const Uint8 *keystates=SDL_GetKeyboardState(NULL);
	SDL_PollEvent(&event);
	
	if(event.type == SDL_QUIT)
	{
		// 			main_menu();
		running= false;
	}
	
	if(keystates[SDL_SCANCODE_UP])
	{
		player_paddle.y -= PAD_SPEED;
	}
	
	if(keystates[SDL_SCANCODE_DOWN])
	{
		player_paddle.y += PAD_SPEED;
	}
	
	if(keystates[SDL_SCANCODE_ESCAPE])
	{
		running= false;
	}
	
	if(player_paddle.y + player_paddle.h > 599)
	{
		player_paddle.y = 599 - player_paddle.h;
	}
	
	if(player_paddle.y < 1)
	{
		player_paddle.y = 1;
	}
	
	ball.x += xVel;
	ball.y += yVel;
	
	if(ball.y < 1)
	{
		yVel = -yVel;
		Mix_PlayChannel( -1, wall, 0 );
	}
	if(ball.y + ball.h> 600)
	{
		yVel = -yVel;
		Mix_PlayChannel( -1, wall, 0 );
	}
	
	if(ball.x + ball.h < -3)
	{
		ResetBall();
		aiscore++;
	}
	
	if(ball.x + ball.w > 805)
	{
		ResetBall();
		playerscore++;
	}
	
	if(ai_paddle.y + 0.5 * ai_paddle.h > ball.y + 0.5 * ball.h)
	{
		ai_paddle.y -= PAD_SPEED_AI;
	}
	
	if(ai_paddle.y + 0.5 * ai_paddle.h < ball.y + 0.5 * ball.h)
	{
		ai_paddle.y += PAD_SPEED_AI;
	}
	
	if(CheckCollision(ball, player_paddle) == true)
	{
		xVel = -xVel;
		
		if(ball.y - player_paddle.y <= 9)
		{
			yVel = -3;
		}
		
		if(ball.y - player_paddle.y  > 9 && ball.y - player_paddle.y  <= 18)
		{
			yVel = -(3/0.8);
		}
		
		if(ball.y - player_paddle.y  > 18 && ball.y - player_paddle.y  <= 27)
		{
			yVel = 0;
		}
		
		if(ball.y - player_paddle.y  > 27 && ball.y - player_paddle.y <= 36)
		{
			yVel = 3/0.8;
		}
		
		if(ball.y - player_paddle.y  > 36 && ball.y - player_paddle.y  <= 45)
		{
			yVel = 3;
		}
		//yVel = -yVel;
		//yVel = GetRandomNumber(BALL_SPEED,-BALL_SPEED);
		
		Mix_PlayChannel( -1, hit, 0 );
	}
	
	if(CheckCollision(ball, ai_paddle) == true)
	{
		xVel = -xVel;
		
		if(ball.y - ai_paddle.y <= 9)
		{
			yVel = -3;
		}
		
		if(ball.y - ai_paddle.y  > 9 && ball.y - ai_paddle.y  <= 18)
		{
			yVel = -(3/0.8);
		}
		
		if(ball.y - ai_paddle.y  > 18 && ball.y - ai_paddle.y  <= 27)
		{
			yVel = 0;
		}
		
		if(ball.y - ai_paddle.y  > 27 && ball.y - ai_paddle.y <= 36)
		{
			yVel = 3/0.8;
		}
		
		if(ball.y - ai_paddle.y  > 36 && ball.y - ai_paddle.y  <= 45)
		{
			yVel = 3;
		}
		
		
		//yVel = -yVel;
		//yVel = GetRandomNumber(BALL_SPEED,-BALL_SPEED);
		
		Mix_PlayChannel( -1, hit, 0 );
	}
	if(playerscore == 10)
	{
		choice = 3;
	}
	if(aiscore == 10)
	{
		choice = 3;
	}
	return 0;
}

void render(void)
{
	
	SDL_SetRenderDrawColor(Renderer, 111, 111, 111, 255);
	SDL_RenderClear(Renderer);
	
	SDL_SetRenderDrawColor(Renderer, WHITE);
	SDL_RenderFillRect(Renderer, &player_paddle);
	SDL_RenderFillRect(Renderer, &ai_paddle);
	SDL_RenderFillRect(Renderer, &ball);
	
	//Draw Divider WH=600
	int i;
	for (i=0; i<30; i++){
		divider.y+=divider.h*2;
		SDL_RenderFillRect(Renderer, &divider);
	}
	divider.y=5;
	
	SDL_RenderFillRect(Renderer, &divider);
	
	//Render SCORE
	int w,h;
	//player
	stringstream Pscore;
	Pscore << playerscore;
	PScore = TTF_RenderText_Solid(font, Pscore.str().c_str(),score_color);
	PS_tex  = SDL_CreateTextureFromSurface(Renderer,PScore );
	SDL_QueryTexture(PS_tex, NULL, NULL, &w, &h);
	player_score.w = w;
	player_score.h = h;
	SDL_RenderCopy(Renderer, PS_tex, NULL, &player_score);
	
	//AI
	stringstream Ascore;
	Ascore << aiscore;
	AIScore= TTF_RenderText_Solid(font, Ascore.str().c_str(),score_color);
	AIS_tex = SDL_CreateTextureFromSurface(Renderer,AIScore);
	SDL_QueryTexture(AIS_tex, NULL, NULL, &w, &h);
	ai_score.w = w;
	ai_score.h = h;
	SDL_RenderCopy(Renderer, AIS_tex, NULL, &ai_score);
		
	SDL_RenderPresent(Renderer);
}

bool PointInRect(int x, int y, SDL_Rect rec)
{
	if(x > rec.x && y > rec.y && x < rec.x + rec.w && y < rec.y + rec.h)
	{
		return true;
	}
	return false;
}

bool CheckCollision(SDL_Rect r1, SDL_Rect r2)
{
	if(PointInRect(r1.x,r1.y,r2) == true ||
		PointInRect(r1.x+r1.w, r1.y, r2) == true ||
		PointInRect(r1.x,r1.y+r1.h,r2) == true ||
		PointInRect(r1.x+r1.w,r1.y+r1.h,r2) == true)
	{
		return true;
	}
	
	return false;
}

int GetRandomNumber(int high, int low)
{
	return rand() % (high + 1 - low) + low;
}

void clean_up(void)
{
	Mix_FreeChunk(hit);
	Mix_FreeChunk(wall);
	Mix_CloseAudio();
	TTF_CloseFont(font);
	TTF_Quit();
	SDL_FreeSurface(title);
	SDL_FreeSurface(PScore);
	SDL_FreeSurface(AIScore);
	SDL_DestroyTexture(PS_tex);
	SDL_DestroyTexture(AIS_tex);
	SDL_DestroyRenderer(Renderer);
	SDL_DestroyWindow(Window);
	SDL_Quit();
}

void ResetBall(void)
{

	ball.x = BALL_X;
	ball.y = BALL_Y;
	yVel = GetRandomNumber(BALL_SPEED,-BALL_SPEED);
}

void main_menu(void)
{
;
}


//END FUNCTIONS
