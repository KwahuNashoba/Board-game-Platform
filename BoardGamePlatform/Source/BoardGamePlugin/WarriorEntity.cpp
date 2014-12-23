#include "BoardGamePluginPCH.h"
#include "GameManager.h"
#include "ControllerComponent.h"
#include "Serializer.h"
#include "SoundHelper.h"
#include "WarriorEntity.h"

#include <Vision\Runtime\EnginePlugins\Havok\HavokPhysicsEnginePlugin\vHavokCharacterController.hpp>
#include <Vision\Runtime\EnginePlugins\Havok\HavokBehaviorEnginePlugin\vHavokBehaviorComponent.hpp>

#include <Behavior/Behavior/Event/hkbEventQueue.h>
#include <Behavior/Behavior/World/hkbWorld.h>


V_IMPLEMENT_SERIAL(BG_WarriorEntity, VisBaseEntity_cl, 0, &g_BoardGamePluginModule);
START_VAR_TABLE(BG_WarriorEntity, VisBaseEntity_cl, "Base warrior entity", 0, "")	
	DEFINE_VAR_FLOAT_AND_NAME(BG_WarriorEntity, m_collisionRadius, "Collision Radius", "Collision shape radius", "100", 0, 0);
	DEFINE_VAR_FLOAT_AND_NAME(BG_WarriorEntity, m_collisionHeight, "Collision Height", "Collision shape height", "200", 0, 0);
	// entity properties
	DEFINE_VAR_FLOAT_AND_NAME(BG_WarriorEntity, m_eyeHeight, "Eye Height", "Height of this character's eyes", "150", 0, 0);
	DEFINE_VAR_FLOAT_AND_NAME(BG_WarriorEntity, m_sensorSize, "Ai SensorSize", "Character Sensor Size", "128", 0, 0);
	DEFINE_VAR_FLOAT_AND_NAME(BG_WarriorEntity, m_desiredSpeed, "Ai DesiredSpeed", "Character Desired Speed", "100", 0, 0);
END_VAR_TABLE

BG_WarriorEntity::BG_WarriorEntity()
	: m_collisionRadius(20.f),
	m_collisionHeight(100.f),
	m_eyeHeight(150.0f),
	m_sensorSize(128.0f),
	m_desiredSpeed(100.0f),
	m_dead(false),
	m_dying(false),
	m_timeOfDeath(0.0f)
{
	for(int i = 0; i<BG_WarriorAnimationEvent::kAnimationEventCount; i++)
	{
		m_animationEventIds[i] = -1;
	}
}

void BG_WarriorEntity::InitFunction()
{
}

void BG_WarriorEntity::InitializeProperties()
{
}

void BG_WarriorEntity::PostInitialize()
{
	if(!Vision::Editor.IsPlaying())
	{
		return;
	}

	VASSERT(m_havokBehavior->m_character);
	if(!m_havokBehavior->m_character)
	{
		m_havokBehavior->InitVisionCharacter(this);
	}

	InitAnimationEventIds();
	InitAnimationVariableIds();
	UpdateBehaviorWorldFromModel();

	//AI controller init
	if(GetControllerComponentType())
	{
		//retrieve controller type and create controller of that type
		m_controller = static_cast<BG_ControllerComponent*>(GetControllerComponentType()->CreateInstance());

		m_controller->SetSensorSize(m_sensorSize);
		m_controller->SetDesiredSpeed(m_desiredSpeed);
		m_controller->SetTargetPoint(GetPosition());

		AddComponent(m_controller);
	}
	else
	{
		//if GetControllerComponentType returns NULL it means that entity already has controller component on it so just retrive it
		m_controller = static_cast<BG_ControllerComponent*>(Components().GetComponentOfBaseType(V_RUNTIME_CLASS(BG_ControllerComponent)));
	}

	VASSERT_MSG(m_controller, "This character requires an AI controller component. Please put an Dark warrior controller or Bright warrior controller on it.");

	//Initialize Havok Character Proxy
	m_characterController = new vHavokCharacterController();
	m_characterController->Capsule_Radius = m_collisionRadius;
	m_characterController->Character_Bottom = hkvVec3(0.0f, 0.0f, m_collisionRadius);
	m_characterController->Character_Top = hkvVec3(0.0f, 0.0f, m_collisionHeight - m_collisionRadius);
	m_characterController->Gravity_Scale = 1.0f;
	m_characterController->SetPosition(GetPosition());
	AddComponent(m_characterController);
	//m_characterController->SetDebugRendering(true);
	
	//Used to put warrior back on board after deserialization
	GameManager::GlobalManager().AddWarrior(this, (int)GetPosition().x/BG_WARRIOR_MODEL_WIDTH, (int)GetPosition().y/BG_WARRIOR_MODEL_WIDTH);

	//TODO: create ambient character effects
}

