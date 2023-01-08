#define _USE_MATH_DEFINES
#include<math.h>
#include<stdio.h>
#include<string.h>

extern "C" {
#include"./SDL2-2.0.10/include/SDL.h"
#include"./SDL2-2.0.10/include/SDL_main.h"
}

#define SCREEN_WIDTH	800
#define SCREEN_HEIGHT	700

//starting parameters
#define ROAD_WIDTH 382
#define CAR_SPEED 0.5

//colors
#define CZARNY SDL_MapRGB(screen->format, 0x00, 0x00, 0x00)
#define ZIELONY SDL_MapRGB(screen->format, 0x00, 0xFF, 0x00);
#define CZERWONY SDL_MapRGB(screen->format, 0xFF, 0x00, 0x00);
#define NIEBIESKI SDL_MapRGB(screen->format, 0x11, 0x11, 0xCC);

struct car {
	double pos_x;
	double pos_y;
	int width;
	int height;
	double speed;
	double x_vel;
	double y_vel;
	//SDL_Surface* graphics;
};

struct road {
	int pos_x;
	int pos_y;
	int speed;
	int width;
	//SDL_Surface *graphics;
};

struct player {
	//struct car players_car;
	double score;
	double distance;
};
// narysowanie napisu txt na powierzchni screen, zaczynaj¹c od punktu (x, y)
// charset to bitmapa 128x128 zawieraj¹ca znaki
// draw a text txt on surface screen, starting from the point (x, y)
// charset is a 128x128 bitmap containing character images
void DrawString(SDL_Surface *screen, int x, int y, const char *text,
                SDL_Surface *charset) {
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


// narysowanie na ekranie screen powierzchni sprite w punkcie (x, y)
// (x, y) to punkt œrodka obrazka sprite na ekranie
// draw a surface sprite on a surface screen in point (x, y)
// (x, y) is the center of sprite on screen
void DrawSurface(SDL_Surface *screen, SDL_Surface *sprite, int x, int y) {
	SDL_Rect dest;
	dest.x = x - sprite->w / 2;
	dest.y = y - sprite->h / 2;
	dest.w = sprite->w;
	dest.h = sprite->h;
	SDL_BlitSurface(sprite, NULL, screen, &dest);
	};


// rysowanie pojedynczego pixela
// draw a single pixel
void DrawPixel(SDL_Surface *surface, int x, int y, Uint32 color) {
	int bpp = surface->format->BytesPerPixel;
	Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
	*(Uint32 *)p = color;
	};


// rysowanie linii o d³ugoœci l w pionie (gdy dx = 0, dy = 1) 
// b¹dŸ poziomie (gdy dx = 1, dy = 0)
// draw a vertical (when dx = 0, dy = 1) or horizontal (when dx = 1, dy = 0) line
void DrawLine(SDL_Surface *screen, int x, int y, int l, int dx, int dy, Uint32 color) {
	for(int i = 0; i < l; i++) {
		DrawPixel(screen, x, y, color);
		x += dx;
		y += dy;
		};
	};


// rysowanie prostok¹ta o d³ugoœci boków l i k
// draw a rectangle of size l by k
void DrawRectangle(SDL_Surface *screen, int x, int y, int l, int k,
                   Uint32 outlineColor, Uint32 fillColor) {
	int i;
	DrawLine(screen, x, y, k, 0, 1, outlineColor);
	DrawLine(screen, x + l - 1, y, k, 0, 1, outlineColor);
	DrawLine(screen, x, y, l, 1, 0, outlineColor);
	DrawLine(screen, x, y + k - 1, l, 1, 0, outlineColor);
	for(i = y + 1; i < y + k - 1; i++)
		DrawLine(screen, x + 1, i, l - 2, 1, 0, fillColor);
	};

void render_road(double delta, struct road road, SDL_Surface *screen, SDL_Surface* road_graphics) {
	static double start_pos1 = SCREEN_HEIGHT - 1739;
	static double start_pos2 = SCREEN_HEIGHT - 2*1739;
	start_pos1 += road.speed * delta;
	start_pos2 += road.speed * delta;


	if (start_pos1 > 1739 + SCREEN_HEIGHT) {
		start_pos1 = -1739;
	};
	if (start_pos2 > 1739 + SCREEN_HEIGHT) {
		start_pos2 = -1739;
	};
	DrawSurface(screen, road_graphics, SCREEN_WIDTH / 2, start_pos1);
	DrawSurface(screen, road_graphics, SCREEN_WIDTH / 2, start_pos2);
}

void free_surfaces(int count, SDL_Surface** tab[], SDL_Texture** scrtex, SDL_Surface** screen, SDL_Window** window, SDL_Renderer** renderer){
	for (int i = 0; i < count; i++) {
		SDL_FreeSurface(*tab[i]);
	}
	SDL_DestroyTexture(*scrtex);
	SDL_DestroyWindow(*window);
	SDL_DestroyRenderer(*renderer);
}

void keydown(SDL_Event* event, struct car* car_1, int* quit){
	switch (event->key.keysym.sym) {
	case SDLK_LEFT:
		car_1->x_vel = -car_1->speed;
		break;
	case SDLK_RIGHT:
		car_1->x_vel = car_1->speed;
		break;
	case SDLK_UP:
		car_1->y_vel = -car_1->speed;
		break;
	case SDLK_DOWN:
		car_1->y_vel = car_1->speed;
		break;
	case SDLK_ESCAPE:
		*quit = 1;
	default:
		break;
	}
}

void keyup(SDL_Event* event, struct car* car_1, int* quit) {
	switch (event->key.keysym.sym) {
	case SDLK_LEFT:
		if (car_1->x_vel < 0)
			car_1->x_vel = 0;
		break;
	case SDLK_RIGHT:
		if (car_1->x_vel > 0)
			car_1->x_vel = 0;
		break;
	case SDLK_UP:
		if (car_1->y_vel < 0)
			car_1->y_vel = 0;
		break;
	case SDLK_DOWN:
		if (car_1->y_vel > 0)
			car_1->y_vel = 0;
		break;
	default:
		break;
	}
}

void newgame(struct player* player_1, struct car* car_1, double * worldTime) {
	player_1->score = 0;
	car_1->pos_x = SCREEN_WIDTH / 2;
	car_1->pos_y = SCREEN_HEIGHT * 2 / 3;
	*worldTime = 0;
}

void controls(SDL_Event* event, struct car* car_1, int* quit, struct player *player_1, double *worldTime, int *pause) {
	while (SDL_PollEvent(event)) {
		switch (event->type) {
		case SDL_KEYDOWN:
			keydown(event, car_1, quit);
			break;
		case SDL_KEYUP:
			if (event->key.keysym.sym == 'n') newgame(player_1, car_1, worldTime);
			if (event->key.keysym.sym == 'p') {
				*pause = (*pause + 1) % 2;
				printf("%d", *pause);
				//break;
			}
			keyup(event, car_1, quit);
			break;
		case SDL_QUIT:
			*quit = 1;
		default:
			break;
		}
	}
/* Update the car position */
		car_1->pos_x += car_1->x_vel;
		car_1->pos_y += car_1->y_vel;
}

void print_info(double worldTime, double fps, SDL_Surface* charset, SDL_Surface* screen, SDL_Renderer* renderer, SDL_Texture* scrtex, int score) {
	// tekst informacyjny / info text
		//DrawRectangle(screen, 4, 4, SCREEN_WIDTH - 8, 36, czerwony, niebieski);
		//            "template for the second project, elapsed time = %.1lf s  %.0lf frames / s"
	char text[128];
	sprintf(text, "Szablon drugiego zadania, czas trwania = %.1lf s  %.0lf klatek / s", worldTime, fps);
	DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, 10, text, charset);
	//	      "Esc - exit, \030 - faster, \031 - slower"
	sprintf(text, "Esc - wyjscie, \030 - przyspieszenie, \031 - zwolnienie");
	DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, 26, text, charset);

	sprintf(text, "score: %d", score);
	DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, 36, text, charset);

	SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
	//		SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, scrtex, NULL, NULL);
	SDL_RenderPresent(renderer);
}

