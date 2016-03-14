#include <stdio.h>
#include "keyhandler.h"

Key_t key_from_raw_input(const char* buf) {
	int p[3] = {*buf, *(buf+1), *(buf+2)};
	Key_t k;

	if(p[0] == 27) {
		if(p[1] == 0 && p[2] == 0) { // esc
			k = KEY_ESC;
		} else if(p[1] == 91 && p[2] == 65) { // up
			k = KEY_UP;
		} else if(p[1] == 91 && p[2] == 66) { // down
			k = KEY_DOWN;
		} else if(p[1] == 91 && p[2] == 67) { // right
			k = KEY_RIGHT;
		} else if(p[1] == 91 && p[2] == 68) { // left
			k = KEY_LEFT;
		} else {
			k = KEY_ESC;
		}
	} else if(p[0] == 13 && p[1] == 0 && p[2] == 0) { //return
		k = KEY_ENTER;
	} else if(p[0] == 97 && p[1] == 0) {
		k = KEY_A;
	} else if(p[0] == 100 && p[1] == 0) {
		k = KEY_D;
	} else if(p[0] == 112 && p[1] == 0) {
		k = KEY_P;
	} else {
		k = NUM_KEYS;
	}

	return k;
}