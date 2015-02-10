#include "BoardGamePluginPCH.h"
#include "DarkWarriorControllerComponent.h"
#include "DarkWarriorEntity.h"
#include "GameManager.h"

V_IMPLEMENT_SERIAL(BG_DarkWarriorControllerComponent, BG_ControllerComponent, 0, &g_BoardGamePluginModule);
START_VAR_TABLE(BG_DarkWarriorControllerComponent, BG_ControllerComponent, "Dark warrior controller", 0, "")
END_VAR_TABLE

BG_DarkWarriorControllerComponent::BG_DarkWarriorControllerComponent()
	: BG_ControllerComponent()
{
}

void BG_DarkWarriorControllerComponent::OnTick(float deltaTime)
{
	BG_ControllerComponent::OnTick(deltaTime);
}

void BG_DarkWarriorControllerComponent::SetOwner(VisTypedEngineObject_cl *newOwner)
{
	BG_ControllerComponent::SetOwner(newOwner);

	if(newOwner)
	{
		//create states
		m_states[BG_ControllerStateId::kMoving] = new BG_DarkWarriorControllerState::Moving();
		m_states[BG_ControllerStateId::kIdling] = new BG_DarkWarriorControllerState::Idling();
		m_states[BG_ControllerStateId::kMeleeAttacking] = new BG_DarkWarriorControllerState::MeleeAttacking();
	}
}

void BG_DarkWarriorControllerState::Idling::OnEnterState(BG_ControllerComponent *const controller)
{
	GameManager::GlobalManager().SetPlayingTheMoveEnd(true);
}

void BG_DarkWarriorControllerState::Idling::OnTick(BG_ControllerComponent *const controller, float deltaTime)
{
	BG_ControllerStateBase::OnTick(controller, deltaTime);

	BG_DarkWarriorControllerComponent *warriorController = vstatic_cast<BG_DarkWarriorControllerComponent*>(controller);
	BG_DarkWarriorEntity *warrior = static_cast<BG_DarkWarriorEntity*>(controller->GetOwner());

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

void BG_DarkWarriorControllerState::Moving::OnTick(BG_ControllerComponent *const controller, float deltaTime)
{
	BG_ControllerStateBase::OnTick(controller, deltaTime);

	BG_DarkWarriorControllerComponent *warriorController = vstatic_cast<BG_DarkWarriorControllerComponent*>(controller);
	BG_DarkWarriorEntity *warrior = static_cast<BG_DarkWarriorEntity*>(controller->GetWarriorEntity());
	BG_WarriorEntity *target = warriorController->GetTarget();
	hkvVec3 targetPoint;

	//KILL 'EM ALL!
	if(target)
	{
		hkvVec3 targetToWarriorProjectedDirection;
		float targetToWarriorProjectedDistance;
		BG_ControllerHelper::GetProjectedDirAndDistFromTarget(warrior, target, targetToWarriorProjectedDirection, targetToWarriorProjectedDistance);

		float const minDistanceToAttack = BG_ControllerHelper::GetMinDistanceToAttack(warrior, target);

		if(targetToWarriorProjectedDistance < minDistanceToAttack)
		{
			controller->GetWarriorEntity()->RaiseAnimationEvent(BG_WarriorAnimationEvent::kMeleeAttack);
			controller->SetState(BG_ControllerStateId::kMeleeAttacking);
			return;
		}
		else
		{
			targetPoint = target->GetPosition();
		}
	}
	else
	{
		targetPoint = warriorController->GetTargetPoint();
	}

	hkvVec3 const warriorToTargetPointVector(warrior->GetPosition() - targetPoint);
	//use getLengthSquared when comparing relative lengths, since the computation of the squared length does not require a sqrt
	float const  warriorToTargetPointDistanceSqr = warriorToTargetPointVector.getLengthSquared();
	float const pathGoalReachedToleranceSqr = controller->GetPathGoalReachedTolerance() * controller->GetPathGoalReachedTolerance();

	if(warriorToTargetPointDistanceSqr <= pathGoalReachedToleranceSqr)
	{
		controller->SetState(BG_ControllerStateId::kIdling);
		//since warrior reached target point reset it to warrior current position
		controller->SetTargetPoint(warrior->GetPosition());
		warrior->SetDirection(controller->GetFinalDirection());
		return;
	}
	else
	{
		controller->RequestPath(targetPoint);
	}

	hkvVec3 direction;
	BG_ControllerHelper::CalcDirection(direction, warrior->GetDirection(), controller->GetDirection(), 0.25);
	controller->GetWarriorEntity()->SetDirection(direction);
}


void BG_DarkWarriorControllerState::MeleeAttacking::OnEnterState(BG_ControllerComponent *const controller)
{
	BG_ControllerStateBase::OnEnterState(controller);
}

void BG_DarkWarriorControllerState::MeleeAttacking::OnProcessAnimationEvent(BG_ControllerComponent *const controller, hkbEvent const& animationEvent)
{
	BG_ControllerStateBase::OnProcessAnimationEvent(controller, animationEvent);

	BG_WarriorEntity *warrior = controller->GetWarriorEntity();

	if(warrior->GetIdForAnimationEvent(BG_WarriorAnimationEvent::kMeleeAttackEnd) == animationEvent.getId())
	{
		controller->SetState(BG_ControllerStateId::kMoving);
	}
	else if(warrior->GetIdForAnimationEvent(BG_WarriorAnimationEvent::kMeleeAttack) == animationEvent.getId())
	{
		controller->GetTarget()->Die();
	}
}

void BG_DarkWarriorControllerState::MeleeAttacking::OnTick(BG_ControllerComponent *controller, float deltaTime)
{
	BG_ControllerStateBase::OnTick(controller, deltaTime);

	BG_WarriorEntity const *const target = controller->GetTarget();

	if(target)
	{
		BG_ControllerHelper::FaceTowards(controller, target->GetPosition(), deltaTime);
	}
}