int initialize(SDL_Window** window, int* rc, SDL_Renderer** renderer, SDL_Surface** screen, SDL_Texture** scrtex) {
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("SDL_Init error: %s\n", SDL_GetError());
		return 1;
	}
	*rc = SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, window, renderer);
	if (*rc != 0) {
		SDL_Quit();
		printf("SDL_CreateWindowAndRenderer error: %s\n", SDL_GetError());
		return 1;
	};

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	SDL_RenderSetLogicalSize(*renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
	SDL_SetRenderDrawColor(*renderer, 0, 0, 0, 255);

	SDL_SetWindowTitle(*window, "Szablon do zdania drugiego 2017");


	*screen = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32,
		0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);

	*scrtex = SDL_CreateTexture(*renderer, SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		SCREEN_WIDTH, SCREEN_HEIGHT);
	
	// wy³¹czenie widocznoœci kursora myszy
	SDL_ShowCursor(SDL_DISABLE);

	return 0;
}

int load_graphics(int img_number, SDL_Surface** img[], const char paths[][100], SDL_Surface** screen, SDL_Texture** scrtex, SDL_Window** window, SDL_Renderer** renderer) {
	for (int i = 0; i < img_number; i++) {
		*img[i] = SDL_LoadBMP(paths[i]);
		if (*img[i] == NULL) {
			printf("SDL_LoadBMP() error: %s\n", SDL_GetError());
			free_surfaces(img_number, img, scrtex, screen, window, renderer);
			SDL_Quit();
			return 1;
		}
	}
	SDL_SetColorKey(*img[0], true, 0x000000);
	return 0;
}

