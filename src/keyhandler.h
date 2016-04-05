#ifndef KEYHANDLER_H
#define KEYHANDLER_H

typedef enum {
	KEY_ENTER,
	KEY_UP,
	KEY_DOWN,
	KEY_LEFT,
	KEY_RIGHT,
	KEY_A,
	KEY_D,
	KEY_P,
	KEY_ESC,
	KEY_Q,
	NUM_KEYS
} Key_t;

Key_t key_from_raw_input(const char* buf);

#endif