#define _USE_MATH_DEFINES
#include<math.h>
#include<stdio.h>
#include<string.h>
#include<ctype.h>
extern "C" {
#include"./SDL2-2.0.10/include/SDL.h"
#include"./SDL2-2.0.10/include/SDL_main.h"
}

#define SCREEN_WIDTH   1280   // szeroksoc ekranu
#define SCREEN_HEIGHT	720  // wysokosc ekranu
#define CHARACTER_WIDTH  60  // szerokosc postaci
#define CHARACTER_HEIGHT 80  // wysokosc postaci
#define BARREL_WIDTH     20 // szerekosc beczek
#define BARREL_HEIGHT    20  // wysokosc beczek
#define JUMPTIME		1.9 // czas skoku
#define MENU_DIFF        40      // 
#define FILENAME "wyniki.txt"   // nazwa pliku 
#define MAX_SCORES 100    // maxymalna liczba zczytywanych wyników
#define SCOREPLUS_TIME 1.0


struct sdl_t {
	SDL_Event event;
	SDL_Surface* screen, * charset;
	SDL_Surface* eti;
	SDL_Surface* ladder;
	SDL_Surface* platforma;
	SDL_Surface* character;
	SDL_Surface* character1;
	SDL_Surface* character2;
	SDL_Surface* character1Left;
	SDL_Surface* character2Left;
	SDL_Surface* character1Ladder;
	SDL_Surface* character2Ladder;
	SDL_Surface* character1JUMP;
	SDL_Surface* character2JUMP;
	SDL_Surface* monkey;
	SDL_Surface* monkey1;
	SDL_Surface* monkey2;
	SDL_Surface* trophy;
	SDL_Surface* doors;
	SDL_Surface* barrel;
	SDL_Surface* barrel1;
	SDL_Surface* barrel2;
	SDL_Surface* barrel3;
	SDL_Surface* barrel4;
	SDL_Surface* heart;
	SDL_Texture* scrtex;
	SDL_Window* window;
	SDL_Renderer* renderer;
};       
struct score_t {
	char nickname[20];
	int score;
};
struct game_t {
	int unavailable;
	int option=1;
	float pozy;
	int pozx;
	int platformsheight[10][30];
	int actualfloor = 0;
	int ladderposition[3];
	int laddermaxheight[3];
	int ladderminheight[3];
	float barrelx[5];
	float barrely[5];
	int barrelfloor[5];
	int numberbarrel;
	int quit;
	int barrelstart[5];
	float jumpTime = 0;
	bool duringJump = false;
	int lastmove = 0;
	int t1, t2, frames, rc;
	int startgame = 0;
	double delta, worldTime,stageTime, fpsTimer, fps, distance;
	int lives=3;
	char playerinput[20];
	int score=0;
	struct score_t allscores[MAX_SCORES];
	int countofscores=0;
	int page=0;
	int colletectedtrophy = 0;
	int trophyx = 760;
	int momentscore = 0;
	float momentscoretime = 0;
	int isscored = 0;
};

// FUNCKCJE DO WYNIKOW
void saveScore(struct game_t* game) {
	FILE* file = fopen(FILENAME, "a");
	if (file != NULL) {
		fprintf(file, "%s %d\n", game->playerinput, game->score);
		fclose(file);
	}
	else {
		perror("Unable to open file for saving scores.");
	}
}
void sortScores(struct game_t*game) {
	for (int i = 0; i < game->countofscores - 1; i++) {
		for (int j = i + 1; j < game->countofscores; j++) {
			if (game->allscores[i].score < game->allscores[j].score) {
				// Zamiana miejscami
				struct score_t temp = game->allscores[i];
				game->allscores[i] = game->allscores[j];
				game->allscores[j] = temp;
			}
		}
	}
}
int loadScores(struct game_t* game) {
	FILE* file = fopen(FILENAME, "r");
	if (file != NULL) {
		int count = 0;
		while (fscanf(file, "%s %d", game->allscores[count].nickname, &game->allscores[count].score) == 2) {
			count++;

		}
		fclose(file);
		return count;
	}
	else {
		perror("Unable to open file for loading scores.");
		return 0;
	}
}
// FUnCKJE DO ANIMACJI 
void playerAnimation(struct sdl_t* sdl, struct game_t *game, SDL_Surface* surface1, SDL_Surface* surface2) {
	if (int(game->worldTime * 5) % 2 == 1) {
		sdl->character = surface1;

	}
	else sdl->character = surface2;
}
void barrelAnimation(struct sdl_t* sdl, struct game_t* game, SDL_Surface* surface1, SDL_Surface* surface2) {
	if (int(game->worldTime*5) % 2 == 1) {
		sdl->barrel = surface1;

	}

	else sdl->barrel = surface2;
}
void monkeyAnimation(struct sdl_t* sdl, struct game_t* game, SDL_Surface* surface1, SDL_Surface* surface2){
	// Oblicz aktualny czas w cyklu animacji
	int cycleTime = int(game->stageTime * 5) % 22;

	// Okreœl odpowiedni¹ powierzchniê na podstawie aktualnego czasu w cyklu
	if ((cycleTime >= 0 && cycleTime < 4) || (cycleTime >= 18 && cycleTime < 22)) {
		sdl->monkey = surface1;
	}
	else {
		sdl->monkey = surface2;
	}
	}

