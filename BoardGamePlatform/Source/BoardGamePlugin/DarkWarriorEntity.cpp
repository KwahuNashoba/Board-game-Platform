#include "BoardGamePluginPCH.h"
#include "DarkWarriorEntity.h"
#include "ControllerComponent.h"
#include "DarkWarriorControllerComponent.h"
#include "SoundHelper.h"

#include <Vision\Runtime\EnginePlugins\Havok\HavokBehaviorEnginePlugin\vHavokBehaviorComponent.hpp>
#include <Vision\Runtime\EnginePlugins\Havok\HavokPhysicsEnginePlugin\vHavokCharacterController.hpp>

V_IMPLEMENT_SERIAL(BG_DarkWarriorEntity, BG_WarriorEntity, 0, &g_BoardGamePluginModule);
START_VAR_TABLE(BG_DarkWarriorEntity, BG_WarriorEntity, "Dark warrior enity", 0, "Models/Dark_Warrior/Dark_Warrior.model")
END_VAR_TABLE

BG_DarkWarriorEntity::BG_DarkWarriorEntity()
	:BG_WarriorEntity(),
	m_dyingDuration(2.0f)
{
	m_soundHelper = new BG_SoundHelper(false);
}

VType* BG_DarkWarriorEntity::GetControllerComponentType()
{
	return V_RUNTIME_CLASS(BG_ControllerComponent);
}

void BG_DarkWarriorEntity::InitFunction()
{
	BG_WarriorEntity::InitFunction();

	m_havokBehavior = new vHavokBehaviorComponent();
	m_havokBehavior->m_projectName = "HavokAnimation/BG_Warriors.hkt";
	m_havokBehavior->m_characterName = "Dark_Warrior.hkt";
	m_havokBehavior->m_behaviorName = "Dark_Warrior.hkt";
	AddComponent(m_havokBehavior);

	VTextureObject *textureHandler = Vision::TextureManager.Load2DTexture("Assets/Models/Textures/Dark_Warrior/Dark_Warrior_cloth_d.png");
	GetMesh()->GetSurface(3)->SetTexture(VisSurfaceTextures_cl::VTextureType_e::VTT_Diffuse, textureHandler);
	textureHandler = Vision::TextureManager.Load2DTexture("Assets/Models/Textures/Dark_Warrior/Dark_Warrior_cloth_n.png");
	GetMesh()->GetSurface(3)->SetTexture(VisSurfaceTextures_cl::VTextureType_e::VTT_NormalMap, textureHandler);

	textureHandler = Vision::TextureManager.Load2DTexture("Assets/Models/Textures/Dark_Warrior/Dark_Warrior_bones_d.png");
	GetMesh()->GetSurface(1)->SetTexture(VisSurfaceTextures_cl::VTextureType_e::VTT_Diffuse, textureHandler);
	textureHandler = Vision::TextureManager.Load2DTexture("Assets/Models/Textures/Dark_Warrior/Dark_Warrior_bones_n.png");
	GetMesh()->GetSurface(1)->SetTexture(VisSurfaceTextures_cl::VTextureType_e::VTT_NormalMap, textureHandler);

	textureHandler = Vision::TextureManager.Load2DTexture("Assets/Models/Textures/Dark_Warrior/Dark_Warrior_wepons_d.tga");
	GetMesh()->GetSurface(2)->SetTexture(VisSurfaceTextures_cl::VTextureType_e::VTT_Diffuse, textureHandler);
	GetMesh()->GetSurface(0)->SetTexture(VisSurfaceTextures_cl::VTextureType_e::VTT_Diffuse, textureHandler);
	textureHandler = Vision::TextureManager.Load2DTexture("Assets/Models/Textures/Dark_Warrior/Dark_Warrior_wepons_n.tga");
	GetMesh()->GetSurface(2)->SetTexture(VisSurfaceTextures_cl::VTextureType_e::VTT_NormalMap, textureHandler);
	GetMesh()->GetSurface(0)->SetTexture(VisSurfaceTextures_cl::VTextureType_e::VTT_NormalMap, textureHandler);

	m_collisionRadius = 30;
	m_collisionHeight = 160;
	m_sensorSize = 64;
	m_desiredSpeed = 80;

	PostInitialize();

	SetDirection(hkvVec3(-1,0,0));
}

void BG_DarkWarriorEntity::PostInitialize()
{
	BG_WarriorEntity::PostInitialize();
}

void BG_DarkWarriorEntity::OnTick(float deltaTime)
{
	BG_WarriorEntity::OnTick(deltaTime);
}