//Called to shut down the character
void BG_WarriorEntity::DisposeObject()
{
	m_characterController = NULL;
	m_havokBehavior = NULL;
	m_controller = NULL;

	VisBaseEntity_cl::DisposeObject();
}

void BG_WarriorEntity::ThinkFunction()
{
	const float deltaTime = Vision::GetTimer()->GetTimeDifference();

	// decompose it if it is dead
	if(IsDead())
	{
		float timeDif = Vision::GetTimer()->GetTime() - m_timeOfDeath;
		if(timeDif > 4)
			DisposeObject();
		return;
	}

	OnTick(deltaTime);
	UpdateBehaviorWorldFromModel();
}

void BG_WarriorEntity::OnTick(float deltaTime)
{
	if(!IsDead())
	{
		ProcessAnimationEvents();
	}

	if(m_controller)
	{
		m_controller->OnTick(deltaTime);
		SetAnimationVariable(BG_WarriorAnimationVariable::kMoveSpeed, m_controller->GetSpeed());
	}
	else if(m_characterController)
	{
		//if there is no m_controller then m_characterController should be removed
		RemoveComponent(m_characterController);
		m_characterController = NULL;
	}
}

void BG_WarriorEntity::Die()
{
	m_dead = true;
	m_timeOfDeath = Vision::GetTimer()->GetTime();

	//TODO: stop all effects
	m_soundHelper->PlayFromPosition(BG_SoundEvent::eDie, GetPosition());
	m_soundHelper->DeInitSoundEffects();
	m_soundHelper = NULL;

	RaiseAnimationEvent(BG_WarriorAnimationEvent::kDie);

	//TODO: obrisi senku

	SetController(NULL);

	if(m_characterController)
	{
		RemoveComponent(m_characterController);
	}

	SetCollisionMesh(NULL);

	//TODO: napravi effect za umiranje
}

bool BG_WarriorEntity::IsDead() const
{
	return m_dead;
}

bool BG_WarriorEntity::IsDying() const
{
	return m_dying;
}

float BG_WarriorEntity::GetCollisionRadius() const
{
	return m_collisionRadius;
}

float BG_WarriorEntity::GetCollisionHeight() const
{
	return m_collisionHeight;
}

vHavokBehaviorComponent *BG_WarriorEntity::GetBehaviorComponent()
{
	return m_havokBehavior;
}

vHavokBehaviorComponent const *BG_WarriorEntity::GetBehaviorComponent() const
{
	return m_havokBehavior;
}

void BG_WarriorEntity::RaiseAnimationEvent(BG_WarriorAnimationEvent::Enum animationEvent)
{
	VASSERT(m_havokBehavior);
	int eventId = m_animationEventIds[animationEvent];
	if(m_havokBehavior && eventId >= 0)
	{
		m_havokBehavior->m_character->getEventQueue()->enqueueWithExternalId(eventId);
	}
}

void BG_WarriorEntity::SetAnimationVariable(BG_WarriorAnimationVariable::Enum const animationVariable, float const value)
{
  VASSERT(m_havokBehavior);
  int const variableId = m_animationVariableIds[animationVariable];
  if(m_havokBehavior && variableId >= 0)
  {
	m_havokBehavior->m_character->getBehavior()->setVariableValueWord(variableId, value);
  }
}

void BG_WarriorEntity::InitAnimationEventIds()
{
	hkStringMap<int> const& eventNameToIdMap = vHavokBehaviorModule::GetInstance()->getBehaviorWorld()->accessEventLinker().m_nameToIdMap;
	m_animationEventIds[BG_WarriorAnimationEvent::kDie] = eventNameToIdMap.getWithDefault("Die", -1);

	m_animationEventIds[BG_WarriorAnimationEvent::kMove] = eventNameToIdMap.getWithDefault("Move", -1);
	m_animationEventIds[BG_WarriorAnimationEvent::kMoveEnd] = eventNameToIdMap.getWithDefault("MoveEnd", -1);

	m_animationEventIds[BG_WarriorAnimationEvent::kMeleeAttack] = eventNameToIdMap.getWithDefault("MeleeAttack", -1);
	m_animationEventIds[BG_WarriorAnimationEvent::kMeleeAttackImpact] = eventNameToIdMap.getWithDefault("MeleeAttackImpact", -1);
	m_animationEventIds[BG_WarriorAnimationEvent::kMeleeAttackEnd] = eventNameToIdMap.getWithDefault("MeleeAttackEnd", -1);

	m_animationEventIds[BG_WarriorAnimationEvent::kFootStepRight] = eventNameToIdMap.getWithDefault("FootstepRight", -1);
	m_animationEventIds[BG_WarriorAnimationEvent::kFootStepLeft] = eventNameToIdMap.getWithDefault("FootstepLeft", -1);
}

void BG_WarriorEntity::InitAnimationVariableIds()
{
	hkStringMap<int> const& eventNameToIdMap = vHavokBehaviorModule::GetInstance()->getBehaviorWorld()->accessVariableLinker().m_nameToIdMap;
	m_animationVariableIds[BG_WarriorAnimationVariable::kMoveSpeed] = eventNameToIdMap.getWithDefault("MoveSpeed", -1);
}

