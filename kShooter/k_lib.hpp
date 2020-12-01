#pragma once

class k_sprite {
private:
	int x, y, w, h;
	char *path;
	void *texture;
public:
	k_sprite(const char *path);
	~k_sprite();
	void set_position(int x, int y);
	void get_position(int &x, int &y);
	void add_position(int x, int y);
	void render();
};

namespace k_lib {
	void init(int w, int h);
	bool poll_events();
	void update();
	void clear();
	void msleep(int t);
	int mticks();

	enum {
		k_lib_bt_up = 0,
		k_lib_bt_down,
		k_lib_bt_left,
		k_lib_bt_right,
		k_lib_bt_1,
		k_lib_bt_2,
		k_lib_bt_3,
		k_lib_bt_4
	};

	bool button_down(int n);
};