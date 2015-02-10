/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */


//  Basic Plugin Framework to house your own components
//

#include "BoardGamePluginPCH.h"
#include "GameManager.h"

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokSync.hpp>

#include <Common/Base/KeyCode.h>

// use plugins if supported
VIMPORT IVisPlugin_cl* GetEnginePlugin_vFmodEnginePlugin();

#if defined( HAVOK_PHYSICS_2012_KEYCODE )
VIMPORT IVisPlugin_cl* GetEnginePlugin_vHavok();
#endif
#if defined( HAVOK_AI_KEYCODE )
VIMPORT IVisPlugin_cl* GetEnginePlugin_vHavokAi();
#endif
#if defined( HAVOK_BEHAVIOR_KEYCODE )
VIMPORT IVisPlugin_cl* GetEnginePlugin_vHavokBehavior();
#endif

namespace
{
	class BG_PluginInitializer : public IVisCallbackHandler_cl
	{
	public:
		BG_PluginInitializer() 
			: m_bStaticsSynced(false)
		{}

	protected:
		void OnHandleCallback(IVisCallbackDataObject_cl *callback_data) HKV_OVERRIDE;

	private:
		bool m_bStaticsSynced;
	}
	s_PluginInitializer;
}

//============================================================================================================
//  Set up the Plugin Class
//============================================================================================================
class BG_Plugin : public IVisPlugin_cl
{
public:

  void OnInitEnginePlugin();    
  void OnDeInitEnginePlugin();  

  const char *GetPluginName()
  {
    return "BoardGamePlugin";  // must match DLL name
  }
};

//  global plugin instance
BG_Plugin g_BoardGamePlugin;

//--------------------------------------------------------------------------------------------
//  create a global instance of a VModule class
//  note: g_myComponentModule is defined in stdfx.h
//--------------------------------------------------------------------------------------------
DECLARE_THIS_MODULE(g_BoardGamePluginModule, MAKE_VERSION(1,0),
                    "Board game Plugin", 
                    "danilo.hadzi.pesic@gmail.com",
					"Plugin that allows implementing logic board games based on black vs white \"dots\" such as Lines in action, Checkers...etc"
					, &g_BoardGamePlugin);


//--------------------------------------------------------------------------------------------
//  Use this to get and initialize the plugin when you link statically
//--------------------------------------------------------------------------------------------
VEXPORT IVisPlugin_cl* GetEnginePlugin_BoardGamePlugin(){  return &g_BoardGamePlugin; }


#if (defined _DLL) || (defined _WINDLL)

  //  The engine uses this to get and initialize the plugin dynamically
  VEXPORT IVisPlugin_cl* GetEnginePlugin(){return GetEnginePlugin_BoardGamePlugin();}

#endif // _DLL or _WINDLL


//============================================================================================================
//  Initialize our plugin.
//============================================================================================================
//  Called when the plugin is loaded
//  We add our component initialize data here
void BG_Plugin::OnInitEnginePlugin()
{
  hkvLog::Info("BG_Plugin:OnInitEnginePlugin()");
  Vision::RegisterModule(&g_BoardGamePluginModule);
  //TODO: force static links
  
// load plugins if supported
#if defined( HAVOK_PHYSICS_2012_KEYCODE )
  VISION_PLUGIN_ENSURE_LOADED(vHavok);
#endif
#if defined( HAVOK_AI_KEYCODE )
  VISION_PLUGIN_ENSURE_LOADED(vHavokAi);
#endif
#if defined( HAVOK_BEHAVIOR_KEYCODE )
  VISION_PLUGIN_ENSURE_LOADED(vHavokBehavior);
#endif
  
  VISION_PLUGIN_ENSURE_LOADED(vFmodEnginePlugin);

  // Hook physics init
  vHavokPhysicsModule::OnBeforeInitializePhysics += &s_PluginInitializer;
  vHavokPhysicsModule::OnBeforeWorldCreated += &s_PluginInitializer;
  vHavokPhysicsModule::OnAfterDeInitializePhysics += &s_PluginInitializer;
  Vision::Callbacks.OnWorldInit += &s_PluginInitializer;

  // Start our component managers and game manager here....
  GameManager::GlobalManager().OneTimeInit();

  //---------------------------------------------------------------------------------------------------------
  // register the action module with the vision engine action manager
  // only after that will the action become available in the console.
  //---------------------------------------------------------------------------------------------------------
  VActionManager *pManager = Vision::GetActionManager();
  pManager->RegisterModule(&g_BoardGamePluginModule);

  // Set to true to open the console at startup and print some data to the display
  Vision::GetConsoleManager()->Show(false);
  pManager->Print( "Welcome to the console!" );
  pManager->Print( "This module is called '%s'", g_BoardGamePluginModule.GetName() );
  pManager->Print( "Type in 'help' for a list of all actions" );
  pManager->Print( "Type in 'myAction' to test this projects demo action" );
}

