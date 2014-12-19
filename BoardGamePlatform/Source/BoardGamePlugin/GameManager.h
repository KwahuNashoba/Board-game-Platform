/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */


//  This class show how to use the vForge "play the Game" mode

#ifndef BG_PLUGIN_GAME_MANAGER_H__
#define BG_PLUGIN_GAME_MANAGER_H__

#include "BoardGamePluginModule.h"

class BG_WarriorEntity;
class BG_BrightWarriorEntity;
class BG_DarkWarriorEntity;
class BG_UIManager;
class BG_GameLogic;

class GameManager : public IVisCallbackHandler_cl
{
public:
	GameManager();

	// called when plugin is loaded/unloaded
	void OneTimeInit();
	void OneTimeDeInit();

	//x and y are board coordinates NOT world coordinates (0 <= x < 8), (0 <= y < 8)
	void AddWarrior(BG_WarriorEntity* warrior, int x, int y);

	//TODO: mozda ovo moze i pametnije da se odradi umesto dve funkcije koje rade gotovo istu stvar
	BG_BrightWarriorEntity* CreateBrightWarriorEntity(const hkvVec3& position);
	BG_DarkWarriorEntity* CreateDarkWarriorEntity(const hkvVec3& position);

	// switch to play-the-game mode. When not in vForge, this is default
	void SetPlayTheGame(bool bStatus);

	void SetPlayingTheMoveEnd(bool status) {m_playingTheMoveEnd = status; };

	// access one global instance of the Game manager
	static GameManager& GlobalManager() {return GameManager::g_GameManager;}

	void SetMouseInput(const hkvVec3& mouseInput) { m_mouseInput = mouseInput; }

private:
	void OnBeforeSceneLoaded(char const* sceneFileName);
	void OnAfterSceneLoaded();
	void OnBeforeSceneUnloaded();
	void OnAfterSceneUnloaded();
	void OnUpdateSceneBegin();

	//IVisCallbackHendler_cl
	void OnHandleCallback(IVisCallbackDataObject_cl *callbackData) HKV_OVERRIDE;

	//Used to highlight/dehighlight fields on board
	void HighlightFields();
	void DeHighlightFields();

private:
	VString m_sceneFileName;

	BG_WarriorEntity *m_board[8][8];
	BG_WarriorEntity *m_selectedWarrior;

	hkvVec3 m_mouseInput;

	BG_UIManager* m_UIManager;

	bool m_bPlayingTheGame;
	bool m_playingTheMoveStart;			//true from the moment when warrior starts moving/killing until it finish
	bool m_playingTheMoveEnd;			//true after warrior finish move
	static GameManager g_GameManager;

	BG_GameLogic* m_gameLogic;

	VArray<hkvVec2> m_possibleMoves;	//possible moves for currently selected warrior
	hkvVec2 m_startPosition;
	hkvVec2 m_endPosition;

	VArray<VisBaseEntity_cl*> m_highlights;

	bool m_whiteNext;					//true if white player plays next, false if it's black
};


#endif // PLUGINMANAGER_H_INCLUDED

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
