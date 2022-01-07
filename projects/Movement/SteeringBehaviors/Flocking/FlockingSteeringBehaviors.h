#pragma once
#include "projects/Movement/SteeringBehaviors/Steering/SteeringBehaviors.h"
class Flock;

//COHESION - FLOCKING
//*******************
class Cohesion : public Seek
{
public:
	Cohesion(Flock* pFlock) :m_pFlock(pFlock) {};

	//Cohesion Behavior
	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;

private:
	Flock* m_pFlock = nullptr;
};


//SEPARATION - FLOCKING
//*********************
class Separation : public ISteeringBehavior
{
public:
	Separation(Flock* pFlock) :m_pFlock(pFlock) {};

	//Separation behavior
	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;

private:
	Flock* m_pFlock = nullptr;
};

//VELOCITY MATCH - FLOCKING
//************************
class VelocityMatch : public ISteeringBehavior
{
public:
	VelocityMatch(Flock* pFlock) :m_pFlock(pFlock) {};

	//Velocity match behavior
	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;
private:
	Flock* m_pFlock = nullptr;
};