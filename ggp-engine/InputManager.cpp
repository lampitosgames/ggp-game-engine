#include "stdafx.h"

#include "InputManager.h"
#include "GameObject.h"
#include "InputListener.h"

using namespace std;

InputManager* InputManager::instance = nullptr;

InputManager* InputManager::GetInstance() {
	if (instance == nullptr) {
		instance = new InputManager();
	}
	return instance;
}

void InputManager::ReleaseInstance() {
	if (instance != nullptr) {
		delete instance;
		instance = nullptr;
	}
}

void InputManager::Update() {
	//Iterate through the keyState map
	map<int, bool>::iterator ksIt;
	for (ksIt = keyState.begin(); ksIt != keyState.end(); ++ksIt) {
		//Don't handle mouse input here.  Handle it in the mouse event listeners coming directly from Game.cpp
		//Users can software-rebind their mouse buttons and GetAsyncKeyState won't recognize it
		//NOTE: Extra mouse buttons will still be checked like other keyboard keys
		if (ksIt->first == VK_LBUTTON || ksIt->first == VK_RBUTTON || ksIt->first == VK_MBUTTON) {
			continue;
		}
		//Store current key state in previous state
		prevKeyState[ksIt->first] = ksIt->second;
		//Store new key state by querying windows
		keyState[ksIt->first] = GetKeyState(ksIt->first) & 0x8000;

		//Check if we need to dispatch events
		//If the key was just pressed
		if (!prevKeyState[ksIt->first] && keyState[ksIt->first]) {
			//Dispatch a key pressed event
			Dispatch(InputEvent(InputType::KEY, false, true, ksIt->first));
		//If the key was just released
		} else if (prevKeyState[ksIt->first] && !keyState[ksIt->first]) {
			//Dispatch a key released event
			Dispatch(InputEvent(InputType::KEY, false, false, ksIt->first));
		//If the key is being held down
		} else if (prevKeyState[ksIt->first] && keyState[ksIt->first]) {
			//Dispatch a held event (Key is being continually pressed)
			Dispatch(InputEvent(InputType::KEY, true, true, ksIt->first));
		}
	}
}

UINT InputManager::AddInputListener(InputListener* _inputListener) {
	inputListenerUIDMap[ilCount] = _inputListener;
	ilCount++;
	return ilCount - 1;
}

InputListener* InputManager::GetInputListener(InputListenerID _uniqueID) {
	auto thisIL = inputListenerUIDMap.find(_uniqueID);
	//If found, return it.  Else, return nullptr
	if (thisIL != inputListenerUIDMap.end()) {
		return thisIL->second;
	}
	return nullptr;
}

void InputManager::RemoveInputListener(InputListener * _inputListener) {
	auto ilIt = inputListenerUIDMap.begin();
	for (; ilIt != inputListenerUIDMap.end(); ++ilIt) {
		if (ilIt->second == _inputListener) {
			inputListenerUIDMap[ilIt->first] = nullptr;
		}
	}
}

bool InputManager::ActionPressed(string _action) {
	//Nothing is being pressed until proven otherwise
	bool actionPressed = false;
	//Loop through the relevant keys bound to this action
	for (vector<int>::iterator i = keybinds[_action].begin(); i != keybinds[_action].end(); i++) {
		//If this key's entry in the keyState is pressed, set actionPressed to true
		if (keyState[*i]) {
			actionPressed = true;
		}
	}
	//Return whether or not one of the bound keys is currently pressed
	return actionPressed;
}

bool InputManager::ActionPressed(std::string _action, InputEvent _event) {
	//Quick check to make sure this is a key press event
	if (!_event.pressed) { return false; }
	//Loop through the keybinds for this action
	for (vector<int>::iterator i = keybinds[_action].begin(); i != keybinds[_action].end(); i++) {
		//If the pressed key is bound to this action, return true
		if (*i == _event.keyCode) {
			return true;
		}
	}
	//If we got past the loop, the pressed key is not bound to this action
	return false;
}

//:: BUG - Sometimes fires two events
bool InputManager::ActionReleased(std::string _action, InputEvent _event) {
	//Quick check to make sure this is a key release event
	if (_event.pressed) { return false; }
	//Loop through the keybinds for this action
	for (vector<int>::iterator i = keybinds[_action].begin(); i != keybinds[_action].end(); i++) {
		//If the released key is bound to this action, return true
		if (*i == _event.keyCode) {
			return true;
		}
	}
	//If we got past the loop, the released key is not bound to this action
	return false;
}

bool InputManager::ActionHeld(std::string _action, InputEvent _event) {
	//Quick check to make sure this is a key held event
	//Held event triggers every frame AFTER the first frame the key is pressed
	//Usually it is better to just check ActionPressed() in the object's update() method
	//unless there is some *specific* functionality desired for held keys (like holding a key for x seconds)
	if (!_event.held) { return false; }
	//Loop through the keybinds for this action
	for (vector<int>::iterator i = keybinds[_action].begin(); i != keybinds[_action].end(); i++) {
		//If the held key is bound to this action, return true
		if (*i == _event.keyCode) {
			return true;
		}
	}
	//If we got past the loop, the held key is not bound to this action
	return false;
}

int* InputManager::GetPrevMousePos() { return prevMousePos; }

int* InputManager::GetMousePos() { return mousePos; }

bool InputManager::GetMouseLocked() {
	return mouseLocked;
}

void InputManager::SetMouseLocked(bool _val) { mouseLocked = _val; }

