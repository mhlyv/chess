#include <stdio.h>
#include <stdlib.h>
#include "SDL.h"

struct Window {
	int tile_size;
	SDL_Window *window;
	SDL_Surface *surface;
	SDL_Renderer *renderer;
};

static inline int sdl_check(int code);
static inline void *sdl_ptr_check(void *ptr);
static struct Window init();
static inline void handle_resize(struct Window *window, SDL_Event *event);
static inline int handle_event(struct Window *window, SDL_Event *event);
static inline void clear(struct Window *window);
static inline void draw_board(struct Window *window);
static void run(struct Window *window);

#define WINDOW_START_SIZE 400
#define COLOR(renderer, r, b, g, a) \
	sdl_check(SDL_SetRenderDrawColor(renderer, r, b, g, a))

int
sdl_check(int code)
{
	if (code < 0) {
		fprintf(stderr, "SDL error: %s\n", SDL_GetError());
		abort();
	}

	return code;
}

void *
sdl_ptr_check(void *ptr)
{
	if (!ptr) {
		fprintf(stderr, "SDL error: %s\n", SDL_GetError());
		abort();
	}

	return ptr;
}

struct Window
init()
{
	sdl_check(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS));

	struct Window w = {
		.window = sdl_ptr_check(
			SDL_CreateWindow(
				"chess",
				SDL_WINDOWPOS_UNDEFINED,
				SDL_WINDOWPOS_UNDEFINED,
				WINDOW_START_SIZE,
				WINDOW_START_SIZE,
				SDL_WINDOW_SHOWN |
				SDL_WINDOW_RESIZABLE |
				SDL_WINDOW_ALLOW_HIGHDPI
			)
		),
		.surface = sdl_ptr_check(
			SDL_GetWindowSurface(w.window)
		),
		.renderer = sdl_ptr_check(
			SDL_CreateRenderer(
				w.window,
				-1,
				SDL_RENDERER_ACCELERATED |
				SDL_RENDERER_PRESENTVSYNC
			)
		),
		.tile_size = WINDOW_START_SIZE / 4
	};

	return w;
}

void
handle_resize(struct Window *window, SDL_Event *event)
{
	int newsize = event->window.data1 > event->window.data2 ?
		event->window.data2 : event->window.data1;
	while (newsize > 8 && newsize % 8)
		newsize--;

	window->tile_size = newsize / 8;
}

void
handle_window_event(struct Window *window, SDL_Event *event)
{
	if (SDL_GetWindowID(window->window) != event->window.windowID)
		return;

	switch (event->window.event) {
	case SDL_WINDOWEVENT_RESIZED: {
		handle_resize(window, event);
	} break;
	case SDL_WINDOWEVENT_SIZE_CHANGED: {
		handle_resize(window, event);
	} break;
	}
}

int
handle_event(struct Window *window, SDL_Event *event)
{
	switch (event->type) {
	case SDL_QUIT: {
		return 1;
	} break;
	case SDL_WINDOWEVENT: {
		handle_window_event(window, event);
	} break;
	}

	return 0;
}

void
clear(struct Window *window)
{
	COLOR(window->renderer, 0, 0, 0, 255);
	sdl_check(SDL_RenderClear(window->renderer));
}

void
draw_board(struct Window *window)
{
}

void
run(struct Window *window)
{
	int quit = 0;
	SDL_Event e;
	while (!quit) {
		while(SDL_PollEvent(&e)) {
			quit = handle_event(window, &e);
		}

		clear(window);

		COLOR(window->renderer, 255, 255, 255, 255);
		sdl_check(
			SDL_RenderDrawLine(
				window->renderer,
				0,
				0,
				window->tile_size,
				window->tile_size
			)
		);

		SDL_RenderPresent(window->renderer);
	}
}

int
main()
{
	struct Window w = init();
	run(&w);
	SDL_Quit();
	return 0;
}