// Called before the plugin is unloaded
void BG_Plugin::OnDeInitEnginePlugin()
{
  hkvLog::Info("BG_Plugin:OnDeInitEnginePlugin()");

  GameManager::GlobalManager().OneTimeDeInit();
  
  // Unhook physics init
  vHavokPhysicsModule::OnAfterDeInitializePhysics -= &s_PluginInitializer;
  vHavokPhysicsModule::OnBeforeWorldCreated -= &s_PluginInitializer;
  vHavokPhysicsModule::OnBeforeInitializePhysics -= &s_PluginInitializer;
  Vision::Callbacks.OnWorldInit -= &s_PluginInitializer;
  
  // de-register component from action manager
  VActionManager * pManager = Vision::GetActionManager ();
  pManager->UnregisterModule( &g_BoardGamePluginModule );

  // de-register our module when the plugin is de-initialized
  Vision::UnregisterModule(&g_BoardGamePluginModule);
}

void BG_PluginInitializer::OnHandleCallback(IVisCallbackDataObject_cl *callback_data)
{
	if(&vHavokPhysicsModule::OnBeforeInitializePhysics == callback_data->m_pSender)
	{
		VISION_HAVOK_SYNC_STATICS();
		VISION_HAVOK_SYNC_PER_THREAD_STATICS(static_cast<vHavokPhysicsModuleCallbackData *>(callback_data)->GetHavokModule());
		m_bStaticsSynced = true;

		hkVtableClassRegistry::getInstance().registerList(hkBuiltinTypeRegistry::StaticLinkedTypeInfos, hkBuiltinTypeRegistry::StaticLinkedClasses);
	}
	else if(&vHavokPhysicsModule::OnBeforeWorldCreated == callback_data->m_pSender)
	{
		vHavokPhysicsModule *const havokModule = static_cast<vHavokBeforeWorldCreateDataObject_cl *>(callback_data)->GetHavokModule();
		
		havokModule->SetUseAsynchronousPhysics(false);
		havokModule->SetEnabledVisualDebugger(true);

		vHavokAiModule::GetInstance()->GetAiWorld()->getNavMeshCutter()->m_performValidationChecks = false;
	}
	else if(&vHavokPhysicsModule::OnAfterDeInitializePhysics == callback_data->m_pSender)
	{
		vHavokAiModule::GetInstance()->getCharacterBehaviors().clearAndDeallocate();

		VISION_HAVOK_UNSYNC_PER_THREAD_STATICS(static_cast<vHavokPhysicsModuleCallbackData *>(callback_data)->GetHavokModule());
		VISION_HAVOK_UNSYNC_STATICS();
		m_bStaticsSynced = false;
	}
	else if(&Vision::Callbacks.OnWorldInit == callback_data->m_pSender)
	{
		vHavokPhysicsModule* physicsModule = vHavokPhysicsModule::GetInstance();
		
		if(physicsModule != NULL && !m_bStaticsSynced)
		{
			physicsModule->OnDeInitPhysics();
			physicsModule->OnInitPhysics();
		}
	}
}

/*
 * Havok SDK - Base file, BUILD(#20140328)
 * 
 * Confidential Information of Havok.  (C) Copyright 1999-2014
 * Telekinesys Research Limited t/a Havok. All Rights Reserved. The Havok
 * Logo, and the Havok buzzsaw logo are trademarks of Havok.  Title, ownership
 * rights, and intellectual property rights in the Havok software remain in
 * Havok and/or its suppliers.
 * 
 * Use of this software for evaluation purposes is subject to and indicates
 * acceptance of the End User licence Agreement for this product. A copy of
 * the license is included with this software and is also available from salesteam@havok.com.
 * 
 */