// FUNKCJE OD SKOKU
void jump(struct game_t* game) {
	if (game->jumpTime >= JUMPTIME)
	{
		game->duringJump = false;
		game->jumpTime = 0;
	}
	if (game->duringJump == true)
	{
		game->jumpTime += game->delta;
		if (game->jumpTime <= 1) {
			game->pozy -= 100* game->delta;
		}
		else {
			game->pozy += 100 * game->delta;
		}
	}
	
	
}
// spadek
void fall(struct game_t* game) {
	if (game->actualfloor == 1 && game->pozx > 1220 && game->duringJump == 0) {
		game->pozy += 50 * game->delta;
	}
	if (game->actualfloor == 0 && game->duringJump == 0 && game->pozx > 1220) {
		game->pozy += 50 * game->delta;
		if (game->pozy > 618) game->pozy = 618;
		
	}
	if (game->actualfloor == 2 && game->pozx < 80 && game->duringJump == 0) {
		game->pozy += 50 * game->delta;
	}
	if (game->actualfloor == 1 && game->duringJump == 0 && game->pozx <80) {
		game->pozy += 50 * game->delta;
		if (game->pozy > 424) game->pozy = 424;

	}

}
// FUNCJE RYSOWANIA SDL
void DrawString(SDL_Surface *screen, int x, int y, const char *text, SDL_Surface *charset) {
	int px, py, c;
	SDL_Rect s, d;
	s.w = 8;
	s.h = 8;
	d.w = 8;
	d.h = 8;
	while(*text) {
		c = *text & 255;
		px = (c % 16) * 8;
		py = (c / 16) * 8;
		s.x = px;
		s.y = py;
		d.x = x;
		d.y = y;
		SDL_BlitSurface(charset, &s, screen, &d);
		x += 8;
		text++;
		};
	};

void DrawSurface(SDL_Surface *screen, SDL_Surface *sprite, int x, int y) {
	SDL_Rect dest;
	dest.x = x - sprite->w / 2;
	dest.y = y - sprite->h / 2;
	dest.w = sprite->w;
	dest.h = sprite->h;
	SDL_BlitSurface(sprite, NULL, screen, &dest);
	};

void DrawPixel(SDL_Surface *surface, int x, int y, Uint32 color) {
	int bpp = surface->format->BytesPerPixel;
	Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
	*(Uint32 *)p = color;
	};

void DrawLine(SDL_Surface *screen, int x, int y, int l, int dx, int dy, Uint32 color) {
	for(int i = 0; i < l; i++) {
		DrawPixel(screen, x, y, color);
		x += dx;
		y += dy;
		};
	};

void DrawRectangle(SDL_Surface *screen, int x, int y, int l, int k, Uint32 outlineColor, Uint32 fillColor) {
	int i;
	DrawLine(screen, x, y, k, 0, 1, outlineColor);
	DrawLine(screen, x + l - 1, y, k, 0, 1, outlineColor);
	DrawLine(screen, x, y, l, 1, 0, outlineColor);
	DrawLine(screen, x, y + k - 1, l, 1, 0, outlineColor);
	for(i = y + 1; i < y + k - 1; i++)
		DrawLine(screen, x + 1, i, l - 2, 1, 0, fillColor);
	};

void Drawcharacter(struct sdl_t *sdl,struct game_t* game) {
	DrawSurface( sdl->screen,sdl->character,game->pozx,game->pozy);
}

void Drawmap(SDL_Surface* screen, SDL_Surface* sprite,struct game_t* game) {
	int x = 40;
	int y = 700;
	for (int i=0; i < 16; i++) {
		DrawSurface(screen, sprite, x, y);
		x += 80;
		y -= 2;
		game->platformsheight[0][i] = y - 40;
	}
	y = 500;
	x -= 80;
	for (int i = 0; i < 15; i++) {
		x -= 80;
		DrawSurface(screen, sprite, x, y);
		
		y -= 2; 
		game->platformsheight[1][14-i] = y - 40;
		game->platformsheight[1][15] = game->platformsheight[0][15];
		game->platformsheight[1][15] = game->platformsheight[1][14];
	}
	y = 300;
	for (int i = 0; i < 15; i++) {
		x += 80;
		DrawSurface(screen, sprite, x, y);
		
		y -= 2;
		game->platformsheight[2][i] = y - 40;
	}
}

void Drawladder(SDL_Surface* screen, SDL_Surface* sprite,struct game_t *game) {
	
	DrawSurface(screen, sprite, 1000, 580);
	game->ladderposition[0] = 1000;
	game->laddermaxheight[0] = 680;
	game->ladderminheight[0] = 440;
	DrawSurface(screen, sprite, 280, 380);
	game->ladderposition[1] = 280;
	game->laddermaxheight[1] = 480;
	game->ladderminheight[1] = 240;
}

