#pragma once



#include <stdio.h>
#include "../../src/Model.h"

class Player : public Model
{
public:
	Player();
	virtual ~Player();
	bool loadModels(const char* player_model);
	void steer(float ForwardBackward, float LeftRight);
	void update(float dtime, Camera& cam);
	virtual void draw(const BaseCamera& Cam);
	float toRadian(float degrees);
	void resetPosition();
protected:
	Model* player;
	Vector direction;
	Vector fb;
};

