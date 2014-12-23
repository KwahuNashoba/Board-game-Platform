#include "BoardGamePluginPCH.h"
#include "BrightWarriorEntity.h"
#include "ControllerComponent.h"
#include "BrightWarriorControllerComponent.h"
#include "SoundHelper.h"

#include <Vision\Runtime\EnginePlugins\Havok\HavokBehaviorEnginePlugin\vHavokBehaviorComponent.hpp>
#include <Vision\Runtime\EnginePlugins\Havok\HavokPhysicsEnginePlugin\vHavokCharacterController.hpp>
#include <Vision\Runtime\EnginePlugins\VisionEnginePlugin\Scene\VPrefab.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Components/VSkeletalBoneProxy.hpp>

V_IMPLEMENT_SERIAL(BG_BrightWarriorEntity, BG_WarriorEntity, 0, &g_BoardGamePluginModule);
START_VAR_TABLE(BG_BrightWarriorEntity, BG_WarriorEntity, "Bright warrior enity", 0, "Models/Barbarian/Barbarian.model")
END_VAR_TABLE

BG_BrightWarriorEntity::BG_BrightWarriorEntity()
	:BG_WarriorEntity(),
	m_dyingDuration(3.8f)
{
	m_soundHelper = new BG_SoundHelper(true);
}

VType* BG_BrightWarriorEntity::GetControllerComponentType()
{
	return V_RUNTIME_CLASS(BG_ControllerComponent);
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

	//load weapon model
	m_weapon = Vision::Game.CreateEntity("VisBaseEntity_cl", hkvVec3(0,0,0), "Models/Barbarian/Barbarian_Axe.model");

	//load textures - diffuse & normal map
	VTextureObject *textureHandler = Vision::TextureManager.Load2DTexture("Models/Textures/Barbarian/Barbarian_Weapon_Axe_d.tga");
	m_weapon->GetMesh()->GetSurface(0)->SetTexture(VisSurfaceTextures_cl::VTextureType_e::VTT_Diffuse, textureHandler);

	textureHandler = Vision::TextureManager.Load2DTexture("Models/Textures/Barbarian/Barbarian_Weapon_Axe_n.tga");
	m_weapon->GetMesh()->GetSurface(0)->SetTexture(VisSurfaceTextures_cl::VTextureType_e::VTT_NormalMap, textureHandler);

	VSkeletalBoneProxyObject* boneProxy = new VSkeletalBoneProxyObject();
	boneProxy->AttachToEntityBone(this, "right_attachment_jnt");
	boneProxy->UpdateBoneBinding();

	m_weapon->AttachToParent(boneProxy);

	m_weapon->ResetLocalTransformation();
	m_weapon->SetLocalPosition(hkvVec3(0,0,0));
	m_weapon->SetLocalOrientation(hkvVec3(0,0,0));
}

void BG_BrightWarriorEntity::OnTick(float deltaTime)
{
	//TODO: azuriraj poziciju zvuka

	BG_WarriorEntity::OnTick(deltaTime);
}