void DrawLives(SDL_Surface* screen, SDL_Surface* sprite, struct game_t* game)
{
	for (int i = 1; i <= game->lives; i++)
	{
		DrawSurface(screen, sprite, 20 + 25 * (i - 1), 70);
	}
	
}
void Drawdoors(SDL_Surface* screen, SDL_Surface* sprite, struct game_t* game)
{
	
	
	DrawSurface(screen, sprite, 900 , 240);
	

}
void DrawTrophy(SDL_Surface* screen, SDL_Surface* sprite, struct game_t* game)
{

	if(game->colletectedtrophy==0) DrawSurface(screen, sprite, game->trophyx, 655);


}
void Drawmonkey(SDL_Surface* screen, SDL_Surface* sprite) {
	DrawSurface(screen, sprite, 1150,190);
}
// funckje WYSWIETLANIA MENU
void menu(struct sdl_t* sdl, struct game_t *game, char text[], int frameColor, int backgroundColor)
{
	
	DrawRectangle(sdl->screen, SCREEN_WIDTH / 2 - SCREEN_WIDTH / 8, SCREEN_HEIGHT / 2 - SCREEN_HEIGHT / 4, SCREEN_WIDTH / 4, SCREEN_HEIGHT / 2, frameColor, backgroundColor);
	DrawRectangle(sdl->screen, SCREEN_WIDTH / 2 - SCREEN_WIDTH / 8 + 10, SCREEN_HEIGHT / 2 - 85 + (MENU_DIFF * (game->option - 1)), SCREEN_WIDTH / 4 - 20, 18, frameColor, SDL_MapRGB(sdl->screen->format, 0x00, 0x00, 0x00));
	sprintf(text, "START GAME");
	DrawString(sdl->screen, SCREEN_WIDTH / 2 - strlen(text) * 8 / 2, SCREEN_HEIGHT / 2 - MENU_DIFF * 2, text, sdl->charset);
	sprintf(text, "SAVE GAME");
	DrawString(sdl->screen, SCREEN_WIDTH / 2 - strlen(text) * 8 / 2, SCREEN_HEIGHT / 2 - MENU_DIFF, text, sdl->charset);
	sprintf(text, "CHOOSE STAGE");
	DrawString(sdl->screen, SCREEN_WIDTH / 2 - strlen(text) * 8 / 2, SCREEN_HEIGHT / 2, text, sdl->charset);
	sprintf(text, "RESULTS");
	DrawString(sdl->screen, SCREEN_WIDTH / 2 - strlen(text) * 8 / 2, SCREEN_HEIGHT / 2 + MENU_DIFF, text, sdl->charset);
	sprintf(text, "EXIT");
	DrawString(sdl->screen, SCREEN_WIDTH / 2 - strlen(text) * 8 / 2, SCREEN_HEIGHT / 2 + MENU_DIFF * 2, text, sdl->charset);
	if (game->unavailable==1)
	{
		sprintf(text, "This option is currently unavailable");
		DrawString(sdl->screen, SCREEN_WIDTH / 2 - strlen(text) * 8 / 2, 20, text, sdl->charset);
	}
	sprintf(text, "CURRENT OPTION: %.1li", game->option);
	DrawString(sdl->screen, SCREEN_WIDTH / 2 - strlen(text) * 8 / 2, 40, text, sdl->charset);
	sprintf(text, "Wpisz swój nick: %s", game->playerinput);

	
	DrawString(sdl->screen, SCREEN_WIDTH / 2 - strlen(text) * 8 / 2, 60, text, sdl->charset);
}
void menutocontinue(struct sdl_t* sdl, struct game_t* game, char text[], int frameColor, int backgroundColor) {
	DrawRectangle(sdl->screen, SCREEN_WIDTH / 2 - SCREEN_WIDTH / 8, SCREEN_HEIGHT / 2 - SCREEN_HEIGHT / 6, SCREEN_WIDTH / 4, SCREEN_HEIGHT / 4, frameColor, backgroundColor);
	DrawRectangle(sdl->screen, SCREEN_WIDTH / 2 - SCREEN_WIDTH / 8 + 10, SCREEN_HEIGHT / 2 - 45 + (MENU_DIFF * (game->option - 1)), SCREEN_WIDTH / 4 - 20, 18, frameColor, SDL_MapRGB(sdl->screen->format, 0x00, 0x00, 0x00));
	sprintf(text, "YOU DIED. DO YOU WANT TO CONTINUE?");
	DrawString(sdl->screen, SCREEN_WIDTH / 2 - strlen(text) * 8 / 2, SCREEN_HEIGHT / 2 - MENU_DIFF * 2, text, sdl->charset);
	sprintf(text, "YES");
	DrawString(sdl->screen, SCREEN_WIDTH / 2 - strlen(text) * 8 / 2, SCREEN_HEIGHT / 2 - MENU_DIFF, text, sdl->charset);
	sprintf(text, "NO");
	DrawString(sdl->screen, SCREEN_WIDTH / 2 - strlen(text) * 8 / 2, SCREEN_HEIGHT / 2, text, sdl->charset);
}
void results(struct sdl_t* sdl, struct game_t* game, char text[], int frameColor, int backgroundColor) {
	int scoresPerPage = 3;
	int startIndex = game->page * scoresPerPage;
	int endIndex = startIndex + scoresPerPage;

	DrawRectangle(sdl->screen, SCREEN_WIDTH / 2 - SCREEN_WIDTH / 8, SCREEN_HEIGHT / 2 - SCREEN_HEIGHT / 4, SCREEN_WIDTH / 4, SCREEN_HEIGHT / 2, frameColor, backgroundColor);

	for (int i = startIndex; i < endIndex && i < game->countofscores; i++) {
		sprintf(text, "%.1li : %s  %.1li", i + 1, game->allscores[i].nickname, game->allscores[i].score);
		DrawString(sdl->screen, SCREEN_WIDTH / 2 - strlen(text) * 8 / 2, SCREEN_HEIGHT / 2 - MENU_DIFF * 2 + (i - startIndex) * 30, text, sdl->charset);
	}
}
void menutosave(struct sdl_t* sdl, struct game_t* game, char text[], int frameColor, int backgroundColor) {
	DrawRectangle(sdl->screen, SCREEN_WIDTH / 2 - SCREEN_WIDTH / 8, SCREEN_HEIGHT / 2 - SCREEN_HEIGHT / 6, SCREEN_WIDTH / 4, SCREEN_HEIGHT / 4, frameColor, backgroundColor);
	DrawRectangle(sdl->screen, SCREEN_WIDTH / 2 - SCREEN_WIDTH / 8 + 10, SCREEN_HEIGHT / 2 - 45 + (MENU_DIFF * (game->option - 1)), SCREEN_WIDTH / 4 - 20, 18, frameColor, SDL_MapRGB(sdl->screen->format, 0x00, 0x00, 0x00));
	sprintf(text, "DO YOU WANT TO SAVE YOUR SCORE?");
	DrawString(sdl->screen, SCREEN_WIDTH / 2 - strlen(text) * 8 / 2, SCREEN_HEIGHT / 2 - MENU_DIFF * 2, text, sdl->charset);
	sprintf(text, "YES");
	DrawString(sdl->screen, SCREEN_WIDTH / 2 - strlen(text) * 8 / 2, SCREEN_HEIGHT / 2 - MENU_DIFF, text, sdl->charset);
	sprintf(text, "NO");
	DrawString(sdl->screen, SCREEN_WIDTH / 2 - strlen(text) * 8 / 2, SCREEN_HEIGHT / 2, text, sdl->charset);
}
// FUNCKJE BECZEK
void BarrelStart(struct game_t* game) {
	if (game->stageTime > 5)
	{
		game->barrelstart[1] = 1;
	}
	if (game->stageTime > 10) {
		game->barrelstart[2] = 1;
	}
	if (game->stageTime > 15) {
		game->barrelstart[3] = 1;
	}
	if (game->stageTime > 20) {
		game->barrelstart[4] = 1;
	}
}
void BarrelMove(struct game_t* game) {
	for (int i = 0; i < 5; i++)
	{
		if ((int)game->barrely[i] >= 650) {
			game->barrelfloor[i] = 0;
		}
		else if ((int)game->barrely[i] < 660 && (int)game->barrely[i] >= 440) {
			game->barrelfloor[i] = 1;
		}
		else
		{
			game->barrelfloor[i] = 2;
		}
	}
	for(int i=0;i<5; i++){
		if (game->barrelstart[i] == 1) {
			if (game->barrelfloor[i] == 2)
			{
				if (game->barrelx[i] < 70) {
					game->barrely[i] += 0.5;
				}
				else
				{
					game->barrelx[i] -= 0.5;
					game->barrely[i] = game->platformsheight[2][((int)game->barrelx[i] / 80)] + 23;
				}
			}
			else if (game->barrelfloor[i] == 1)
			{
				if (game->barrelx[i] > 1210)
				{
					game->barrely[i] += 0.5;
				}
				else
				{
					game->barrelx[i] += 0.5;
					game->barrely[i] = game->platformsheight[1][((int)game->barrelx[i] / 80)] + 20;
				}
			}
			else if (game->barrelfloor[i] == 0)
			{

				game->barrelx[i] -= 0.5;
				game->barrely[i] = game->platformsheight[0][((int)game->barrelx[i] / 80)] + 23;

			}
		}
	}
	for (int i = 0; i < 5; i++)
	{
		if (game->barrelx[i] < 1) {
			game->barrelx[i] = 1160;
			game->barrely[i] = 250;
		}
			
	}

}
void DrawBarrel(SDL_Surface* screen, SDL_Surface* sprite, struct game_t* game) {
	for(int i=0;i<5;i++)
	{
		DrawSurface(screen, sprite, game->barrelx[i], game->barrely[i]);
	}

}
// sprawdzanie kolizji
int CheckCollision(SDL_Rect characterBox, SDL_Rect barrelBox, SDL_Rect barrelBox1, SDL_Rect barrelBox2 , SDL_Rect barrelBox3, SDL_Rect barrelBox4)
{
	if (characterBox.x < barrelBox.x + barrelBox.w &&
		characterBox.x + characterBox.w > barrelBox.x &&
		characterBox.y < barrelBox.y + barrelBox.h &&
		characterBox.y + characterBox.h > barrelBox.y) {
		return 1;
	}
	else if (characterBox.x < barrelBox1.x + barrelBox1.w &&
		characterBox.x + characterBox.w > barrelBox1.x &&
		characterBox.y < barrelBox1.y + barrelBox1.h &&
		characterBox.y + characterBox.h > barrelBox1.y) {
		return 1;
	}
	else if (characterBox.x < barrelBox2.x + barrelBox2.w &&
		characterBox.x + characterBox.w > barrelBox2.x &&
		characterBox.y < barrelBox2.y + barrelBox2.h &&
		characterBox.y + characterBox.h > barrelBox2.y) {
		return 1;
	}
	else if (characterBox.x < barrelBox3.x + barrelBox3.w &&
		characterBox.x + characterBox.w > barrelBox3.x &&
		characterBox.y < barrelBox3.y + barrelBox3.h &&
		characterBox.y + characterBox.h > barrelBox3.y) {
		return 1;
	}
	else if (characterBox.x < barrelBox4.x + barrelBox4.w &&
		characterBox.x + characterBox.w > barrelBox4.x &&
		characterBox.y < barrelBox4.y + barrelBox4.h &&
		characterBox.y + characterBox.h > barrelBox4.y) {
		return 1;
	}
	else return 0;
}
// sprawdzanie czy sie dotar³o do konca etapu
int CheckEndstage(struct game_t* game) {
	if (game->pozx > 890 && game->actualfloor == 2) {
		return 1; 
	}
	return 0;
}
 //  FUNCKJA SPRAWDZANIA NA KTÓREJ DRABINIE JEST CHARAKTER
