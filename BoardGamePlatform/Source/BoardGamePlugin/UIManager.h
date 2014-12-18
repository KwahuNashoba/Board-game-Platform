
#ifndef BG_PLUGIN_UI_MANAGER_H__
#define BG_PLUGIN_UI_MANAGER_H__

#include <Vision\Runtime\Engine\Physics\IVisApiPhysicsModule.hpp>
#include <Vision\Runtime\EnginePlugins\VisionEnginePlugin\GUI\vGUI.hpp>

class BG_ControllerComponent;
typedef VSmartPtr<BG_ControllerComponent> BG_ControllerComponentPtr;

enum BG_Inputs
{
	iInvalid = -1,
	iClickTouch = 0,

	iCount
};

class BG_UIManager : public VDialog
{
public:
	BG_UIManager();

	VInputMap *GetInputMap() { return m_inputMap; }

private:
	//VDialog
	void OnActivate() HKV_OVERRIDE;
	void OnDeactivate() HKV_OVERRIDE;
	void OnTick(float deltaTime) HKV_OVERRIDE;
	//VCursor *GetCurrentCursor(VGUIUserInfo_t& user) HKV_OVERRIDE; //TODO: nisam siguran da ovo treba

	bool GetClosestPointOnNavMeshUnderCursor(hkVector4& point);

	void InitInputMap();
	hkvVec2 GetCursorPosition(IVGUIContext const* context) const;

private:
	VSmartPtr<VInputMap> m_inputMap;

	VGUIMainContextPtr m_GUIContext;
};

#endif