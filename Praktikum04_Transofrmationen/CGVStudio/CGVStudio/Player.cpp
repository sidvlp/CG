#include "Player.h"
#define _USE_MATH_DEFINES
#include "math.h"
#include "../../src/ConstantShader.h"
#include "../../src/TriangleBoxModel.h"

Player::Player(): fb(0,0,0)
{
	pBoundingBoxModel = new LineBoxModel(1, 1, 1);
	playerFootBoundingBox = new LineBoxModel(1, 1, 1);
	isFalling = false;
	isGrounded = true;
}

Player::~Player()
{
	delete pBoundingBoxModel;
	delete playerFootBoundingBox;
}

bool Player::loadModels(const char* player_model)
{
	if (player_model != nullptr) {
		player = new Model(player_model);
		player->shader(pShader);
		Matrix playerRotX, playerRotY, playerMove, playerUrsprung;
		playerRotX.rotationX(toRadian(-90));
		playerRotY.rotationY(toRadian(180));
		playerMove.translation(0, 0.5, 7.5);
		playerUrsprung = this->player->transform();
		playerUrsprung.scale(0.02f);
		player->transform(playerMove * playerUrsprung * playerRotY * playerRotX);
		return true;
	}
	return false;
}

float Player::toRadian(float degrees) {

	return (float)degrees * M_PI / 180;

}

void Player::resetPosition()
{
	Matrix playerRotX, playerRotY, playerMove, playerUrsprung;
	playerRotX.rotationX(toRadian(-90));
	playerRotY.rotationY(toRadian(180));
	playerMove.translation(0, 0.5, 7.5);
	playerUrsprung = this->player->transform();
	player->transform(playerMove * playerUrsprung * playerRotY * playerRotX);
}

void Player::drawBoundingBox(const BaseCamera& Cam) {
	if (!pBoundingBoxModel) return;
	ConstantShader* pConstShader;
	pConstShader = new ConstantShader();
	pConstShader->color(Color(1, 1, 1));

	pBoundingBoxModel->shader(pConstShader, true);
	pBoundingBoxModel->draw(Cam);
}

void Player::updateBoundingBox()
{
	AABB localBox = player->boundingBox();
	Matrix transform = player->transform();

	Vector corners[8] = {
		Vector(localBox.Min.X, localBox.Min.Y, localBox.Min.Z),
		Vector(localBox.Min.X, localBox.Min.Y, localBox.Max.Z),
		Vector(localBox.Min.X, localBox.Max.Y, localBox.Min.Z),
		Vector(localBox.Min.X, localBox.Max.Y, localBox.Max.Z),
		Vector(localBox.Max.X, localBox.Min.Y, localBox.Min.Z),
		Vector(localBox.Max.X, localBox.Min.Y, localBox.Max.Z),
		Vector(localBox.Max.X, localBox.Max.Y, localBox.Min.Z),
		Vector(localBox.Max.X, localBox.Max.Y, localBox.Max.Z)
	};


	for (int i = 0; i < 8; ++i)
		corners[i] = transform * corners[i];

	Vector newMin = corners[0];
	Vector newMax = corners[0];

	for (int i = 1; i < 8; ++i) {
		newMin.X = std::min(newMin.X, corners[i].X);
		newMin.Y = std::min(newMin.Y, corners[i].Y);
		newMin.Z = std::min(newMin.Z, corners[i].Z);

		newMax.X = std::max(newMax.X, corners[i].X);
		newMax.Y = std::max(newMax.Y, corners[i].Y);
		newMax.Z = std::max(newMax.Z, corners[i].Z);
	}

	Vector bSize = newMax - newMin;
	Matrix scaleMat;
	scaleMat.scale(bSize.X, 0.1, bSize.Z);

	Vector bcenter = (newMin + newMax) * 0.5f;
	Matrix transMat;
	transMat.translation(bcenter.X, bcenter.Y / 2, bcenter.Z);
	pBoundingBoxModel->transform(transMat * scaleMat);

	BoundingBox.Min = Vector(newMin.X, newMin.Y, newMin.Z);
	BoundingBox.Max = Vector(newMax.X, newMax.Y, newMax.Z);
	//std::cout << "BoundingBox Min: " << newMin.X << ", " << newMin.Y << ", " << newMin.Z << std::endl;
//std::cout << "BoundingBox Max: " << newMax.X << ", " << newMax.Y << ", " << newMax.Z << std::endl;
//std::cout << "---- " << std::endl;
}



