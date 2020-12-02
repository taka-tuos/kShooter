#include "k_lib.hpp"

#include <string.h>
#include <stdlib.h>

class k_bullet {
private:
	enum {
		k_pos_x,
		k_pos_y,
		k_used,
		k_vel_x,
		k_vel_y,
		k_alloc_num
	};

	int bullet_max;
	int radius;
	int *position[5];
	void(*render_func)(int, int, int, void*);
	int(*move_func)(int, int&, int&, int&, int&, void*);
	void(*velocity_func)(int, int&, int&, int&, int&, void*);

	void *render_ext;
	void *move_ext;
	void *velocity_ext;

public:
	k_bullet(int radius = 4, int max = 100) {
		this->bullet_max = max;
		this->radius = radius;
		for (int i = 0; i < k_alloc_num; i++) {
			position[i] = (int *)malloc(bullet_max * sizeof(int));
			memset(position[i], 0, bullet_max * sizeof(int));
		}
	}

	~k_bullet() {
		for (int i = 0; i < k_alloc_num; i++) {
			free(position[i]);
		}
	}

	void set_render_func(void(*render)(int, int, int, void*)) {
		this->render_func = render;
	}

	void set_render_ext(void *ext) {
		this->render_ext = ext;
	}

	void set_move_func(int(*move)(int, int&, int&, int&, int&, void*)) {
		this->move_func = move;
	}

	void set_move_ext(void *ext) {
		this->move_ext = ext;
	}

	void set_velocity_func(void(*velocity)(int, int&, int&, int&, int&, void*)) {
		this->velocity_func = velocity;
	}

	void set_velocity_ext(void *ext) {
		this->velocity_ext = ext;
	}

	void render() {
		for (int i = 0; i < bullet_max; i++) {
			if (position[k_used][i]) {
				render_func(i, position[k_pos_x][i], position[k_pos_y][i], render_ext);
			}
		}
	}

	void move() {
		for (int i = 0; i < bullet_max; i++) {
			if (position[k_used][i]) {
				position[k_used][i] = move_func(i, position[k_pos_x][i], position[k_pos_y][i], position[k_vel_x][i], position[k_vel_y][i], move_ext);
			}
		}
	}

	void shot(int x, int y) {
		for (int i = 0; i < bullet_max; i++) {
			if (!position[k_used][i]) {
				position[k_used][i] = 1;
				position[k_pos_x][i] = x;
				position[k_pos_y][i] = y;

				velocity_func(i, position[k_pos_x][i], position[k_pos_y][i], position[k_vel_x][i], position[k_vel_y][i], velocity_ext);

				break;
			}
		}
	}

	int check_hit(int x, int y, int r) {
		for (int i = 0; i < bullet_max; i++) {
			if (position[k_used][i]) {
				int px1 = x;
				int py1 = y;
				int px2 = position[k_pos_x][i];
				int py2 = position[k_pos_y][i];
				int r1 = r;
				int r2 = radius;
				if ((px2 - px1) * (px2 - px1) + (py2 - py1) * (py2 - py1) <= (r1 + r2) * (r1 + r2)) {
					position[k_used][i] = 0;
					return 1;
				}
			}
		}
		return 0;
	}
};

int bullet_move_vel(int idx, int &x, int &y, int &vx, int &vy, void *ext) {
	x += vx;
	y += vy;
	if (y < -20) return 0;
	if (y >= k_lib::get_height() + 20) return 0;
	if (x < -20) return 0;
	if (x >= k_lib::get_width() + 20) return 0;

	return 1;
}

void bullet_velocity_up(int idx, int &x, int &y, int &vx, int &vy, void *ext) {
	vx = 0;
	vy = -16;
}

void bullet_render_normal(int idx, int x, int y, void *ext) {
	k_sprite *spr = (k_sprite *)ext;
	spr->set_position(x - spr->get_width() / 2, y - spr->get_height() / 2);
	spr->render();
}

class k_enemy {
protected:
	int x, y;
public:
	virtual int move() = 0;
	virtual void render() = 0;

	virtual int get_points() = 0;
	virtual int get_radius() = 0;

	k_enemy(int x, int y) {
		this->x = x;
		this->y = y;
	}

	void get_position(int &x, int &y) {
		x = this->x;
		y = this->y;
	}
};

class k_enemy_zako : public k_enemy {
private:
	int state;
	int state2;
	k_sprite *spr;
public:
	k_enemy_zako(int x, int y) : k_enemy(x, y) {
		spr = new k_sprite("test.png");
		state = 0;
		if (x > k_lib::get_width() / 2) state2 = 1;
		else state2 = 0;
	}

	~k_enemy_zako() {
		delete spr;
	}

	int move() {
		switch(state) {
		case 0:
			y += 10;
			if (y > k_lib::get_height() - 200) state++;
			break;
		case 1:
			if (state2) {
				if (x > k_lib::get_width() / 2) x -= 6;
				else x = k_lib::get_width() / 2;
			} else {
				if (x < k_lib::get_width() / 2) x += 6;
				else x = k_lib::get_width() / 2;
			}
			y -= 6;
			if (y < -20) return 0;
			break;
		}

		return 1;
	}

