#include "k_lib.hpp"

#include <string.h>

int main(int argc, char *argv[])
{
	k_lib::init(640, 480);

	k_sprite *spr = new k_sprite("test.png");
	k_sprite *spr2 = new k_sprite("test.png");

	int bpos[3][20];

	memset(bpos[0], 0, 20 * sizeof(int));
	memset(bpos[1], 0, 20 * sizeof(int));
	memset(bpos[2], 0, 20 * sizeof(int));

	while (!k_lib::poll_events()) {
		k_lib::clear();

		spr->render();

		if (k_lib::button_down(k_lib::k_lib_bt_up)) spr->add_position(0, -1);
		if (k_lib::button_down(k_lib::k_lib_bt_down)) spr->add_position(0, 1);
		if (k_lib::button_down(k_lib::k_lib_bt_left)) spr->add_position(-1, 0);
		if (k_lib::button_down(k_lib::k_lib_bt_right)) spr->add_position(1, 0);

		if (k_lib::button_down(k_lib::k_lib_bt_1)) {
			for (int i = 0; i < 20; i++) {
				if (bpos[2][i] == 0) {
					bpos[2][i] = 1;
					spr->get_position(bpos[0][i], bpos[1][i]);
					break;
				}
			}
		}

		for (int i = 0; i < 20; i++) {
			if (bpos[2][i] == 1) {
				spr2->set_position(bpos[0][i], bpos[1][i]);
				spr2->render();
				bpos[1][i] -= 8;
				if (bpos[1][i] < -20) bpos[2][i] = 0;
			}
		}

		k_lib::update();
		k_lib::msleep(10);
	}

	return 0;
}