#include "BoardGamePluginPCH.h"
#include "BrightWarriorControllerComponent.h"
#include "BrightWarriorEntity.h"

V_IMPLEMENT_SERIAL(BG_BrightWarriorControllerComponent, BG_ControllerComponent, 0, &g_BoardGamePluginModule);
START_VAR_TABLE(BG_BrightWarriorControllerComponent, BG_ControllerComponent, "Bright warrior controller", 0, "")
END_VAR_TABLE

BG_BrightWarriorControllerComponent::BG_BrightWarriorControllerComponent()
	: BG_ControllerComponent()
{
}

void BG_BrightWarriorControllerComponent::OnTick(float deltaTime)
{
	BG_ControllerComponent::OnTick(deltaTime);
}

void BG_BrightWarriorControllerComponent::SetOwner(VisTypedEngineObject_cl *newOwner)
{
	BG_ControllerComponent::SetOwner(newOwner);

	if(newOwner)
	{
		//create states
		m_states[BG_ControllerStateId::kNotControlled] = new BG_ControllerState::NotControlled();
		m_states[BG_ControllerStateId::kMoving] = new BG_BrightWarriorControllerState::Moving();
		m_states[BG_ControllerStateId::kIdling] = new BG_BrightWarriorControllerState::Idling();
	}
}

void BG_BrightWarriorControllerState::Idling::OnTick(BG_ControllerComponent *controller, float deltaTime)
{
	BG_ControllerStateBase::OnTick(controller, deltaTime);

	BG_BrightWarriorControllerComponent *warriorController = vstatic_cast<BG_BrightWarriorControllerComponent*>(controller);
	BG_BrightWarriorEntity *warrior = static_cast<BG_BrightWarriorEntity*>(controller->GetOwner());

	hkvVec3 targetPoint = controller->GetTargetPoint();
	//get distance from warrior to target point
	hkvVec3 warriorToTargetPointVector(warrior->GetPosition() - targetPoint);
	//use getLengthSquared when comparing relative lengths, since the computation of the squared length does not require a sqrt
	targetPoint.z = 0.0f;
	warriorToTargetPointVector.z = 0.0f;

	float const  warriorToTargetPointDistanceSqr = warriorToTargetPointVector.getLengthSquared();
	float const pathGoalReachedToleranceSqr = controller->GetPathGoalReachedTolerance() * controller->GetPathGoalReachedTolerance();

	if(warriorToTargetPointDistanceSqr > pathGoalReachedToleranceSqr)
	{
		controller->SetState(BG_ControllerStateId::kMoving);
		return;
	}
}

void BG_BrightWarriorControllerState::Moving::OnTick(BG_ControllerComponent *controller, float deltaTime)
{
	BG_ControllerStateBase::OnTick(controller, deltaTime);

	BG_BrightWarriorControllerComponent *warriorController = vstatic_cast<BG_BrightWarriorControllerComponent*>(controller);
	BG_BrightWarriorEntity *warrior = static_cast<BG_BrightWarriorEntity*>(controller->GetWarriorEntity());
	//TODO: ubaci kod za napad, sada ima samo za kretanje

	hkvVec3 const targetPoint = controller->GetTargetPoint();
	hkvVec3 const warriorToTargetPointVector(warrior->GetPosition() - targetPoint);
	//use getLengthSquared when comparing relative lengths, since the computation of the squared length does not require a sqrt
	float const  warriorToTargetPointDistanceSqr = warriorToTargetPointVector.getLengthSquared();
	float const pathGoalReachedToleranceSqr = controller->GetPathGoalReachedTolerance() * controller->GetPathGoalReachedTolerance();

	if(warriorToTargetPointDistanceSqr <= pathGoalReachedToleranceSqr)
	{
		controller->SetState(BG_ControllerStateId::kIdling);
		//since warrior reached target point reset it to warrior current position
		controller->SetTargetPoint(warrior->GetPosition());
		return;
	}
	else
	{
		controller->RequestPath(targetPoint);
	}

	//TODO: prepravi ovo da se okrece samo prema "levo" ili "desno" u zavisnosti od poteza
	hkvVec3 direction;
	BG_ControllerHelper::CalcDirection(direction, warrior->GetDirection(), controller->GetDirection(), 0.25);
	controller->GetWarriorEntity()->SetDirection(direction);
}