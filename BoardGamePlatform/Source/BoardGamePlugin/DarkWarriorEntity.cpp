#include "BoardGamePluginPCH.h"
#include "DarkWarriorEntity.h"
#include "ControllerComponent.h"
#include "DarkWarriorControllerComponent.h"

#include <Vision\Runtime\EnginePlugins\Havok\HavokBehaviorEnginePlugin\vHavokBehaviorComponent.hpp>
#include <Vision\Runtime\EnginePlugins\Havok\HavokPhysicsEnginePlugin\vHavokCharacterController.hpp>

V_IMPLEMENT_SERIAL(BG_DarkWarriorEntity, BG_WarriorEntity, 0, &g_BoardGamePluginModule);
START_VAR_TABLE(BG_DarkWarriorEntity, BG_WarriorEntity, "Dark warrior enity", 0, "Models/Guardian_Melee/Guardian_Melee.model")
END_VAR_TABLE

BG_DarkWarriorEntity::BG_DarkWarriorEntity()
	:BG_WarriorEntity(),
	m_dyingDuration(3.8f)
{
}

VType* BG_DarkWarriorEntity::GetControllerComponentType()
{
	return V_RUNTIME_CLASS(BG_DarkWarriorControllerComponent);
}

void BG_DarkWarriorEntity::InitFunction()
{
	BG_WarriorEntity::InitFunction();

	m_havokBehavior = new vHavokBehaviorComponent();
	m_havokBehavior->m_projectName = "D:\\Havok\\AnarchySDK\\Data\\Vision\\Samples\\Engine\\RPG\\HavokAnimation\\Project\\Project.hkt";
	m_havokBehavior->m_characterName = "Guardian_Melee.hkt";
	m_havokBehavior->m_behaviorName = "Guardian_Melee.hkt";
	AddComponent(m_havokBehavior);

	//TODO: treba da se inicijalizuju ostali parametri
	m_collisionRadius = 40;
	m_collisionHeight = 160;
	m_sensorSize = 256;
	m_desiredSpeed = 350;

	PostInitialize();

	SetDirection(hkvVec3(-1,0,0));
}

void BG_DarkWarriorEntity::PostInitialize()
{
	BG_WarriorEntity::PostInitialize();
	//TODO: postavi audio
}

void BG_DarkWarriorEntity::OnTick(float deltaTime)
{
	//TODO: azuriraj poziciju zvuka

	BG_WarriorEntity::OnTick(deltaTime);
}