int onwhichladderischaracter(struct game_t* game) {
	for (int i = 0; i < 2; i++)
	{
		if ((game->ladderposition[i] / 80) == (game->pozx / 80))
			return i;
	}
	return 9;
}
// FUNCKJA SPRAWDZANIA NA JAKIM PIETRZE JEST NASZA POSTAC 
void FloorCheck(struct game_t* game)
{
	if (game->pozy > 450) {
		game->actualfloor = 0;
	}
	else if (game->pozy < 450 && game->pozy>250) {
		game->actualfloor = 1;
	}
	else
	{
		game->actualfloor = 2;
	}
}


// reset wartosci 
void resetgame(struct game_t* game) {
	game->actualfloor = 0;
	game->frames = 0;
	game->fpsTimer = 0;
	game->fps = 0;
	game->pozx = 40;
	game->pozy = 650;
	game->barrelx[0] = 1160;
	game->barrely[0] = 250;
	game->barrelx[1] = 1160;
	game->barrely[1] = 250;
	game->barrelx[2] = 1160;
	game->barrely[2] = 250;
	game->barrelx[3] = 1160;
	game->barrely[3] = 250;
	game->barrelx[4] = 1160;
	game->barrely[4] = 250;
	game->barrelstart[0] = 1;
	game->barrelstart[1] = 0;
	game->barrelstart[2] = 0;
	game->barrelstart[3] = 0;
	game->barrelstart[4] = 0;
	game->stageTime = 0;
}

