#include "k_lib.hpp"
#include <SDL/SDL.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//----------<klib>

namespace k_lib {
	SDL_Window *window;
	SDL_Event event;
	SDL_Renderer *renderer;
};

void k_lib::init(int w, int h) {
	SDL_Init(SDL_INIT_VIDEO);
	window = SDL_CreateWindow(
		"SDL2",                            // ウィンドウのタイトル
		SDL_WINDOWPOS_UNDEFINED,           // X座標の初期値
		SDL_WINDOWPOS_UNDEFINED,           // Y座標の初期値
		640,                               // 幅のピクセル数
		480,                               // 高さのピクセル数
		0                                  // フラグ
	);

	renderer = SDL_CreateRenderer(window, -1, 0);
	SDL_RenderClear(k_lib::renderer);
}

bool k_lib::poll_events() {
	if (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT:
			return true;
		}
	}

	return false;
}

void k_lib::clear() {
	SDL_RenderClear(k_lib::renderer);
}

void k_lib::update() {
	SDL_RenderPresent(k_lib::renderer);
}

void k_lib::msleep(int t) {
	SDL_Delay(t);
}

int k_lib::mticks() {
	return SDL_GetTicks();
}

bool k_lib::button_down(int n) {
	const Uint8 *state = SDL_GetKeyboardState(NULL);

	if (n == k_lib_bt_up	&& state[SDL_SCANCODE_UP])		return true;
	if (n == k_lib_bt_down	&& state[SDL_SCANCODE_DOWN])	return true;
	if (n == k_lib_bt_left	&& state[SDL_SCANCODE_LEFT])	return true;
	if (n == k_lib_bt_right	&& state[SDL_SCANCODE_RIGHT])	return true;

	if (n == k_lib_bt_1		&& state[SDL_SCANCODE_SPACE])	return true;
	if (n == k_lib_bt_2		&& state[SDL_SCANCODE_Z])		return true;
	if (n == k_lib_bt_3		&& state[SDL_SCANCODE_X])		return true;
	if (n == k_lib_bt_4		&& state[SDL_SCANCODE_C])		return true;

	return false;
}

//----------</klib>

k_sprite::k_sprite(const char *path) {
	this->x = 0;
	this->y = 0;

	int dmy;

	stbi_uc *pix = stbi_load(path, &w, &h, &dmy, 4);

	texture = SDL_CreateTexture(k_lib::renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, w, h);
	SDL_UpdateTexture((SDL_Texture *)texture, NULL, pix, w * 4);

	stbi_image_free(pix);
}

k_sprite::~k_sprite() {
	SDL_DestroyTexture((SDL_Texture *)texture);
}

void k_sprite::set_position(int x, int y) {
	this->x = x;
	this->y = y;
}

void k_sprite::get_position(int &x, int &y) {
	x = this->x;
	y = this->y;
}

void k_sprite::add_position(int x, int y) {
	this->x += x;
	this->y += y;
}

void k_sprite::render() {
	SDL_Rect r;

	r.x = x;
	r.y = y;
	r.w = w;
	r.h = h;

	SDL_RenderCopy(k_lib::renderer, (SDL_Texture *)texture, NULL, &r);
}