	void render() {
		spr->set_position(x - spr->get_width() / 2, y - spr->get_height() / 2);
		spr->render();
	}

	int get_points() {
		return 100;
	}

	int get_radius() {
		return 8;
	}
};

class k_enemy_queue {
private:
	k_enemy *enemies[100];
	struct {
		k_enemy *inst;
		int ttl;
	} queue[200];
public:
	k_enemy_queue() {
		for (int i = 0; i < 200; i++) {
			queue[i].ttl = 0;
			queue[i].inst = NULL;
		}

		for (int i = 0; i < 100; i++) {
			enemies[i] = NULL;
		}
	}

	void tick() {
		for (int i = 0; i < 200; i++) {
			if (queue[i].inst != NULL) {
				queue[i].ttl--;
				if (queue[i].ttl == 0) {
					for (int j = 0; j < 100; j++) {
						if (enemies[j] == NULL) {
							enemies[j] = queue[i].inst;
							queue[i].inst = NULL;
							break;
						}
					}
				}
			}
		}

		for (int i = 0; i < 100; i++) {
			if (enemies[i] != NULL) {
				int r = enemies[i]->move();
				if (!r) {
					delete enemies[i];
					enemies[i] = NULL;
				}
			}
		}
	}

	void render() {
		for (int i = 0; i < 100; i++) {
			if (enemies[i] != NULL) {
				enemies[i]->render();
			}
		}
	}

	int bullet_check(k_bullet *b) {
		int score = 0;
		for (int i = 0; i < 100; i++) {
			if (enemies[i] != NULL) {
				int x, y, r;
				r = enemies[i]->get_radius();
				enemies[i]->get_position(x, y);
				if (b->check_hit(x, y, r)) {
					score += enemies[i]->get_points();
					delete enemies[i];
					enemies[i] = NULL;
				}
			}
		}
		return score;
	}

	bool player_check(int x, int y, int r) {
		for (int i = 0; i < 100; i++) {
			if (enemies[i] != NULL) {
				int px1, py1, r1;
				int px2, py2, r2;
				r2 = enemies[i]->get_radius();
				enemies[i]->get_position(px2, py2);

				px1 = x;
				py1 = y;
				r1 = r;

				if ((px2 - px1) * (px2 - px1) + (py2 - py1) * (py2 - py1) <= (r1 + r2) * (r1 + r2)) {
					return true;
				}
			}
		}
		return false;
	}

	void push_queue(k_enemy *inst, int ttl) {
		for (int i = 0; i < 200; i++) {
			if (queue[i].inst == NULL) {
				queue[i].inst = inst;
				queue[i].ttl = ttl;
				break;
			}
		}
	}
};

int main(int argc, char *argv[])
{
	k_lib::init(480, 640);

	k_sprite *spr = new k_sprite("test.png");
	k_sprite *spr2 = new k_sprite("test.png");

	k_bullet *pblt = new k_bullet(16);
	int timr_pblt = 0;

	int g_frm = 0;

	pblt->set_move_func(bullet_move_vel);
	pblt->set_velocity_func(bullet_velocity_up);
	pblt->set_render_func(bullet_render_normal);
	pblt->set_render_ext(spr2);

	k_enemy *zako = new k_enemy_zako(10, 0);

	k_enemy_queue equeue;
	equeue.push_queue(zako, 100);

	int timr_fps = k_lib::mticks();

	spr->set_position(k_lib::get_width() / 2  - spr->get_width() / 2, k_lib::get_height() / 2);

	while (!k_lib::poll_events()) {
		if (k_lib::button_down(k_lib::k_lib_bt_up)) spr->add_position(0, -4);
		if (k_lib::button_down(k_lib::k_lib_bt_down)) spr->add_position(0, 4);
		if (k_lib::button_down(k_lib::k_lib_bt_left)) spr->add_position(-3, 0);
		if (k_lib::button_down(k_lib::k_lib_bt_right)) spr->add_position(3, 0);

		if (k_lib::button_down(k_lib::k_lib_bt_1)) {
			int x, y;
			spr->get_position(x, y);
			if (g_frm - timr_pblt > 2) {
				pblt->shot(x + spr->get_width() / 2, y + spr->get_height() / 2);
				timr_pblt = g_frm;
			}
		}

		pblt->move();

		//zako->move();
		//zako->render();
		equeue.tick();
		equeue.bullet_check(pblt);

		int x, y;
		spr->get_position(x, y);
		if (equeue.player_check(x, y, 16)) {
			spr->set_position(k_lib::get_width() / 2 - spr->get_width() / 2, k_lib::get_height() / 2);
		}

		k_lib::clear();

		equeue.render();
		pblt->render();
		spr->render();

		k_lib::update();

		int dif_timr = k_lib::mticks() - timr_fps;

		k_lib::msleep(15 - dif_timr);

		timr_fps = k_lib::mticks();
		g_frm++;
	}

	return 0;
}