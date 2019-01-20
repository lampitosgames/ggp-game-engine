#ifndef GGP_INPUT_LISTENER_H
#define GGP_INPUT_LISTENER_H

#include "BaseComponent.h"
#include "InputEvent.h"
class GameObject;

class InputListener : public ECS::BaseComponent<InputListener> {
public:
	InputListener(GameObject* _gameObject);
	~InputListener();

	void Input(InputEvent _event);

	GameObject* gameObject;
};

#endif //GGP_INPUT_LISTENER_H