void BG_WarriorEntity::OnHavokAnimationEvent(hkbEvent const& behaviorEvent, bool raisedBySdk)
{
	if(behaviorEvent.getPayload())
	{
		hkReferencedObject::lockAll();
		behaviorEvent.getPayload()->addReference();
		hkReferencedObject::unlockAll();
	}

	m_queuedAnimationEvents.pushBack(behaviorEvent);
}

void BG_WarriorEntity::ProcessAnimationEvents()
{
	hkReferencedObject::lockAll();
	for(hkArray<hkbEvent>::const_iterator i = m_queuedAnimationEvents.begin(), end = m_queuedAnimationEvents.end(); i != end; i++)
	{
		OnProcessAnimationEvent(*i);

		VASSERT(m_controller);
		if(m_controller)
		{
			m_controller->OnProcessAnimationEvent(this, *i);
		}

		hkbEventPayload *const payload = (*i).getPayload();
		if(payload)
		{
			payload->removeReference();
		}
	}
	hkReferencedObject::unlockAll();

	m_queuedAnimationEvents.clear();
}

void BG_WarriorEntity::OnProcessAnimationEvent(hkbEvent const& behaviorEvent)
{
	//play right footstep sound
	if(GetIdForAnimationEvent(BG_WarriorAnimationEvent::kFootStepRight) == behaviorEvent.getId())
	{
		m_soundHelper->PlayFromPosition(BG_SoundEvent::eFootstepRight, GetPosition());
		return;
	}
	//play left footstep sound
	if(GetIdForAnimationEvent(BG_WarriorAnimationEvent::kFootStepLeft) == behaviorEvent.getId())
	{
		m_soundHelper->PlayFromPosition(BG_SoundEvent::eFootStepLeft, GetPosition());
		return;
	}
	//TODO: dodaj obradu za udarac oruzja
}

void BG_WarriorEntity::UpdateBehaviorWorldFromModel()
{
	VASSERT(m_havokBehavior);
	VASSERT(m_havokBehavior->m_character);
	if(m_havokBehavior && m_havokBehavior->m_character)
	{
		hkQsTransform worldFromModel;
		vHavokConversionUtils::VisMatrixToHkQuat(GetRotationMatrix(), worldFromModel.m_rotation);
		worldFromModel.setScale(hkVector4::getConstant<HK_QUADREAL_1>());
		vHavokConversionUtils::VisVecToPhysVecLocal(GetPosition(), worldFromModel.m_translation);

		m_havokBehavior->m_character->setWorldFromModel(worldFromModel);
	}
}

void BG_WarriorEntity::SetController(BG_ControllerComponent *newController)
{
	IVObjectComponent *const controller = Components().GetComponentOfBaseType(V_RUNTIME_CLASS(BG_ControllerComponent));
	if(controller)
	{
		RemoveComponent(controller);
	}

	if(newController)
	{
		AddComponent(newController);
	}

	m_controller = newController;
}

BG_ControllerComponent* BG_WarriorEntity::GetControllerComponent()
{
	return m_controller;
}

BG_ControllerComponent const* BG_WarriorEntity::GetControllerComponent() const
{
	return m_controller;
}

bool BG_WarriorEntity::IsOnNavMesh(float const testRadius) const
{
	return BG_ControllerHelper::IsPointOnNavMesh(GetPosition(), testRadius);
}

void BG_WarriorEntity::SetVisible(bool visible)
{
	unsigned int visibleBitmask;
	
	if(visible)
	{
		visibleBitmask = VIS_ENTITY_VISIBLE;
	}
	else
	{
		visibleBitmask = VIS_ENTITY_INVISIBLE;
	}

	//set character visibility
	SetVisibleBitmask(visibleBitmask);
}

void BG_WarriorEntity::Serialize(VArchive& ar)
{
	//call parent class Serialize function
	VisBaseEntity_cl::Serialize(ar);
	BG_Serializer::Serialize(this, ar);
}

VBool BG_WarriorEntity::WantsDeserializationCallback(const VSerializationContext& context)
{
	return TRUE;
}

void BG_WarriorEntity::OnDeserializationCallback(const VSerializationContext& context)
{
	VisBaseEntity_cl::OnDeserializationCallback(context);

	//init properties after deserialization
	InitializeProperties();

	PostInitialize();
}

void BG_WarriorEntity::MessageFunction(int iID, INT_PTR iParamA, INT_PTR iParamB )
{
	VisBaseEntity_cl::MessageFunction(iID, iParamA, iParamB);
	
	switch(iID)
	{
		case BG_UserMessages::kAnimationEvent:
			OnHavokAnimationEvent(*(hkbEvent*)(iParamA), iParamB != 0);
			break;
	}
}