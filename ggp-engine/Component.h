#ifndef GGP_ABSTRACT_COMPONENT_H
#define GGP_ABSTRACT_COMPONENT_H

#include "ComponentTypes.h"
class GameObject;

typedef unsigned int UINT;

//Abstract base class for all components.
class Component {
protected:
	CompType type;
public:
	GameObject* gameObject;
	UINT uniqueID;
};

#endif //GGP_ABSTRACT_COMPONENT_H