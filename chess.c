#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include "SDL.h"
#include "png.h"

#define IMAGE_PATH "assets/pieces.png"
#define IMAGE_WIDTH 1024
#define IMAGE_HEIGHT 341
#define WINDOW_START_SIZE 400
#define COLOR(renderer, r, b, g, a) \
	sdl_check(SDL_SetRenderDrawColor(renderer, r, b, g, a))

struct Window {
	int tile_size;
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Texture *image_texture;
};

static inline int sdl_check(int code);
static inline void *sdl_ptr_check(void *ptr);
static void init_image(struct Window *window);
static struct Window init();
static inline void handle_resize(struct Window *window, SDL_Event *event);
static inline int handle_event(struct Window *window, SDL_Event *event);
static inline void clear(struct Window *window);
static inline void draw_board(struct Window *window);
static void run(struct Window *window);

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

void
init_image(struct Window *window)
{
	// FILE *file = fopen(IMAGE_PATH, "rb");
	// assert(file);
	// uint8_t header[8];
	// fread(header, sizeof(uint8_t), sizeof(header), file);
	// assert(png_sig_cmp(header, 0, sizeof(header)) != 0);
	png_image image;
	memset(&image, 0, sizeof(image));

	image.version = PNG_IMAGE_VERSION;
	assert(png_image_begin_read_from_file(&image, IMAGE_PATH));
	image.format = PNG_FORMAT_RGBA;

	uint32_t image_pixels[image.width * image.height];
	
	assert(png_image_finish_read(
		&image,
		NULL,
		image_pixels,
		0,
		NULL));
	
	SDL_Surface *image_surface =
		sdl_ptr_check(
			SDL_CreateRGBSurfaceFrom(
				image_pixels,
				 (int) image.width,
				 (int) image.height,
				 32,
				 (int) image.width * 4,
				 0x000000FF,
				 0x0000FF00,
				 0x00FF0000,
				 0xFF000000));

	window->image_texture = sdl_ptr_check(
		SDL_CreateTextureFromSurface(window->renderer,
			                     image_surface));
	SDL_FreeSurface(image_surface);
}

struct Window
init()
{
	sdl_check(SDL_Init(SDL_INIT_VIDEO));

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
		.renderer = sdl_ptr_check(
			SDL_CreateRenderer(
				w.window,
				-1,
				SDL_RENDERER_ACCELERATED |
				SDL_RENDERER_PRESENTVSYNC
			)
		),
		.image_texture = NULL,
		.tile_size = WINDOW_START_SIZE / 4,
	};

	init_image(&w);

	return w;
}

void
handle_resize(struct Window *window, SDL_Event *event)
{
	int newsize = event->window.data1 > event->window.data2 ?
		event->window.data2 : event->window.data1;

	if (newsize > 8 && newsize % 8)
		newsize -= newsize % 8;

	window->tile_size = newsize / 8;
}

void
handle_window_event(struct Window *window, SDL_Event *event)
{
	if (SDL_GetWindowID(window->window) != event->window.windowID)
		return;

	switch (event->window.event) {
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
	int color = 1; // 1 is white 0 is dark
	COLOR(window->renderer, 255, 255, 255, 255);

	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
			if (!x) {}
			else if (color) {
				COLOR(window->renderer, 34, 34, 34, 255);
				color = 0;
			} else {
				COLOR(window->renderer, 255, 255, 255, 255);
				color = 1;
			}

			const SDL_Rect r = {
				.x = x * window->tile_size,
				.y = y * window->tile_size,
				.w = window->tile_size,
				.h = window->tile_size
			};

			sdl_check(SDL_RenderFillRect(window->renderer, &r));
		}
	}

	SDL_Rect rect = {
		.x = 0,
		.y = 0,
		.w = IMAGE_WIDTH,
		.h = IMAGE_HEIGHT
	};

	SDL_RenderCopy(window->renderer, window->image_texture, &rect, &rect);
}

void
run(struct Window *window)
{
	int quit = 0;
	SDL_Event e;
	while (!quit) {
		while(SDL_PollEvent(&e))
			quit = handle_event(window, &e);

		clear(window);
		draw_board(window);
		SDL_RenderPresent(window->renderer);
	}
}

int
main()
{
	struct Window window = init();

	run(&window);

	SDL_DestroyTexture(window.image_texture);
	SDL_DestroyRenderer(window.renderer);
	SDL_DestroyWindow(window.window);
	SDL_Quit();

	return 0;
}
