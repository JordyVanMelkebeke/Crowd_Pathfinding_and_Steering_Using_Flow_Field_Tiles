#include "stdafx.h"
#include "FlockingSteeringBehaviors.h"
#include "TheFlock.h"
#include "../SteeringAgent.h"
#include "../SteeringHelpers.h"

//*******************
//COHESION (FLOCKING)
SteeringOutput Cohesion::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering = {};

	if (m_pFlock->GetNeighbors().empty())
	{
		return steering;
	}

	

	steering.LinearVelocity = (m_pFlock->GetAverageNeighborPos() - pAgent->GetPosition());
	steering.LinearVelocity.Normalize();

	return steering;
	
}


//*********************
//SEPARATION (FLOCKING)
SteeringOutput Separation::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering = {};
	Elite::Vector2 totalVec{};

	if (m_pFlock->GetNeighbors().empty())
	{
		return SteeringOutput{};
	}


	for (int i = 0; i < m_pFlock->GetNrOfNeighbors(); i++)
	{
		float distance{ Elite::Distance(pAgent->GetPosition(),m_pFlock->GetNeighbors()[i]->GetPosition()) };
		float invertedDistance{ pAgent->GetRadius() - distance };
		Elite::Vector2 vector{};
		vector = pAgent->GetPosition() - m_pFlock->GetNeighbors()[i]->GetPosition();
		vector.Normalize();
		vector *= invertedDistance;
		totalVec += vector;
	}



	steering.LinearVelocity = totalVec;
	steering.LinearVelocity *= -1;
	steering.LinearVelocity *= pAgent->GetMaxLinearSpeed();
	return steering;

}

//*************************
//VELOCITY MATCH (FLOCKING)
SteeringOutput VelocityMatch::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering = {};

	if (m_pFlock->GetNeighbors().empty())
	{
		return SteeringOutput{};
	}

	//Elite::Vector2 avarageVel{ m_pFlock->GetAverageNeighborVelocity() };

	
	steering.LinearVelocity = m_pFlock->GetAverageNeighborVelocity();


	return steering;
}