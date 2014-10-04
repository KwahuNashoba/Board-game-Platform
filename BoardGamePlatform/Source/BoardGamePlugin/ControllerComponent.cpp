#include "BoardGamePluginPCH.h"
#include "ControllerComponent.h"

V_IMPLEMENT_SERIAL(BG_ControllerComponent, IVObjectComponent, 0, &g_BoardGamePluginModule);
START_VAR_TABLE(BG_ControllerComponent, IVObjectComponent, "Board Game Controller component", VFORGE_HIDECLASS, "")
	DEFINE_VAR_FLOAT_AND_NAME(BG_ControllerComponent, m_sensorSize, "SensorSize", "Warrior sensor size", "128", 0, NULL);
	DEFINE_VAR_FLOAT_AND_NAME(BG_ControllerComponent, m_desiredSpeed, "DesiredSpeed", "Warrior desired speed", "200", 0, NULL);	
END_VAR_TABLE

namespace
{
	//listener of events in Ai world
	class AiWorldListener : public hkaiWorld::Listener
	{
	public:
		void AddController(BG_ControllerComponent* controller);
		void RemoveController(BG_ControllerComponent* controller);

	private:
		void AddListener();
		void RemoveListener();

		void postCharacterStepCallback(hkaiWorld::CharacterStepCallbackContext const& ctx) HKV_OVERRIDE;

	private:
		VRefCountedCollection<BG_ControllerComponent> m_controllers;
	}
	aiWorldListener;

	class LocalSteeringFilter : public hkaiLocalSteeringFilter
	{
		hkBool32 isCharacterEnabled(hkaiCharacter const *aiCharacter, hkaiCharacter const *otherAiCharacter) const HK_OVERRIDE;
		hkBool32 isObstacleEnabled(hkaiCharacter const *aiCharacter, hkaiObstacleGenerator const *otherObstacle) const HK_OVERRIDE;
	}
	localSteeringFilter;
}

BG_ControllerComponent::BG_ControllerComponent(int id, int componentFlag)
	: IVObjectComponent(id, componentFlag),
	m_currentStateId(BG_ControllerStateId::kIdling),
	m_settingState(false),
	m_sensorSize(100.0f),
	m_desiredSpeed(300.0f),
	m_cachedDirection(0.0f, 0.0f, 0.0f),
	m_cachedSpeed(0.0f),
	m_target(NULL)
{
}

void BG_ControllerComponent::SetState(BG_ControllerStateId::Enum newStateId)
{
	VASSERT(!m_settingState);
	if(!m_settingState && m_currentStateId != newStateId)
	{
		m_settingState = true;
		BG_ControllerStateBase *currentState = m_states[m_currentStateId];
		//exit current state
		if(currentState)
		{
			currentState->OnExitState(this);
		}

		m_currentStateId = newStateId;

		currentState = m_states[m_currentStateId];
		//enter new state
		if(currentState)
		{
			currentState->OnEnterState(this);
		}

		m_settingState = false;
	}
}

void BG_ControllerComponent::RequestPath(hkvVec3 const& dest)
{
	hkVector4 scaledDest;
	vHavokConversionUtils::VisVecToPhysVecLocal(dest, scaledDest);
	m_aiBehavior->requestPath(scaledDest);
}

void BG_ControllerComponent::CancelRequestedPath()
{
	m_aiBehavior->cancelRequestedPath();
}

float BG_ControllerComponent::GetPathGoalReachedTolerance() const
{
	hkaiPathFollowingBehavior* pathFollowingBehavior = static_cast<hkaiPathFollowingBehavior*>(m_aiBehavior.val());
	float const radiusMultiplier = hkvMath::Max(0.0f, pathFollowingBehavior->getCharacterRadiusMultiplier() - 1.0f);
	float const tolerance = m_aiCharacter->getRadius() * radiusMultiplier + pathFollowingBehavior->getPathFollowingProperties()->m_goalDistTolerance;
	return HK2VIS_FLOAT_SCALED(tolerance);
}

