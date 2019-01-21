#include "stdafx.h"

#include "BaseComponent.h"

class IComponent;
namespace Util {
	TypeID FamilyTypeID<IComponent>::s_count = 0u;

	template class FamilyTypeID<IComponent>;
}