// STEROWANIE:
void control(struct game_t* game, struct sdl_t* sdl) {
	while (SDL_PollEvent(&sdl->event)) {
		switch (sdl->event.type) {
		case SDL_KEYDOWN:
			if (sdl->event.key.keysym.sym == SDLK_ESCAPE) game->quit = 1;
			else if (sdl->event.key.keysym.sym == SDLK_n)
			{
				game->worldTime = 0;
				resetgame(game);
			}

			else if (sdl->event.key.keysym.sym == SDLK_SPACE)
			{
				game->duringJump = true;
			}
			else if (game->actualfloor == 0)    //  ruch na platformie 0 : 
			{
				if (sdl->event.key.keysym.sym == SDLK_RIGHT) {
					playerAnimation(sdl, game, sdl->character1, sdl->character2);
					game->pozx += 5;
					if (game->pozx > 1270) game->pozx = 1270;
					if (!game->duringJump) game->pozy = game->platformsheight[0][(game->pozx / 80)] - 10;
				}
				else if (sdl->event.key.keysym.sym == SDLK_LEFT) {
					playerAnimation(sdl, game, sdl->character1Left, sdl->character2Left);
					game->pozx -= 5;
					if (game->pozx < 0) game->pozx = 0;
					if(!game->duringJump) game->pozy = game->platformsheight[0][(game->pozx / 80)] - 10;
				}
				else if (sdl->event.key.keysym.sym == SDLK_UP) {
					if (onwhichladderischaracter(game) == 0) {
						playerAnimation(sdl, game, sdl->character1Ladder, sdl->character2Ladder);
						game->pozy -= 10;
						if (game->pozy < game->ladderminheight[0]) game->pozy = game->ladderminheight[0];
					}
				}
				else if (sdl->event.key.keysym.sym == SDLK_DOWN)
				{
					if (onwhichladderischaracter(game) == 0) {
						playerAnimation(sdl, game, sdl->character1Ladder, sdl->character2Ladder);
						game->pozy += 10;

						if (game->pozy > (game->platformsheight[0][(game->pozx / 80)] - 10)) game->pozy = game->platformsheight[0][(game->pozx / 80)] - 10;
					}
				}
			}
			else if (game->actualfloor == 1)     // ruch na platformie 1
			{
				if (sdl->event.key.keysym.sym == SDLK_RIGHT) {
					playerAnimation(sdl, game, sdl->character1, sdl->character2);
					game->pozx += 5;
					if (game->pozx > 1280) game->pozx = 1280;
					if (!game->duringJump)game->pozy = game->platformsheight[1][(game->pozx / 80)] - 10;
				}
				else if (sdl->event.key.keysym.sym == SDLK_LEFT) {
					playerAnimation(sdl, game, sdl->character1Left, sdl->character2Left);
					game->pozx -= 5;
					if (game->pozx < 0) game->pozx = 0;
					if (!game->duringJump)game->pozy = game->platformsheight[1][(game->pozx / 80)] - 10;
				}
				else if (sdl->event.key.keysym.sym == SDLK_UP) {
					if (onwhichladderischaracter(game) == 0) {\
						playerAnimation(sdl, game, sdl->character1Ladder, sdl->character2Ladder);
						game->pozy -= 10;
						if (game->pozy < game->ladderminheight[0]) game->pozy = game->ladderminheight[0];
					}
					if (onwhichladderischaracter(game) == 1) {
						playerAnimation(sdl, game, sdl->character1Ladder, sdl->character2Ladder);
						game->pozy -= 10;
					}
				}
				else if (sdl->event.key.keysym.sym == SDLK_DOWN)
				{
					if (onwhichladderischaracter(game) == 1) {
						playerAnimation(sdl, game, sdl->character1Ladder, sdl->character2Ladder);
						game->pozy += 10;
						if (!game->duringJump)if (game->pozy > (game->platformsheight[1][(game->pozx / 80)] - 10)) game->pozy = game->platformsheight[1][(game->pozx / 80)] - 10;

					}
					if (onwhichladderischaracter(game) == 0) {
						playerAnimation(sdl, game, sdl->character1Ladder, sdl->character2Ladder);
						game->pozy += 10;

					}
				}
			}
			else if (game->actualfloor == 2)     // ruch na platformie 2
			{
				if (sdl->event.key.keysym.sym == SDLK_RIGHT) {
					playerAnimation(sdl, game, sdl->character1, sdl->character2);
					game->pozx += 5;
					if (game->pozx > 1280) game->pozx = 1280;
					if (!game->duringJump)game->pozy = game->platformsheight[2][(game->pozx / 80)] - 10;
				}
				else if (sdl->event.key.keysym.sym == SDLK_LEFT) {
					playerAnimation(sdl, game, sdl->character1Left, sdl->character2Left);
					game->pozx -= 5;
					if (game->pozx < 0) game->pozx = 0;
					if (!game->duringJump)game->pozy = game->platformsheight[2][(game->pozx / 80)] - 10;
				}
				else if (sdl->event.key.keysym.sym == SDLK_UP) {
					if (onwhichladderischaracter(game) == 1) {
						playerAnimation(sdl, game, sdl->character1Ladder, sdl->character2Ladder);
						game->pozy -= 10;
						if (game->pozy < game->ladderminheight[1]) game->pozy = game->ladderminheight[1];

					}
				}
				else if (sdl->event.key.keysym.sym == SDLK_DOWN)
				{
					if (onwhichladderischaracter(game) == 1) {
						playerAnimation(sdl, game, sdl->character1Ladder, sdl->character2Ladder);
						game->pozy += 10;


					}
				}

				break;
		case SDL_QUIT:
			game->quit = 1;
			break;
			};
		};
		game->frames++;
	}
}
void controlmenu(struct game_t* game, struct sdl_t* sdl) {
	
	while (SDL_PollEvent(&sdl->event)) 
	{
		switch (sdl->event.type) 
		{
			case SDL_KEYDOWN:
			if (sdl->event.key.keysym.sym == SDLK_ESCAPE) game->quit = 1;
			else  if (sdl->event.key.keysym.sym == SDLK_DOWN)
			{
				game->option++;
				game->unavailable = false;
				if (game->option >= 5) game->option = 5;
				
			}
			else if (sdl->event.key.keysym.sym == SDLK_UP)
			{
				game->option--;
				game->unavailable = false;
				if (game->option <= 1) game->option = 1;
			}
			else if (sdl->event.key.keysym.sym == SDLK_RETURN)
			{
				switch (game->option)
				{
				case 1:
					game->t1 = SDL_GetTicks();
					game->startgame = 1;
					break;
				case 2:
					game->unavailable = 1;
					break;
				case 3:
					game->unavailable = 1;
					break;
				case 4:
					game->countofscores=loadScores(game);
					sortScores(game);
					game->startgame = 3;
					break;
				case 5:
					game->quit = 1;
					break;
				}
			}
			else if (isalpha(sdl->event.key.keysym.sym) || sdl->event.key.keysym.sym == SDLK_SPACE) {
				// Jeœli klawisz to litera lub spacja, dodaj do bufora
				strncat(game->playerinput, SDL_GetKeyName(sdl->event.key.keysym.sym), sizeof(game->playerinput) - strlen(game->playerinput) - 1);
			}
			else if (sdl->event.key.keysym.sym == SDLK_BACKSPACE) {
				// Jeœli klawisz to Backspace, usuñ ostatni znak z bufora
				size_t len = strlen(game->playerinput);
				if (len > 0) {
					game->playerinput[len - 1] = '\0';
				}
			}
			break;
		}
		
	}
}
void controlcontinuegame(struct game_t* game, struct sdl_t* sdl) {
	while (SDL_PollEvent(&sdl->event))
	{
		switch (sdl->event.type)
		{
		case SDL_KEYDOWN:
			if (sdl->event.key.keysym.sym == SDLK_ESCAPE) game->quit = 1;
			else  if (sdl->event.key.keysym.sym == SDLK_DOWN)
			{
				game->option++;
				game->unavailable = false;
				if (game->option >= 2) game->option = 2;
	
			}
			else if (sdl->event.key.keysym.sym == SDLK_UP)
			{
				game->option--;
				game->unavailable = false;
				if (game->option <= 1) game->option = 1;
			}
			else if (sdl->event.key.keysym.sym == SDLK_RETURN)
			{
				switch (game->option)
				{
				case 1:
					game->startgame = 1;
					break;
				case 2:
					game->startgame = 4;
					break;
				}
			}
		}

	}
}
void controlsavegame(struct game_t* game, struct sdl_t* sdl) {
	while (SDL_PollEvent(&sdl->event))
	{
		switch (sdl->event.type)
		{
		case SDL_KEYDOWN:
			if (sdl->event.key.keysym.sym == SDLK_ESCAPE) game->quit = 1;
			else  if (sdl->event.key.keysym.sym == SDLK_DOWN)
			{
				game->option++;
				game->unavailable = false;
				if (game->option >= 2) game->option = 2;

			}
			else if (sdl->event.key.keysym.sym == SDLK_UP)
			{
				game->option--;
				game->unavailable = false;
				if (game->option <= 1) game->option = 1;
			}
			else if (sdl->event.key.keysym.sym == SDLK_RETURN)
			{
				switch (game->option)
				{
				case 1:
					saveScore(game);
					game->startgame = 0;
					break;
				case 2:
					game->startgame = 0;
					break;
				}
			}
		}

	}
}
void controlresult(struct game_t* game, struct sdl_t* sdl) {
	while (SDL_PollEvent(&sdl->event))
	{
		switch (sdl->event.type)
		{
		case SDL_KEYDOWN:
			if (sdl->event.key.keysym.sym == SDLK_ESCAPE) game->startgame = 0;
			else if (sdl->event.key.keysym.sym == SDLK_1) game->page = 0;
			else if (sdl->event.key.keysym.sym == SDLK_2) game->page = 1;
			else if (sdl->event.key.keysym.sym == SDLK_3) game->page = 2;
			else if (sdl->event.key.keysym.sym == SDLK_4) game->page = 3;
			else if (sdl->event.key.keysym.sym == SDLK_5) game->page = 4;
			else if (sdl->event.key.keysym.sym == SDLK_6) game->page = 5;
			else if (sdl->event.key.keysym.sym == SDLK_7) game->page = 6;
			else if (sdl->event.key.keysym.sym == SDLK_8) game->page = 7;
			else if (sdl->event.key.keysym.sym == SDLK_9) game->page = 8;

		}
	}
}