void BG_ControllerComponent::SetSensorSize(float sensorSize)
{
	m_sensorSize = sensorSize;
}

void BG_ControllerComponent::SetDesiredSpeed(float desiredSpeed)
{
	m_desiredSpeed = desiredSpeed;
}

void BG_ControllerComponent::OnTick(float deltaTime)
{
	BG_WarriorEntity const *const controlledWarrior = static_cast<BG_WarriorEntity*>(GetOwner());

	VASSERT(controlledWarrior);
	if(!controlledWarrior || controlledWarrior->IsDead())
	{
		return;
	}

	hkVector4 warriorPos;
	//convert Vision vector to Havok vector
	vHavokConversionUtils::VisVecToPhysVecLocal(controlledWarrior->GetPosition(), warriorPos);
	//update aiCharacter with current position
	m_aiCharacter->setPosition(warriorPos);
	
	hkaiWorld *const aiWorld = vHavokAiModule::GetInstance()->GetAiWorld();
	hkaiCharacter *const aiCharacter = m_aiCharacter;

	hkaiCharacterUtil::ProjectToMeshSettings projectToMeshSettings;
	//apply motion info to controlled mesh and move the mesh
	hkaiCharacterUtil::integrateMotionAndProjectToMesh(deltaTime, aiWorld, &aiCharacter, 1, projectToMeshSettings);

	//perform additional processing specific to current state
	if(m_states[m_currentStateId])
	{
		m_states[m_currentStateId]->OnTick(this, deltaTime);
	}
}

void BG_ControllerComponent::OnProcessAnimationEvent(BG_WarriorEntity* owner, hkbEvent const& animEvent)
{
	if(m_states[m_currentStateId])
	{
		m_states[m_currentStateId]->OnProcessAnimationEvent(this, animEvent);
	}
}

void BG_ControllerComponent::OnAfterCharacterStep(float deltaTime)
{
	//get suggested velocity vector by Ai world step
	hkVector4 const& aiVelocity = m_aiCharacter->getVelocity();
	
	vHavokConversionUtils::PhysVecToVisVecLocal(aiVelocity, m_cachedDirection);
	m_cachedSpeed = VIS2HK_FLOAT_SCALED(m_cachedDirection.getLengthAndNormalize());

	hkVector4 aiDisplacement;
	aiDisplacement.setMul(deltaTime, aiVelocity);

	hkvVec3 motionDelta;
	vHavokConversionUtils::PhysVecToVisVecLocal(aiDisplacement, motionDelta);

	BG_WarriorEntity* controlledWarrior = static_cast<BG_WarriorEntity*>(GetOwner());
	//moves entity towards a goal - this is used with animations that has no motion delta in them
	controlledWarrior->IncMotionDeltaWorldSpace(motionDelta);

}

