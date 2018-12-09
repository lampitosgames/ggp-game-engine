#ifndef GGP_SCENE_H
#define GGP_SCENE_H

#include <string>
#include "GameObject.h"
class Camera;

class Scene : public GameObject {
protected:
	Camera * activeCamera;
	float totalTime;
public:
	Scene(std::string _uniqueID);

	//Extra init method that runs before start. Useful to create objects and set up the scene
	virtual void Init();
	virtual void Start();
	virtual void Update(float _deltaTime);

	//Resize event
	void OnResize(float width, float height);
};

#endif //GGP_SCENE_H