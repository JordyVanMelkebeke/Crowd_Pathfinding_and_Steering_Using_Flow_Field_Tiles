/*=============================================================================*/
// Copyright 2021-2022 Elite Engine
// Authors: Matthieu Delaere, Thomas Goussaert
/*=============================================================================*/
// SteeringBehaviors.h: SteeringBehaviors interface and different implementations
/*=============================================================================*/
#ifndef ELITE_STEERINGBEHAVIORS
#define ELITE_STEERINGBEHAVIORS

//-----------------------------------------------------------------
// Includes & Forward Declarations
//-----------------------------------------------------------------
#include "../SteeringHelpers.h"
class SteeringAgent;
class Obstacle;

#pragma region **ISTEERINGBEHAVIOR** (BASE)
class ISteeringBehavior
{
public:
	ISteeringBehavior() = default;
	virtual ~ISteeringBehavior() = default;

	virtual SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) = 0;

	//Seek Functions
	void SetTarget(const TargetData& target) { m_Target = target; }

	template<class T, typename std::enable_if<std::is_base_of<ISteeringBehavior, T>::value>::type* = nullptr>
	T* As()
	{ return static_cast<T*>(this); }

protected:
	TargetData m_Target;
};
#pragma endregion

///////////////////////////////////////
//SEEK
//****
class Seek : public ISteeringBehavior
{
public:
	Seek() = default;
	virtual ~Seek() = default;

	//Seek Behaviour
	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;
};

/////////////////////////
//FLEE
//****
class Flee : public Seek
{
public:
	Flee() = default;
	virtual ~Flee() = default;

	//flee Behavior
	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;
	

	void SetFleeRadius(float fleeRadius) { m_FleeRadius = fleeRadius; }
	float GetFleeRadius() { return m_FleeRadius; }

private:
	float m_FleeRadius = 20.f;
};


//Arrive
class Arrive : public Seek
{
public:
	Arrive() = default;
	virtual ~Arrive() = default;

	//Arrive Behavior
	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;

};

class Face : public ISteeringBehavior
{
public:
	Face() = default;
	virtual ~Face() = default;

	//Face Behavior
	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;

};

class Wander : public Seek
{
public:
	Wander() = default;
	virtual ~Wander() = default;

	//Wander Behavior
	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;

	void SetWanderOffset(float offset) { m_OffsetDistance = offset; }
	void SetWanderRadius(float radius) { m_Radius = radius; }
	void SetMaxAngleChange(float rad) { m_MaxAngleChange = rad; }
	void SetVectorRot(Elite::Vector2& vector, float angle);

protected:
	float m_OffsetDistance = 6.f; //offset (agent Direction)
	float m_Radius = 4.f; //wanderRadius
	float m_MaxAngleChange =  Elite::ToRadians(30.f); //Max WanderAngle change per frame
	float m_WanderAngle = 0.f; //Internal
};

class Persuit : public Seek
{
public:
	Persuit() = default;
	virtual ~Persuit() = default;

	//Persue Behavior
	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;

};

class Evade : public Flee
{
public:
	Evade() = default;
	virtual ~Evade() = default;

	//Evade behavior
	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;

};
#endif