void BG_ControllerComponent::SetOwner(VisTypedEngineObject_cl* newOwner)
{
	IVObjectComponent::SetOwner(newOwner);

	BG_WarriorEntity* warriorOwner = static_cast<BG_WarriorEntity*>(newOwner);
	if(warriorOwner)
	{
		hkvVec3 const& position = warriorOwner->GetPosition();
		m_cachedDirection = warriorOwner->GetDirection();

		hkaiAvoidanceSolver::AvoidanceProperties* avoidanceProperties = new hkaiAvoidanceSolver::AvoidanceProperties();
		hkReal const sensorSize = VIS2HK_FLOAT_SCALED(m_sensorSize);
		//set bounding box parameters
		avoidanceProperties->m_localSensorAabb.m_max.setAll3(sensorSize);
		avoidanceProperties->m_localSensorAabb.m_min.setNeg3(avoidanceProperties->m_localSensorAabb.m_max);

		hkaiCharacter::Cinfo characterInfo;
		vHavokConversionUtils::VisVecToPhysVecLocal(position, characterInfo.m_initialPosition);

		characterInfo.m_up = hkVector4::getConstant<HK_QUADREAL_0010>();

		hkReal const radius = VIS2HK_FLOAT_SCALED(warriorOwner->GetCollisionRadius());
		hkReal const desiredSpeed = VIS2HK_FLOAT_SCALED(m_desiredSpeed);
		hkReal const height = VIS2HK_FLOAT_SCALED(warriorOwner->GetCollisionHeight());
		
		characterInfo.m_radius = radius;
		characterInfo.m_desiredSpeed = desiredSpeed;
		characterInfo.m_avoidanceProperties = avoidanceProperties;
		characterInfo.m_userData = (hkUlong)this;

		m_aiCharacter = new hkaiCharacter(characterInfo);
		m_aiCharacter->setSteeringFilter(&localSteeringFilter);
		avoidanceProperties->removeReference();
		m_aiCharacter->removeReference();

		hkaiPathFollowingBehavior::Cinfo behaviorCinfo;
		m_aiBehavior = new hkaiPathFollowingBehavior(m_aiCharacter, vHavokAiModule::GetInstance()->GetAiWorld(), behaviorCinfo);
		//one reference to an object must be removed because of the nature of hkRefPtr - line above sets ref count to 2
		m_aiBehavior->removeReference();

		vHavokAiModule::GetInstance()->getCharacterBehaviors().pushBack(m_aiBehavior);

		aiWorldListener.AddController(this);
	}
	else
	{
		aiWorldListener.RemoveController(this);

		int const aiBehaviorIndex = vHavokAiModule::GetInstance()->getCharacterBehaviors().indexOf(m_aiBehavior);
		VASSERT(aiBehaviorIndex >= 0);
		if(aiBehaviorIndex >= 0)
		{
			vHavokAiModule::GetInstance()->getCharacterBehaviors().removeAt(aiBehaviorIndex);
		}

		m_aiBehavior = NULL;
		m_aiCharacter = NULL;

		for(int i = 0; i < BG_ControllerStateId::kStateCount; i++)
		{
			m_states[i] = NULL;
		}
	}
}

void BG_ControllerComponent::Serialize(VArchive& ar)
{
	//TODO: imas dupli kod za serializaciju. Smisli gde ces da ga stavis i pretvoris ga u neku staticku funckiju
	//call parent class Serialize function
	IVObjectComponent::Serialize(ar);

	if(ar.IsLoading())
	{
		//get list of objects variables
		VARIABLE_LIST const* const varList = this->GetVariableList();
		VASSERT(varList);
		if(varList)
		{
			int const numVars = varList->GetSize();
			ar << numVars;

			for(VARIABLE_ELEM const* el = varList->first; el; el = el->next)
			{
				VisVariable_cl const* const var = el->value;
				ar << var->name;
				ar << var->type;

				switch(var->type)
				{
					case VULPTYPE_REFERENCED_OBJECT:
						{
							// cast entity objects address in char*(string) and add offset of this var in its objects class, then cast it to VTypedObjectReference
							//as far as I understood it :)
							VTypedObjectReference const* const ref = (VTypedObjectReference const*)((char const*)this + var->clsOffset);
							//grab variable and put in archive
							ar << ref->GetReferencedObject();
						}
						break;
					default:
						{
							char valueString[1000];
							const_cast<VisVariable_cl*>(var)->GetValue(const_cast<VTypedObject*>((VTypedObject const*) this), valueString);
							ar << valueString;
						}
				}
			}
		}
	}
	else
	{
		int numVars;
		ar >> numVars;
		for(int i = 0; i < numVars; i++)
		{
			VString varName;
			ar >> varName;

			int varType;
			ar >> varType;

			VisVariable_cl *const var = this->GetVariable(varName);
			VASSERT(var && var->type == varType);
			if(var && var->type == varType)
			{
				switch(varType)
				{
					case VULPTYPE_REFERENCED_OBJECT:
						{
							VTypedObject *varObj;
							ar >> varObj;

							VTypedObjectReference *const ref = (VTypedObjectReference*)((char*)this + var->clsOffset);
							ref->SetReferencedObject(varObj);
						}
						break;

					default:
						{
							VString varValue;
							ar >> varValue;

							var->SetValue((VTypedObject*)this, varValue.AsChar());
						}
				}
			}
		}
	}
}

