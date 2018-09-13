#ifndef GGP_COMPONENT_H
#define GPP_COMPONENT_H

#include "ComponentTypes.h"
class GameObject;

class Component {
protected:
	CompType type;
public:
	GameObject* gameObject;
	UINT uniqueID;
};

#endif //GGP_COMPONENT_H