#ifndef GB_PLUGIN_CONTROLLER_COMPONENT_H__
#define GB_PLUGIN_CONTROLLER_COMPONENT_H__

#include "WarriorEntity.h"

//class BG_WarriorEntity;

class hkbEvent;
class hkaiBehavior;
class hkaiCharacter;

class BG_ControllerStateBase;

namespace BG_ControllerStateId
{
	enum Enum
	{
		kNotControlled,
		kIdling,
		kMoving,
		//TODO: dodaj napade kad ih smislis

		kStateCount
	};
}

class BG_ControllerComponent : public IVObjectComponent
{
public:
	BG_ControllerComponent(int id = 0, int componentFlags = VIS_OBJECTCOMPONENTFLAG_EXPOSETOEDITOR);

	void SetState(BG_ControllerStateId::Enum newStateId);

	BG_WarriorEntity* GetWarriorEntity() const { return vstatic_cast<BG_WarriorEntity*>(GetOwner());}

	//AI
	void RequestPath(hkvVec3 const& dest);
	void CancelRequestedPath();
	float GetPathGoalReachedTolerance() const;
	
	hkvVec3 GetDirection() const { return m_cachedSpeed > 0.0f ? m_cachedDirection : GetWarriorEntity()->GetDirection(); }
	float GetSpeed() const { return m_cachedSpeed; }
	void SetSensorSize(float sensorSize);
	void SetDesiredSpeed(float desiredSpeed);

	//Entity to attack
	void SetTarget(BG_WarriorEntity* target) { m_target = (target ? target->GetWeakReference() : NULL); }
	BG_WarriorEntity* getTarget() const { return vstatic_cast<BG_WarriorEntity*>(m_target); }
	
	void SetTargetPoint(hkvVec3 const& targetPoint) { m_targetPoint = targetPoint; }
	hkvVec3 const& GetTargetPoint() { return m_targetPoint; }

	virtual void OnTick(float deltaTime);

	virtual void OnProcessAnimationEvent(BG_WarriorEntity* owner, hkbEvent const& animEvent);

	// Called by AI world step with AI step dt
	void OnAfterCharacterStep(float deltaTime);

protected:
	//IVObjectComponent
	void Serialize(VArchive& ar) HKV_OVERRIDE;

	void SetOwner(VisTypedEngineObject_cl *newOwner) HKV_OVERRIDE;

private:
	V_DECLARE_SERIAL_DLLEXP(BG_ControllerComponent, BG_PLUGIN_IMPEXP);
	V_DECLARE_VARTABLE(BG_ControllerComponent, BG_PLUGIN_IMPEXP);

protected:
	VSmartPtr<BG_ControllerStateBase> m_states[BG_ControllerStateId::kStateCount];
	BG_ControllerStateId::Enum m_currentStateId;
	bool m_settingState;

	hkRefPtr<hkaiCharacter> m_aiCharacter;
	hkRefPtr<hkaiBehavior> m_aiBehavior;

	float m_sensorSize;
	float m_desiredSpeed;
	float m_cachedSpeed;
	hkvVec3 m_cachedDirection;

	VWeakPtr<VisBaseEntity_cl> m_target;
	hkvVec3 m_targetPoint;
};

//Abstract class used for managing BG_ControllerComponent's state
//All specific states should be derived from this class
class BG_ControllerStateBase : public VRefCounter
{
public:
	virtual void OnEnterState(BG_ControllerComponent* controller);
	virtual void OnExitState(BG_ControllerComponent* controller);
	virtual void OnProcessAnimationEvent(BG_ControllerComponent* controller, hkbEvent const& animationEvent);
	virtual void OnTick(BG_ControllerComponent* controller, float deltaTime);
	virtual char const *GetName() const = 0;
};

namespace BG_ControllerState
{
	class NotControlled : public BG_ControllerStateBase
	{
		void OnProcessAnimationEvent(BG_ControllerComponent* controller, hkbEvent const& animationEvent) HKV_OVERRIDE;

		char const *GetName() const HKV_OVERRIDE { return "Controller::NotControlled"; }
	};

	class Moving : public BG_ControllerStateBase
	{
		void OnEnterState(BG_ControllerComponent* controller) HKV_OVERRIDE;
		void OnExitState(BG_ControllerComponent* controller) HKV_OVERRIDE;

		char const *GetName() const HKV_OVERRIDE { return "Controller::Moving"; }
	};
}

namespace BG_ControllerHelper
{
	void CalcDirection(hkvVec3& resultDirection, hkvVec3& currentDirection, hkvVec3& desiredDirection, float t);
	bool GetClosestPointOnNavMesh(hkvVec3 const& position, float testRadius, hkvVec3& outPosition);
	bool IsPointOnNavMesh(hkvVec3 const& position, float const testRadius = 0.1f);
	void FaceTowards(BG_ControllerComponent* controller, hkvVec3 const& toPosition, float t);
	void GetProjectedDirAndDistFromTarget(BG_WarriorEntity const* warrior, BG_WarriorEntity *const target, hkvVec3& outDir, float& outDst);
	float GetMinDistanceToAttack(BG_WarriorEntity const* warrior, BG_WarriorEntity *const target);
}

#endif