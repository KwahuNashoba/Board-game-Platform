#include "BoardGamePluginPCH.h"
#include "UIManager.h"

#include "ControllerComponent.h"
#include "WarriorEntity.h"
#include "GameManager.h"

namespace
{

}

BG_UIManager::BG_UIManager()
	: VDialog()
{
	m_GUIContext = new VGUIMainContext(NULL);

	InitDialog(m_GUIContext, NULL, NULL);

	m_GUIContext->ShowDialog(this);
	
	m_GUIContext->SetActivate(true);
}

void BG_UIManager::OnActivate()
{
	VDialog::OnActivate();

	int width, height;
	VisRenderContext_cl::GetCurrentContext()->GetSize(width, height);
	SetSize(static_cast<float>(width), static_cast<float>(height));

	m_inputMap = new VInputMap(BG_Inputs::iCount, 4);
	InitInputMap();
}

void BG_UIManager::OnDeactivate()
{
	VDialog::OnDeactivate();

	m_inputMap = NULL;
}

void BG_UIManager::OnTick(float deltaTime)
{
	VDialog::OnTick(deltaTime);
	if(m_inputMap->GetTrigger(iClickTouch) > 0.0f)
	{
		hkVector4 targetPoint;
		GetClosestPointOnNavMeshUnderCursor(targetPoint);
		hkvVec3 convertedTargetPoint;
		vHavokConversionUtils::PhysVecToVisVecLocal(targetPoint, convertedTargetPoint);
		//check if click is inside the board
		if(convertedTargetPoint.x <= 8*BG_WARRIOR_MODEL_WIDTH && convertedTargetPoint.y <= 8*BG_WARRIOR_MODEL_WIDTH)
		{
			GameManager::GlobalManager().SetMouseInput(convertedTargetPoint);
		}
		else
		{
			GameManager::GlobalManager().SetMouseInput(hkvVec3(-1,-1,-1));
		}
	}
	else
	{
		GameManager::GlobalManager().SetMouseInput(hkvVec3(-1,-1,-1));
	}
	//TODO: verovatno ce treba da se doda jos neki input...pauza, quit i sl
}

bool BG_UIManager::GetClosestPointOnNavMeshUnderCursor(hkVector4& point)
{
	IVGUIContext *context = GetContext();
	VASSERT(context);

	hkvVec2 const mousePosition = GetCursorPosition(context);
	hkvVec3 traceDirOut;
	//Compute a 3D direction vector in world space from 2D screen position
	VisRenderContext_cl::GetCurrentContext()->GetTraceDirFromScreenPos(mousePosition.x, mousePosition.y, traceDirOut, 1.0f);
	hkvVec3 const& cameraPosition = VisRenderContext_cl::GetCurrentContext()->GetCamera()->GetPosition();

	hkVector4 rayStart, rayEnd;
	vHavokConversionUtils::VisVecToPhysVecLocal(cameraPosition, rayStart);
	vHavokConversionUtils::VisVecToPhysVecLocal(cameraPosition + traceDirOut*3000.0f, rayEnd);

	hkaiNavMeshQueryMediator::HitDetails hit;
	if(vHavokAiModule::GetInstance()->GetAiWorld()->getDynamicQueryMediator()->castRay(rayStart, rayEnd, hit))
	{
		point.setInterpolate(rayStart, rayEnd, hit.m_hitFraction);
		return true;
	}

	return false;
}

void BG_UIManager::InitInputMap()
{
	m_inputMap->MapTrigger(iClickTouch, V_MOUSE, CT_MOUSE_LEFT_BUTTON, VInputOptions::Once());

	//TODO: dodaj za touch ako radis build za android
}

hkvVec2 BG_UIManager::GetCursorPosition(IVGUIContext const *context) const
{
	//TODO: dodaj za touch
	return context->GetCurrentMousePos();
}