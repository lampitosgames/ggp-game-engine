#pragma once

using ComponentID = size_t;

class GameObject;

class IComponent
{
public:

    IComponent();

    virtual ~IComponent();


    inline const bool operator==( const IComponent& other ) const { return id == other.id; }
    inline const bool operator!=( const IComponent& other ) const { return id == other.id; }


    /////////////////////////////////////////////////    
    // Accessors 

    inline const ComponentID GetComponentId() const { return this->id; }

    inline const bool IsEnabled() const { return this->isEnabled; }

    inline void SetSenabled( bool aEnabledState ) { this->isEnabled = aEnabledState; }

    inline const GameObject* GetOwner() const { return this->owner; }

protected:
    ComponentID id;

    bool isEnabled;

    GameObject* owner;

};
