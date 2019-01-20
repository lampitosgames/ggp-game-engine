#include "stdafx.h"

#include "InputManager.h"
#include "InputListener.h"
#include "GameObject.h"

InputListener::InputListener(GameObject* _gameObject) {
	gameObject = _gameObject;
	owner = _gameObject->GetUniqueID();
	InputManager::GetInstance()->AddInputListener(this);
}

InputListener::~InputListener() {
	InputManager::GetInstance()->RemoveInputListener(this);
}

void InputListener::Input(InputEvent _event) {
	gameObject->Input(_event);
}
