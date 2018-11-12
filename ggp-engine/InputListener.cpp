#include "stdafx.h"

#include "InputListener.h"
#include "GameObject.h"

InputListener::InputListener(UINT _uniqueID, GameObject* _gameObject) {
	type = CompType::INPUT_LISTENER;
	uniqueID = _uniqueID;
	gameObject = _gameObject;
}

InputListener::~InputListener() {}

void InputListener::Input(InputEvent _event) {
	gameObject->Input(_event);
}