// sprawdzanie czy zdobylo sie punkty
void checkbarrelisjumped(struct game_t* game) {
	for (int i=0; i < 4; i++) {
		if (game->actualfloor == game->barrelfloor[i] && game->pozx == game->barrelx[i]) { 
			game->score += 50; 
			game->isscored = 1;
			game->momentscore = 50;

		}
	}
	
}
void collectingoftrophy(struct game_t* game) {

	
	if (game->pozx == game->trophyx && game->colletectedtrophy == 0) {
			game->score += 200; 
			game->colletectedtrophy = 1;
			game->isscored = 1;
			game->momentscore = 200;
		}
			
	

}
// animacja i kontrola zdobywanych punktów
void momentscore(struct game_t* game, struct sdl_t* sdl) {
	char text[128];
	if (game->momentscoretime >= SCOREPLUS_TIME)
	{
		game->isscored = 0;
		game->momentscoretime = 0;
	}
	if (game->isscored == 1) {
		game->momentscoretime += game->delta;
		sprintf(text, " %.1li ", game->momentscore);
		DrawString(sdl->screen, game->pozx-20,game->pozy - 50, text, sdl->charset);
	}
	
}
	
// INICJALIZACJA BITMAP
void initSDL(struct sdl_t* sdl){
	
	sdl->character1 = SDL_LoadBMP("./ludzik.bmp");
	sdl->character2 = SDL_LoadBMP("./ludzik2.bmp");
	sdl->platforma = SDL_LoadBMP("./platforma.bmp");
	sdl->ladder = SDL_LoadBMP("./drabina.bmp");
	sdl->barrel1 = SDL_LoadBMP("./beczka.bmp");
	sdl->barrel2 = SDL_LoadBMP("./beczka3.bmp");
	sdl->monkey1 = SDL_LoadBMP("./malpa.bmp");
	sdl->monkey2 = SDL_LoadBMP("./malpa2.bmp");
	sdl->heart = SDL_LoadBMP("./serce.bmp");
	sdl->doors = SDL_LoadBMP("./drzwi.bmp");
	sdl->trophy = SDL_LoadBMP("./puchar.bmp");
	sdl->character1Left = SDL_LoadBMP("./ludziklewo1.bmp");
	sdl->character2Left = SDL_LoadBMP("./ludziklewo2.bmp");
	sdl->character1Ladder = SDL_LoadBMP("./ludzikwgore.bmp");
	sdl->character2Ladder = SDL_LoadBMP("./ludzikwgore2.bmp");;
	}