AABB& Player::getBoundingBox()
{
	return BoundingBox;
}



void Player::steer(float ForwardBackward, float LeftRight)
{
	fb.X = ForwardBackward;
	fb.Y = LeftRight;
	//std::cout << "fb.X" << fb.X << std::endl;
	//std::cout << "fb.Y" << fb.Y << std::endl;
}

void Player::update(float dtime, Camera& cam)
{
	Matrix playerMove, playerRot, playerUrsprung;

	playerUrsprung = this->player->transform();

	if (isGrounded) {
		if (fb.X != 0) {
			playerRot.rotationZ(fb.Y * dtime * 5);
			playerMove.translation(0, -fb.X * dtime * 150, 0);
			this->player->transform(playerUrsprung * playerMove * playerRot);
		}
		else {
			playerRot.rotationZ(fb.Y * dtime * 5);
			this->player->transform(playerUrsprung * playerRot);
		}
	}



	updateBoundingBox();

	if (isFalling) {

		std::cout << "Falling" << std::endl;
		//// Fallgeschwindigkeit erhöhen
		//fallVelocity += GRAVITY * dtime;

		//// Position anpassen
		//Matrix trans = player->transform();
		//trans.translation(0, fallVelocity * dtime, 0);
		//player->transform(trans);

		//// Respawn wenn unter bestimmter Höhe
		//if (BoundingBox.Min.Y < -10.0f) {
		////	resetPosition();
		//	isFalling = false;
		//	fallVelocity = 0.0f;
		//}
	}

	cam.setPosition(playerUrsprung.translation() + Vector(0, 5, 5) - player->transform().forward() * 50);
	cam.setTarget(playerUrsprung.translation());
}

void Player::draw(const BaseCamera& Cam)
{
	player->draw(Cam);
	drawBoundingBox(Cam);
	
}

bool Player::checkGroundCollision(const AABB& a, const AABB& b) {
	bool groundCollision = (a.Min.X <= b.Max.X && a.Max.X >= b.Min.X) &&
		(a.Min.Y <= b.Max.Y && a.Max.Y >= b.Min.Y) &&
		(a.Min.Z <= b.Max.Z && a.Max.Z >= b.Min.Z);
	if (groundCollision) {
		/*	isGrounded = true;
			isFalling = false;
			fallVelocity = 0.0f;*/
			return true;
		
	}
	/*else {
		isGrounded = false;
		isFalling = true;
		fallVelocity = 0.0f;
	}*/


	return false;
}



//AABB localBox = player->boundingBox();
//Matrix transform = player->transform();
//
//Vector corners[8] = {
//	Vector(localBox.Min.X, localBox.Min.Y, localBox.Min.Z),
//	Vector(localBox.Min.X, localBox.Min.Y, localBox.Max.Z),
//	Vector(localBox.Min.X, localBox.Max.Y, localBox.Min.Z),
//	Vector(localBox.Min.X, localBox.Max.Y, localBox.Max.Z),
//	Vector(localBox.Max.X, localBox.Min.Y, localBox.Min.Z),
//	Vector(localBox.Max.X, localBox.Min.Y, localBox.Max.Z),
//	Vector(localBox.Max.X, localBox.Max.Y, localBox.Min.Z),
//	Vector(localBox.Max.X, localBox.Max.Y, localBox.Max.Z)
//};
//
//
//for (int i = 0; i < 8; ++i)
//	corners[i] = transform * corners[i];
//
//Vector newMin = corners[0];
//Vector newMax = corners[0];
//
//for (int i = 1; i < 8; ++i) {
//	newMin.X = std::min(newMin.X, corners[i].X);
//	newMin.Y = std::min(newMin.Y, corners[i].Y);
//	newMin.Z = std::min(newMin.Z, corners[i].Z);
//
//	newMax.X = std::max(newMax.X, corners[i].X);
//	newMax.Y = std::max(newMax.Y, corners[i].Y);
//	newMax.Z = std::max(newMax.Z, corners[i].Z);
//}
//
//Vector bSize = newMax - newMin;
//Matrix scaleMat;
//scaleMat.scale(bSize.X, bSize.Y, bSize.Z);
//
//Vector bcenter = (newMin + newMax) * 0.5f;
//Matrix transMat;
//transMat.translation(bcenter.X, bcenter.Y, bcenter.Z);
//pBoundingBoxModel->transform(transMat * scaleMat);