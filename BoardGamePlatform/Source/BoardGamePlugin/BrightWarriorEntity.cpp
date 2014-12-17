#include "BoardGamePluginPCH.h"
#include "BrightWarriorEntity.h"
#include "ControllerComponent.h"
#include "BrightWarriorControllerComponent.h"

#include <Vision\Runtime\EnginePlugins\Havok\HavokBehaviorEnginePlugin\vHavokBehaviorComponent.hpp>
#include <Vision\Runtime\EnginePlugins\Havok\HavokPhysicsEnginePlugin\vHavokCharacterController.hpp>

V_IMPLEMENT_SERIAL(BG_BrightWarriorEntity, BG_WarriorEntity, 0, &g_BoardGamePluginModule);
START_VAR_TABLE(BG_BrightWarriorEntity, BG_WarriorEntity, "Bright warrior enity", 0, "Models/Barbarian/Barbarian.model")
END_VAR_TABLE

BG_BrightWarriorEntity::BG_BrightWarriorEntity()
	:BG_WarriorEntity(),
	m_dyingDuration(3.8f)
{
}

VType* BG_BrightWarriorEntity::GetControllerComponentType()
{
	return V_RUNTIME_CLASS(BG_BrightWarriorControllerComponent);
}

void BG_BrightWarriorEntity::InitFunction()
{
	BG_WarriorEntity::InitFunction();

	m_havokBehavior = new vHavokBehaviorComponent();
	m_havokBehavior->m_projectName = "HavokAnimation/BG_Warriors.hkt";
	m_havokBehavior->m_characterName = "Barbarian.hkt";
	m_havokBehavior->m_behaviorName = "Barbarian.hkt";
	AddComponent(m_havokBehavior);

	//load textures
	VTextureObject *textureHandler = Vision::TextureManager.Load2DTexture("Assets/Models/Textures/Barbarian/Barbarian_Body_Low_d.tga");
	GetMesh()->GetSurface(1)->SetTexture(VisSurfaceTextures_cl::VTextureType_e::VTT_Diffuse, textureHandler);
	
	textureHandler = Vision::TextureManager.Load2DTexture("Assets/Models/Textures/Barbarian/Barbarian_Body_Low_n.tga");
	GetMesh()->GetSurface(1)->SetTexture(VisSurfaceTextures_cl::VTextureType_e::VTT_NormalMap, textureHandler);
	
	textureHandler = Vision::TextureManager.Load2DTexture("Assets/Models/Textures/Barbarian/Barbarian_Belt_Low_d.tga");
	GetMesh()->GetSurface(0)->SetTexture(VisSurfaceTextures_cl::VTextureType_e::VTT_Diffuse, textureHandler);
	
	textureHandler = Vision::TextureManager.Load2DTexture("Assets/Models/Textures/Barbarian/Barbarian_Belt_Low_n.tga");
	GetMesh()->GetSurface(0)->SetTexture(VisSurfaceTextures_cl::VTextureType_e::VTT_NormalMap, textureHandler);
	
	m_collisionRadius = 40;
	m_collisionHeight = 160;
	m_sensorSize = 256;
	m_desiredSpeed = 100;

	PostInitialize();
	
}

void BG_BrightWarriorEntity::PostInitialize()
{
	BG_WarriorEntity::PostInitialize();
	//TODO: postavi audio
}

void BG_BrightWarriorEntity::OnTick(float deltaTime)
{
	//TODO: azuriraj poziciju zvuka

	BG_WarriorEntity::OnTick(deltaTime);
}