void time(int* t1, int* t2, double* delta, double* worldTime, struct player *player_1, struct car *car_1, struct road road, int pause) {
	*t2 = SDL_GetTicks();
	*delta = (*t2 - *t1) * 0.001;
	*t1 = *t2;
	if (!pause) {
		*worldTime += *delta;
		player_1->distance += car_1->speed * *delta;
		player_1->score += road.speed * (*delta) / 100;
	}
}

void gameover(SDL_Surface** screen, SDL_Surface** charset, SDL_Renderer* renderer, SDL_Texture* scrtex, SDL_Event* event,struct player* player_1, struct car* car_1) {
	SDL_FillRect(*screen, NULL, SDL_MapRGB((*screen)->format, 0x00, 0x00, 0x00));
	char text[128];
	sprintf(text, "game over");
	DrawString(*screen, (*screen)->w / 2 - strlen(text) * 8 / 2, 100, text, *charset);
	SDL_UpdateTexture(scrtex, NULL, (*screen)->pixels, (*screen)->pitch);
	//		SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, scrtex, NULL, NULL);
	SDL_RenderPresent(renderer);
}



int collisions(struct car* car_1, struct road *road, SDL_Surface** screen, SDL_Surface** charset, struct player *player) {
	if (car_1->pos_x + (car_1->width) / 2 > road->pos_x + (road->width) / 2){
		printf("1");
		//gameover(screen, charset);
		//newgame(player, car_1);
		return 1;
	}
	else if (car_1->pos_x - (car_1->width) / 2 < road->pos_x - (road->width) / 2) {
		printf("2");
		return 1;
	}
	if (car_1->pos_y - (car_1->height) / 2 < 0) {
		car_1->pos_y = (car_1->height) / 2;
	}
	else if (car_1->pos_y + (car_1->height) / 2 > SCREEN_HEIGHT) {
		car_1->pos_y = SCREEN_HEIGHT - (car_1->height) / 2;
	}
	return 0;
}



#ifdef __cplusplus
extern "C"
#endif
int main(int argc, char **argv) {
	int t1, t2, quit = 0, frames = 0, rc, pause = 0, pausetime = 0;
	double delta, worldTime = 0, fpsTimer = 0, fps = 0;
	SDL_Event event;
	SDL_Surface *screen, *charset, *car_graphics, *road_graphics;
	SDL_Texture *scrtex;
	SDL_Window *window;
	SDL_Renderer *renderer;
	struct car car_1 = { SCREEN_WIDTH/2,SCREEN_HEIGHT-50,50,50, CAR_SPEED,0,0 };
	struct road road = { SCREEN_WIDTH / 2,SCREEN_HEIGHT / 2, 500, ROAD_WIDTH };
	struct player player_1 = { 0, 0 };

	if (initialize(&window, &rc, &renderer, &screen, &scrtex) == 1) {
		return 1;
	}

	// wczytanie obrazkow
	SDL_Surface** tab2[3] = {&charset, &car_graphics, &road_graphics};
	const char paths[3][100] = {"./cs8x8.bmp", "./car2.bmp", "./road_straight.bmp"};
	load_graphics(3, tab2, paths, &screen, &scrtex, &window, &renderer);
	SDL_SetColorKey(charset, true, 0x000000);

	t1 = SDL_GetTicks();
	double start_pos1 = SCREEN_HEIGHT-1739;
	double start_pos2 = SCREEN_HEIGHT - 1739 -1739;

	while (!quit) {
		time(&t1, &t2, &delta, &worldTime, &player_1, &car_1, road, pause); //get time and count distance
		if (!pause) {
			SDL_FillRect(screen, NULL, CZARNY);

			if (collisions(&car_1, &road, &screen, &charset, &player_1) == 0) {
				render_road(delta, road, screen, road_graphics);
				DrawSurface(screen, car_graphics, car_1.pos_x, car_1.pos_y);
				print_info(worldTime, fps, charset, screen, renderer, scrtex, player_1.score);
			}
			else {
				gameover(&screen, &charset, renderer, scrtex, &event, &player_1, &car_1);
			}

		}
		// obs³uga zdarzeñ (o ile jakieœ zasz³y) / handling of events (if there were any)
		controls(&event, &car_1, &quit, &player_1, &worldTime, &pause);
	}
	// zwolnienie powierzchni / freeing all surfaces
	free_surfaces(3, tab2, &scrtex, &screen, &window, &renderer);
	SDL_Quit();
	return 0;
};