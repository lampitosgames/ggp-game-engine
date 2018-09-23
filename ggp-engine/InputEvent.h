#ifndef GGP_INPUT_EVENT_H
#define GGP_INPUT_EVENT_H

enum InputType {
	MOUSE_BUTTON = 0,
	MOUSE_MOVE = 1,
	MOUSE_SCROLL = 2,
	KEY = 3
};

struct InputEvent {
	//Type of event. Possibly add controller/vive support later
	InputType type;
	//True if the key is being held down (was in pressed state last frame)
	//Only true when a key is held
	bool held;
	//True if the key was pressed this update(), false if the key was released this update()
	//InputEvents only fire on key state change
	bool pressed;
	//Keycode for the pressed button
	int keyCode;

	/*
		Mouse-related variables
	*/
	//Previous mouse position
	int pMX;
	int pMY;
	//Mouse position
	int mX;
	int mY;
	//Mouse displacement
	int mMoveX;
	int mMoveY;
	//Scroll wheel delta
	float mDelta;

	//Constructor for key event
	InputEvent(InputType _type, bool _held, bool _pressed, int _keyCode) {
		type = _type;
		held = _held;
		pressed = _pressed;
		keyCode = _keyCode;
	}
	//Constructor for mouse move event
	InputEvent(InputType _type, int _pmx, int _pmy, int _mx, int _my) {
		type = _type;
		pMX = _pmx;
		pMY = _pmy;
		mX = _mx;
		mY = _my;
		mMoveX = mX - pMX;
		mMoveY = mY - pMY;
	}
	//Constructor for mouse scroll event
	InputEvent(InputType _type, float _delta) {
		type = _type;
		mDelta = _delta;
	}
};

#endif //GGP_INPUT_EVENT_H