void BG_ControllerStateBase::OnEnterState(BG_ControllerComponent *const controller)
{
}

void BG_ControllerStateBase::OnExitState(BG_ControllerComponent *const controller)
{
}

void BG_ControllerStateBase::OnProcessAnimationEvent(BG_ControllerComponent *const controller, hkbEvent const& animationEvent)
{
}

void BG_ControllerStateBase::OnTick(BG_ControllerComponent *const controller, float const deltaTime)
{
}

void BG_ControllerState::NotControlled::OnProcessAnimationEvent(BG_ControllerComponent* controller, hkbEvent const& animationEvent)
{
	BG_ControllerStateBase::OnProcessAnimationEvent(controller, animationEvent);

	BG_WarriorEntity *const warriorEntity = controller->GetWarriorEntity();

	//TODO: ovo mozda nece da ti treba...isprobaj dal ga okine kad postavis inicijalno stanje na kNotControlled
	if(warriorEntity->GetIdForAnimationEvent(BG_WarriorAnimationEvent::kTakeControl) == animationEvent.getId())
	{
		controller->SetState(BG_ControllerStateId::kIdling);
	}
}

void BG_ControllerState::Moving::OnEnterState(BG_ControllerComponent* controller)
{
	BG_ControllerStateBase::OnEnterState(controller);
	BG_WarriorEntity *const warriorEntity = controller->GetWarriorEntity();
	//TODO: create moving effect
	warriorEntity->RaiseAnimationEvent(BG_WarriorAnimationEvent::kMove);
}

void BG_ControllerState::Moving::OnExitState(BG_ControllerComponent *const controller)
{
	BG_ControllerStateBase::OnExitState(controller);
	BG_WarriorEntity *const warriorEntity = controller->GetWarriorEntity();
	controller->CancelRequestedPath();
	//TODO: pause moving effect
	warriorEntity->RaiseAnimationEvent(BG_WarriorAnimationEvent::kMoveEnd);
}

//Helper functions
void BG_ControllerHelper::CalcDirection(hkvVec3& resultDirection, hkvVec3& currentDirection, hkvVec3& desiredDirection, float t)
{
	if(currentDirection.dot(desiredDirection) < 0.99f)
	{
		float const dot = currentDirection.dot(desiredDirection);
		float const theta = hkvMath::acosRad(dot) * hkvMath::clamp(t, 0.f, 1.f);
		hkvVec3 const vec = (desiredDirection - currentDirection * dot).getNormalized();
		resultDirection = currentDirection * hkvMath::cosRad(theta) + vec * hkvMath::sinRad(theta);
	  }
	  else
	  {
		resultDirection = desiredDirection;
	  }
}

bool BG_ControllerHelper::GetClosestPointOnNavMesh(hkvVec3 const& position, float testRadius, hkvVec3& outPosition)
{
	VASSERT(vHavokAiModule::GetInstance()->GetAiWorld());

	hkaiWorld const *const aiWorld = vHavokAiModule::GetInstance()->GetAiWorld();
	if(!aiWorld)
	{
		return false;
	}

	hkVector4 inPos, outPos;
	vHavokConversionUtils::VisVecToPhysVecLocal(position, inPos);

	hkaiPackedKey faceKey = aiWorld->getDynamicQueryMediator()->getClosestPoint(inPos, testRadius, outPos);
	if(faceKey != HKAI_INVALID_PACKED_KEY)
	{
		vHavokConversionUtils::PhysVecToVisVecLocal(outPos, outPosition);
		return true;
	}

	return false;
}

