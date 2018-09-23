#ifndef GGP_INPUT_LISTENER_H
#define GGP_INPUT_LISTENER_H

#include "Component.h"
#include "InputEvent.h"
class GameObject;

class InputListener : public Component {
public:
	InputListener(UINT _uniqueID, GameObject* _gameObject);
	~InputListener();

	void Input(InputEvent _event);
};

#endif //GGP_INPUT_LISTENER_H