void InputManager::_OnMouseUp(WPARAM _buttonState, int _x, int _y) {
	//Store current mouse state in previous mouse state
	prevKeyState[VK_LBUTTON] = keyState[VK_LBUTTON];
	prevKeyState[VK_RBUTTON] = keyState[VK_RBUTTON];
	prevKeyState[VK_MBUTTON] = keyState[VK_MBUTTON];

	//Store new key states
	keyState[VK_LBUTTON] = _buttonState & 0x0001;
	keyState[VK_RBUTTON] = _buttonState & 0x0002;
	keyState[VK_MBUTTON] = _buttonState & 0x0010;

	//Check left button for mouse up event
	if (!keyState[VK_LBUTTON]) {
		Dispatch(InputEvent(InputType::MOUSE_BUTTON, false, false, VK_LBUTTON));
	}
	//Check right button for mouse up event
	if (!keyState[VK_RBUTTON]) {
		Dispatch(InputEvent(InputType::MOUSE_BUTTON, false, false, VK_RBUTTON));
	}
	//Check middle button for mouse up event
	if (!keyState[VK_MBUTTON]) {
		Dispatch(InputEvent(InputType::MOUSE_BUTTON, false, false, VK_MBUTTON));
	}
}

void InputManager::_OnMouseDown(WPARAM _buttonState, int _x, int _y) {
	//Store current mouse state in previous mouse state
	prevKeyState[VK_LBUTTON] = keyState[VK_LBUTTON];
	prevKeyState[VK_RBUTTON] = keyState[VK_RBUTTON];
	prevKeyState[VK_MBUTTON] = keyState[VK_MBUTTON];

	//Store new key states
	keyState[VK_LBUTTON] = _buttonState & 0x0001;
	keyState[VK_RBUTTON] = _buttonState & 0x0002;
	keyState[VK_MBUTTON] = _buttonState & 0x0010;

	//Check left button for mouse down event
	if (keyState[VK_LBUTTON]) {
		Dispatch(InputEvent(InputType::MOUSE_BUTTON, false, false, VK_LBUTTON));
	}
	//Check right button for mouse down event
	if (keyState[VK_RBUTTON]) {
		Dispatch(InputEvent(InputType::MOUSE_BUTTON, false, false, VK_RBUTTON));
	}
	//Check middle button for mouse down event
	if (keyState[VK_MBUTTON]) {
		Dispatch(InputEvent(InputType::MOUSE_BUTTON, false, false, VK_MBUTTON));
	}
}

void InputManager::_OnMouseMove(int _prevX, int _prevY, int _x, int _y) {
	//Store previous mouse position
	prevMousePos[0] = _prevX;
	prevMousePos[1] = _prevY;

	//Store new mouse position
	mousePos[0] = _x;
	mousePos[1] = _y;

	//Dispatch mouse move event
	Dispatch(InputEvent(InputType::MOUSE_MOVE, prevMousePos[0], prevMousePos[1], mousePos[0], mousePos[1]));
}

void InputManager::_OnMouseWheel(float _delta) {
	//Add the delta to the total
	totalScrollDelta += _delta;
	//Dispatch a mouse scroll event
	Dispatch(InputEvent(InputType::MOUSE_SCROLL, _delta));
}

InputManager::InputManager() {
	//Set unique identifier for InputListeners to 0
	ilCount = 0;
	//Loop through all keybinds and add an entry in the keyState to track each of them
	map<string, vector<int>>::iterator kbIt;
	for (kbIt = keybinds.begin(); kbIt != keybinds.end(); ++kbIt) {
		//Get the keys bound to this action
		vector<int> thisActionKeys = kbIt->second;
		//Loop through the bound keys
		for (vector<int>::iterator i = thisActionKeys.begin(); i != thisActionKeys.end(); i++) {
			//Add values to the key state maps.  If the value already exists, it is simply reset instead of duplicated
			prevKeyState[*i] = false;
			keyState[*i] = false;
		}
	}
	mouseLocked = true;
	//Ensure entries for the mouse buttons exist.  Note: They could have already been set in the keybinds file, but we want to make sure
	//the mouse input functions have something to check against
	prevKeyState[VK_LBUTTON] = keyState[VK_LBUTTON] = false;
	prevKeyState[VK_RBUTTON] = keyState[VK_RBUTTON] = false;
	prevKeyState[VK_MBUTTON] = keyState[VK_MBUTTON] = false;
	//Two extra mouse buttons for fancy MMO mice
	prevKeyState[VK_XBUTTON1] = keyState[VK_XBUTTON1] = false;
	prevKeyState[VK_XBUTTON2] = keyState[VK_XBUTTON2] = false;
}

InputManager::~InputManager() {
	Release();
}

void InputManager::Release() {
	//Reset input listener unique ID values
	ilCount = 0;
	//Clear the map so the singleton can be reused.
	inputListenerUIDMap.clear();
	//Clear the key press state
	prevKeyState.clear();
	keyState.clear();
}

void InputManager::Dispatch(InputEvent _event) {
	//Loop through every input listener
	map<UINT, InputListener*>::iterator ilIt;
	for (ilIt = inputListenerUIDMap.begin(); ilIt != inputListenerUIDMap.end();) {
		if (ilIt->second == nullptr) {
			inputListenerUIDMap.erase(ilIt++);
			continue;
		}
		//Dispatch this input event to the listener
		ilIt->second->Input(_event);
		ilIt++;
	}
}