// main
#ifdef __cplusplus
extern "C"
#endif
int main(int argc, char **argv) {
	struct game_t game;
	struct sdl_t sdl;
	

	if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("SDL_Init error: %s\n", SDL_GetError());
		return 1;
		}

	// tryb pe³noekranowy / fullscreen mode
	//rc = SDL_CreateWindowAndRenderer(0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP,
	                              //   &window, &renderer);
	game.rc = SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0,
	                                &sdl.window, &sdl.renderer);
	if(game.rc != 0) {
		SDL_Quit();
		printf("SDL_CreateWindowAndRenderer error: %s\n", SDL_GetError());
		return 1;
		};
	
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	SDL_RenderSetLogicalSize(sdl.renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
	SDL_SetRenderDrawColor(sdl.renderer, 0, 0, 0, 255);

	SDL_SetWindowTitle(sdl.window, "Donkey Kong by Micha³ Matysiak");


	sdl.screen = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32,
	                              0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);

	sdl.scrtex = SDL_CreateTexture(sdl.renderer, SDL_PIXELFORMAT_ARGB8888,
	                           SDL_TEXTUREACCESS_STREAMING,
	                           SCREEN_WIDTH, SCREEN_HEIGHT);


	// wy³¹czenie widocznoœci kursora myszy
	SDL_ShowCursor(SDL_DISABLE);

	// wczytanie obrazka cs8x8.bmp
	sdl.charset = SDL_LoadBMP("./cs8x8.bmp");
	initSDL(&sdl);
	SDL_SetColorKey(sdl.charset, true, 0x000000);
	
	char text[128];
	int czarny = SDL_MapRGB(sdl.screen->format, 0x00, 0x00, 0x00);
	int zielony = SDL_MapRGB(sdl.screen->format, 0x00, 0xFF, 0x00);
	int czerwony = SDL_MapRGB(sdl.screen->format, 0xFF, 0x00, 0x00);
	int niebieski = SDL_MapRGB(sdl.screen->format, 0x11, 0x11, 0xCC);
	game.frames = 0;
	game.fpsTimer = 0;
	game.fps = 0;
	game.quit = 0;
	game.worldTime = 0;
	game.stageTime = 0;
	game.pozx = 40;    
	game.pozy = 650;
	game.barrelx[0] = 1160;
	game.barrely[0] = 250;
	game.barrelx[1] = 1160;
	game.barrely[1] = 250;
	game.barrelx[2] = 1160;
	game.barrely[2] = 250;
	game.barrelx[3] = 1160;
	game.barrely[3] = 250;
	game.barrelx[4] = 1160;
	game.barrely[4] = 250;
	game.barrelstart[0] = 1;
	game.lives = 3;
	game.playerinput[0] = '\0';
	game.score = 0; 
	sdl.character = sdl.character1;
	sdl.barrel = sdl.barrel1;
	sdl.monkey = sdl.monkey1;

	while (!game.quit) {
		if (game.startgame == 0) {           // menu pocz¹tkowe
			SDL_FillRect(sdl.screen, NULL, czarny);
			controlmenu(&game, &sdl);
			menu(&sdl, &game, text, zielony, czarny);
			SDL_UpdateTexture(sdl.scrtex, NULL, sdl.screen->pixels, sdl.screen->pitch);
			SDL_RenderCopy(sdl.renderer, sdl.scrtex, NULL, NULL);
			SDL_RenderPresent(sdl.renderer);
		}
		else if (game.startgame == 2) {      // zapytanie czy chcesz kontynuowac gre
			SDL_FillRect(sdl.screen, NULL, czarny);
			controlcontinuegame(&game, &sdl);
			menutocontinue(&sdl, &game, text, zielony, czarny);
			SDL_UpdateTexture(sdl.scrtex, NULL, sdl.screen->pixels, sdl.screen->pitch);
			SDL_RenderCopy(sdl.renderer, sdl.scrtex, NULL, NULL);
			SDL_RenderPresent(sdl.renderer);
		}
		else if (game.startgame == 3) {     // tablica wyników
			SDL_FillRect(sdl.screen, NULL, czarny);
			results(&sdl, &game, text, zielony, czarny);
			controlresult(&game, &sdl);
			SDL_UpdateTexture(sdl.scrtex, NULL, sdl.screen->pixels, sdl.screen->pitch);
			SDL_RenderCopy(sdl.renderer, sdl.scrtex, NULL, NULL);
			SDL_RenderPresent(sdl.renderer);

		}
		else if (game.startgame == 4) {     // zapytanie czy chcesz zapisac gre
			SDL_FillRect(sdl.screen, NULL, czarny); 
			menutosave(&sdl, &game, text, zielony, czarny);
			controlsavegame(&game, &sdl);
			SDL_UpdateTexture(sdl.scrtex, NULL, sdl.screen->pixels, sdl.screen->pitch);
			SDL_RenderCopy(sdl.renderer, sdl.scrtex, NULL, NULL);
			SDL_RenderPresent(sdl.renderer);

		}
		else if (game.startgame == 1) {
			
			game.t2 = SDL_GetTicks();

			// w tym momencie t2-t1 to czas w milisekundach,
			// jaki uplyna³ od ostatniego narysowania ekranu
			// delta to ten sam czas w sekundach
			// here t2-t1 is the time in milliseconds since
			// the last screen was drawn
			// delta is the same time in seconds 
			game.delta = (game.t2 - game.t1) * 0.001;
			game.t1 = game.t2;
			
			game.worldTime += game.delta;
			game.stageTime += game.delta;
			game.fpsTimer += game.delta;
			if (game.fpsTimer > 0.5) {
				game.fps = game.frames * 2;
				game.frames = 0;
				game.fpsTimer -= 0.5;
			};
		
			SDL_FillRect(sdl.screen, NULL, czarny);
			BarrelStart(&game);
			monkeyAnimation(&sdl, &game, sdl.monkey1, sdl.monkey2);
			Drawmap(sdl.screen, sdl.platforma, &game);//rysowanie mapy 
			
			Drawladder(sdl.screen, sdl.ladder, &game); // rysowanie drabinek 
			Drawdoors(sdl.screen, sdl.doors, &game);
			jump(&game);
			fall(&game);
			barrelAnimation(&sdl, &game, sdl.barrel1, sdl.barrel2);
			Drawcharacter(&sdl, &game);
			DrawTrophy(sdl.screen, sdl.trophy, &game);
			DrawBarrel(sdl.screen, sdl.barrel, &game); //rysowanie beczki
			Drawmonkey(sdl.screen, sdl.monkey);
			momentscore(&game, &sdl);
			

			// tekst informacyjny / info text / 
			DrawRectangle(sdl.screen, 4, 4, SCREEN_WIDTH - 8, 100, zielony, czarny);
			sprintf(text, "Twój nick: %s", game.playerinput);
			DrawString(sdl.screen, 10, 10, text, sdl.charset);
			sprintf(text, "Michal Matysiak 198395 Wykonane podpunkty: A,C,D,E,F,G H,   czas trwania = %.1lf s  %.0lf klatek / s", game.worldTime, game.fps);
			DrawString(sdl.screen, sdl.screen->w / 2 - strlen(text) * 8 / 2, 10, text, sdl.charset);
			sprintf(text, "Esc - wyjscie, n - nowa gra");
			sprintf(text, "WYNIK:  %.1li ", game.score);
			DrawString(sdl.screen, sdl.screen->w / 2 - strlen(text) * 8 / 2, 70, text, sdl.charset);
			
			

			DrawLives(sdl.screen, sdl.heart, &game);

			SDL_UpdateTexture(sdl.scrtex, NULL, sdl.screen->pixels, sdl.screen->pitch);
			SDL_RenderCopy(sdl.renderer, sdl.scrtex, NULL, NULL);
			SDL_RenderPresent(sdl.renderer);
			

			FloorCheck(&game);
			BarrelMove(&game);    
			SDL_Rect characterBox = { game.pozx - CHARACTER_WIDTH / 2, game.pozy - CHARACTER_HEIGHT / 2,CHARACTER_WIDTH , CHARACTER_HEIGHT };
			SDL_Rect barrelBox = { (int)game.barrelx[0] - BARREL_WIDTH / 2, (int)game.barrely[0] - BARREL_HEIGHT / 2, BARREL_WIDTH, BARREL_HEIGHT };
			SDL_Rect barrelBox1 = { (int)game.barrelx[1] - BARREL_WIDTH / 2, (int)game.barrely[1] - BARREL_HEIGHT / 2, BARREL_WIDTH, BARREL_HEIGHT };
			SDL_Rect barrelBox2 = { (int)game.barrelx[2] - BARREL_WIDTH / 2, (int)game.barrely[2] - BARREL_HEIGHT / 2, BARREL_WIDTH, BARREL_HEIGHT };
			SDL_Rect barrelBox3 = { (int)game.barrelx[3] - BARREL_WIDTH / 2, (int)game.barrely[3] - BARREL_HEIGHT / 2, BARREL_WIDTH, BARREL_HEIGHT };
			SDL_Rect barrelBox4 = { (int)game.barrelx[4] - BARREL_WIDTH / 2, (int)game.barrely[4] - BARREL_HEIGHT / 2, BARREL_WIDTH, BARREL_HEIGHT };
			if (CheckCollision(characterBox, barrelBox, barrelBox1, barrelBox2, barrelBox3, barrelBox4)) {
				game.lives--;
				if (game.lives == 0) {
					game.startgame = 4;
					
				}
				else {
					resetgame(&game);
					game.startgame = 2;
				}
			}
			
			if (CheckEndstage(&game)) {
				resetgame(&game);
				game.score += 100;
			}
			checkbarrelisjumped(&game);
			collectingoftrophy(&game);
			
			control(&game, &sdl);
			
		}
		game.frames++;
	}
	SDL_FreeSurface(sdl.charset);
	SDL_FreeSurface(sdl.screen);
	SDL_DestroyTexture(sdl.scrtex);
	SDL_DestroyRenderer(sdl.renderer);
	SDL_DestroyWindow(sdl.window);

	SDL_Quit();
	return 0;
	};