bool BG_ControllerHelper::IsPointOnNavMesh(hkvVec3 const& position, float const testRadius)
{
	hkvVec3 outPos;
	return GetClosestPointOnNavMesh(position, testRadius, outPos);
}

void BG_ControllerHelper::FaceTowards(BG_ControllerComponent* controller, hkvVec3 const& toPosition, float t)
{
	hkvVec3 dir;
	hkvVec3 const up(0.0f,0.0f,1.0f);
	hkvVec3 const fromTo(toPosition - controller->GetWarriorEntity()->GetPosition());
	hkvVec3 fromToProjectedDir(fromTo - fromTo.dot(up) * up);
	if(fromToProjectedDir.getLengthAndNormalize() > 0.1f)
	{
		BG_ControllerHelper::CalcDirection(dir, controller->GetWarriorEntity()->GetDirection(), fromToProjectedDir, t);
	}
}

void BG_ControllerHelper::GetProjectedDirAndDistFromTarget(BG_WarriorEntity const* warrior, BG_WarriorEntity *const target, hkvVec3& outDir, float& outDst)
{
	hkvVec3 const warriorFromTarget(warrior->GetPosition() - target->GetPosition());
	hkvVec3 const up(0.0f, 0.0f, 1.0f);
	
	outDir = warriorFromTarget - warriorFromTarget.dot(up) * up;
	outDst = outDir.getLengthAndNormalize();
}

float BG_ControllerHelper::GetMinDistanceToAttack(BG_WarriorEntity const* warrior, BG_WarriorEntity *const target)
{
	//TODO: ovo ti mozda treba a mozda i ne
	float f = 100;
	return f;
}


//Ai world listener functions
void AiWorldListener::AddController(BG_ControllerComponent* controller)
{
	VASSERT(controller);
	if(controller)
	{
		//if this is first controller register this listener to aiWorld
		if(m_controllers.Count() == 0)
		{
			AddListener();
		}

		m_controllers.AddUnique(controller);
	}
}

void AiWorldListener::RemoveController(BG_ControllerComponent* controller)
{
	VASSERT(controller);
	if(controller)
	{
		m_controllers.Remove(controller);
	}

	//if this is last one unregister this listener from aiWorld
	if(m_controllers.Count() == 0)
	{
		RemoveListener();
	}
}
void AiWorldListener::AddListener()
{
	vHavokAiModule::GetInstance()->GetAiWorld()->addListener(this);
}
void AiWorldListener::RemoveListener()
{
	vHavokAiModule::GetInstance()->GetAiWorld()->removeListener(this);
}
void AiWorldListener::postCharacterStepCallback(hkaiWorld::CharacterStepCallbackContext const& ctx)
{
	int const controllerCount = m_controllers.Count();
	for(int i = 0; i < controllerCount; i++)
	{
		BG_ControllerComponent *const controller = m_controllers.GetAt(i);
		controller->OnAfterCharacterStep(ctx.m_timestep);
	}
}

hkBool32 LocalSteeringFilter::isCharacterEnabled(hkaiCharacter const *aiCharacter, hkaiCharacter const *otherAiCharacter) const
{
	BG_ControllerComponent *const controller = (BG_ControllerComponent *const)aiCharacter->m_userData;
	BG_WarriorEntity *const otherWarrior = ((BG_ControllerComponent *const)otherAiCharacter->m_userData)->GetWarriorEntity();

	//Don't steer around the character we are about to attack
	return controller->getTarget() == otherWarrior ? 0 : 1;
}

hkBool32 LocalSteeringFilter::isObstacleEnabled(hkaiCharacter const *aiCharacter, hkaiObstacleGenerator const *otherObstacle) const
{
	return 1;
	//TODO: proveri ovo!!! stavio sam da vraca uvek true posto bi u igri trebalo da napada samo entitije koji imaju aiCharactere
	//a to bi trebalo da resi funkcija iznad
}