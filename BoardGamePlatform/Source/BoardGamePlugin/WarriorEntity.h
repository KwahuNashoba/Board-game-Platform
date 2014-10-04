#ifndef BG_PLUGIN_WARRIOR_ENTITY_H__
#define BG_PLUGIN_WARRIOR_ENTITY_H__

class vHavokBehaviorComponent;
typedef VSmartPtr<vHavokBehaviorComponent> vHavokBehavoirComponentPtr;

class BG_ControllerComponent;
typedef VSmartPtr<BG_ControllerComponent> BG_ControllerComponentPtr;

class vHavokCharacterController;
typedef VSmartPtr<vHavokCharacterController> vHavokCharacterControllerPtr;

namespace BG_WarriorAnimationEvent
{
	enum Enum
	{
		kTakeControl,
		kDie,

		kMove,
		kMoveEnd,

		kMeleeAttack,
		kMeleeAttackEnd,
		//TODO: dodaj ono sto treba

		kFootStepEffect,
		kMeleeAttackFire,


		kAnimationEventCount
	};
}

namespace BG_WarriorAnimationVariable
{
	enum Enum
	{
		kMoveSpeed,

		kAnimationVariableCount
	};
}

class BG_WarriorEntity : public VisBaseEntity_cl
{
public:
	BG_WarriorEntity();

	//TODO: overriduj Initialize funkciju, ce ti treba!!! a mozda i nece :D
	virtual void InitFunction() HKV_OVERRIDE;
	void InitializeProperties();
	virtual void PostInitialize();
	void DisposeObject() HKV_OVERRIDE;

	void ThinkFunction() HKV_OVERRIDE;

	virtual void OnTick(float deltaTime);

	void SetVisible(bool visible);

	virtual void CalcImpactReceivePosition(hkvVec3& targetPoint) const;	//returns position where enemy entity should impact this entity

	virtual VType* GetControllerComponentType() { return NULL; }

	virtual float GetCollisionRadius() const;
	virtual float GetCollisionHeight() const;
	//TODO: trebati ako implementiras napad iz daljine, u suprotnom obrisi
	hkvVec3 const GetEyePosition() const;

	vHavokBehaviorComponent *GetBehaviorComponent();
	vHavokBehaviorComponent const *GetBehaviorComponent() const;

	vHavokCharacterController *GetHavokCharacterControllerComponent() { return m_characterController; }
	
	void SetController(BG_ControllerComponent *newController);
	BG_ControllerComponent *GetControllerComponent();
	BG_ControllerComponent const *GetControllerComponent() const;
	void SetControllerComponent(BG_ControllerComponent* newController);
	bool IsOnNavMesh(float const testRadius = 1.0f) const;

	int GetIdForAnimationEvent(BG_WarriorAnimationEvent::Enum animationEvent) const { return m_animationEventIds[animationEvent]; }
	void RaiseAnimationEvent(BG_WarriorAnimationEvent::Enum animationEvent);
	void SetAnimationVariable(BG_WarriorAnimationVariable::Enum animationVariable, float value);

	//Effects TODO: add functions regarding effects

	virtual void Die();
	bool IsDead() const;
	bool IsDying() const;
	const hkvVec3 GetDeathImpulse() const;

	void InitAnimationEventIds();
	void InitAnimationVariableIds();
	
	void OnHavokAnimationEvent(hkbEvent const& behaviorEvent, bool raisedBySdk);

	void ProcessAnimationEvents();
	virtual void OnProcessAnimationEvent(hkbEvent const& behaviorEvent);

	void UpdateBehaviorWorldFromModel();

	// VisTypedEngineObject_cl
	void MessageFunction(int iID, INT_PTR iParamA, INT_PTR iParamB ) HKV_OVERRIDE;
	
protected:

	//serialization functions
	//for deeper understanding of serialization and its purpose read
	//Programmers Documentation/Vision Runtime Manual/Infrastructure/Serialization	
	//VisBaseEntity_cl
	void Serialize(VArchive& ar) HKV_OVERRIDE;
	//VTypedObject
	VBool WantsDeserializationCallback(const VSerializationContext& context) HKV_OVERRIDE;
	void OnDeserializationCallback(const VSerializationContext& context) HKV_OVERRIDE;


public:
	
	float m_collisionRadius;
	float m_collisionHeight;

protected:

	float m_eyeHeight;

	float m_sensorSize;
	float m_desiredSpeed;

	bool m_dead;
	bool m_dying;
	float m_timeOfDeath;
	hkvVec3 m_deathImpulse;

	int m_animationEventIds[BG_WarriorAnimationEvent::kAnimationEventCount];
	int m_animationVariableIds[BG_WarriorAnimationVariable::kAnimationVariableCount];

	vHavokBehavoirComponentPtr m_havokBehavior;
	hkArray<hkbEvent> m_queuedAnimationEvents;

	// Controller
	BG_ControllerComponentPtr m_controller;

	// Havok Character Proxy
	vHavokCharacterControllerPtr m_characterController;

	//Effects TODO:

private:
	V_DECLARE_SERIAL_DLLEXP(BG_WarriorEntity, BG_PLUGIN_IMPEXP);
	V_DECLARE_VARTABLE(BG_WarriorEntity, BG_PLUGIN_IMPEXP);
};

#endif