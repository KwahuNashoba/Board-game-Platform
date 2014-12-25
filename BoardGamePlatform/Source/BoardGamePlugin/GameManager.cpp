/*
*
* Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
* prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
* Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
*
*/


#include "BoardGamePluginPCH.h"
#include "GameManager.h"
#include "BehaviorWorldListener.h"
#include "WarriorEntity.h"
#include "BrightWarriorEntity.h"
#include "DarkWarriorEntity.h"
#include "ControllerComponent.h"
#include "UIManager.h"
#include "CheckersGameLogic.h"
#include <Vision\Runtime\EnginePlugins\VisionEnginePlugin\Scene\VPrefab.hpp>

GameManager GameManager::g_GameManager;

GameManager::GameManager()
	:m_mouseInput(hkvVec3(-1,-1,-1)),
	m_selectedWarrior(NULL),
	m_whiteNext(true),
	m_playingTheMoveStart(false),
	m_playingTheMoveEnd(false),
	m_startPosition()
{
}
void GameManager::OneTimeInit()
{
	m_bPlayingTheGame = false;
	// the game manager should listen to the following callbacks:
	Vision::Callbacks.OnEditorModeChanged += this;
	Vision::Callbacks.OnBeforeSceneLoaded += this;
	Vision::Callbacks.OnAfterSceneLoaded += this;
	Vision::Callbacks.OnBeforeSceneUnloaded += this;
	Vision::Callbacks.OnAfterSceneUnloaded += this;
	Vision::Callbacks.OnUpdateSceneBegin += this;
	Vision::Callbacks.OnWorldDeInit += this;
}
void GameManager::OneTimeDeInit()
{
	Vision::Callbacks.OnEditorModeChanged -= this;
	Vision::Callbacks.OnBeforeSceneLoaded -= this;
	Vision::Callbacks.OnAfterSceneLoaded -= this;
	Vision::Callbacks.OnBeforeSceneUnloaded -= this;
	Vision::Callbacks.OnAfterSceneUnloaded -= this;
	Vision::Callbacks.OnUpdateSceneBegin -= this;
	Vision::Callbacks.OnWorldDeInit -= this;
}
void GameManager::AddWarrior(BG_WarriorEntity* warrior, int x, int y)
{
	VASSERT(!m_board[x][y]);
	if(m_board[x][y] != NULL)
	{
		m_board[x][y] = warrior;
	}
}
BG_BrightWarriorEntity* GameManager::CreateBrightWarriorEntity(const hkvVec3& position)
{
	BG_BrightWarriorEntity* entity = static_cast<BG_BrightWarriorEntity*>(Vision::Game.CreateEntity("BG_BrightWarriorEntity", position));
	VASSERT(entity);
	return entity;
}
BG_DarkWarriorEntity* GameManager::CreateDarkWarriorEntity(const hkvVec3& position)
{
	BG_DarkWarriorEntity* entity = static_cast<BG_DarkWarriorEntity*>(Vision::Game.CreateEntity("BG_DarkWarriorEntity", position));
	VASSERT(entity);
	return entity;
}
void GameManager::OnHandleCallback(IVisCallbackDataObject_cl *pData)
{
	if (pData->m_pSender==&Vision::Callbacks.OnUpdateSceneBegin)
	{
		//debug info
		ShowDebugOutput();

		OnUpdateSceneBegin();
		//This callback will be triggered at the beginning of every frame
		//You can add your own per frame logic here
		return;
	}
	if (pData->m_pSender==&Vision::Callbacks.OnEditorModeChanged)
	{
		// when vForge switches back from EDITORMODE_PLAYING_IN_GAME, turn off our play the game mode
		if (((VisEditorModeChangedDataObject_cl *)pData)->m_eNewMode != VisEditorManager_cl::EDITORMODE_PLAYING_IN_GAME)
			SetPlayTheGame(false);
		return;
	}
	if (pData->m_pSender==&Vision::Callbacks.OnBeforeSceneLoaded)
	{
		OnBeforeSceneLoaded(static_cast<VisSceneLoadedDataObject_cl*>(pData)->m_szSceneFileName);
		return;
	}
	if (pData->m_pSender==&Vision::Callbacks.OnAfterSceneLoaded)
	{
		OnAfterSceneLoaded();
		//gets triggered when the play-the-game vForge is started or outside vForge after loading the scene
		if (Vision::Editor.IsPlayingTheGame())
			SetPlayTheGame(true);
		return;
	}
	if(pData->m_pSender==&Vision::Callbacks.OnBeforeSceneUnloaded)
	{
		OnBeforeSceneUnloaded();
		return;
	}
	if(pData->m_pSender==&Vision::Callbacks.OnAfterSceneUnloaded)
	{
		OnAfterSceneUnloaded();
		return;
	}
	if (pData->m_pSender==&Vision::Callbacks.OnWorldDeInit)
	{
		// this is important when running outside vForge
		SetPlayTheGame(false);
		m_gameLogic = NULL;
		return;
	}
}


// switch to play-the-game mode
void GameManager::SetPlayTheGame(bool bStatus)
{
  if (m_bPlayingTheGame==bStatus)
    return;

  m_bPlayingTheGame = bStatus;

  if (m_bPlayingTheGame)
  {
     // Play the game mode is started
     Vision::Message.Add(1, "Play the game mode has been started");
  } 
  else
  {
    //the play the game mode has been stopped.
    //clean up all your game specific instances, like e.g. particle effects
    //VisParticleGroupManager_cl::GlobalManager().Instances().Purge();
  }
}

void GameManager::OnBeforeSceneLoaded(char const* sceneFileName)
{
	m_sceneFileName = sceneFileName;

	vHavokBehaviorModule *const havok_behavior_module = vHavokBehaviorModule::GetInstance();
	havok_behavior_module->getBehaviorWorld()->addListener(&BG_BehaviorWorldListener::g_behaviorWorldListener);
}

void GameManager::OnAfterSceneLoaded()
{
	//TODO: view params
	Vision::Callbacks.OnUpdateSceneBegin += this;

	//Initial board state used to put WarriorEntities in the scene
	//Use 'w' for BrightWarriors/white pieces and 'b' for DarkWarriors/black pieces
	const char boardInit [8][8] = {
		{' ','w',' ','w',' ','w',' ','w'},
		{'w',' ','w',' ','w',' ','w',' '},
		{' ','w',' ','w',' ','w',' ','w'},
		{' ',' ',' ',' ',' ',' ',' ',' '},
		{' ',' ',' ',' ',' ',' ',' ',' '},
		{'b',' ','b',' ','b',' ','b',' '},
		{' ','b',' ','b',' ','b',' ','b'},
		{'b',' ','b',' ','b',' ','b',' '}
	};
	
	for(int i = 0; i < 8; i++)
		for(int j = 0; j < 8; j++)
		{
			if(boardInit[i][j] == ' ')
			{
				m_board[i][j] = NULL;
			}
			else if(boardInit[i][j] == 'w')
			{
				float x = i*BG_WARRIOR_MODEL_WIDTH + BG_WARRIOR_MODEL_WIDTH/2;
				float y = j*BG_WARRIOR_MODEL_WIDTH + BG_WARRIOR_MODEL_WIDTH/2;
				float z = 100;
				m_board[i][j] = CreateBrightWarriorEntity(hkvVec3(x, y, z));
			}
			else if(boardInit[i][j] == 'b')
			{
				float x = i*BG_WARRIOR_MODEL_WIDTH + BG_WARRIOR_MODEL_WIDTH/2;
				float y = j*BG_WARRIOR_MODEL_WIDTH + BG_WARRIOR_MODEL_WIDTH/2;
				float z = 100;
				m_board[i][j] = CreateDarkWarriorEntity(hkvVec3(x, y, z));
			}
		}

		m_UIManager = new BG_UIManager();

		m_gameLogic = new CheckersGameLogic();
}

void GameManager::OnBeforeSceneUnloaded()
{
}

void GameManager::OnAfterSceneUnloaded()
{
	Vision::Callbacks.OnUpdateSceneBegin -= this;

	if (!VStringUtil::IsEmpty(m_sceneFileName))
		{
			// Only try to remove the listener if a scene was actually loaded
			vHavokBehaviorModule *const havok_behavior_module = vHavokBehaviorModule::GetInstance();
			havok_behavior_module->getBehaviorWorld()->removeListener(&BG_BehaviorWorldListener::g_behaviorWorldListener);
		}
}

void GameManager::OnUpdateSceneBegin()
{
	if(!m_playingTheMoveStart)
	{
		if(!m_mouseInput.isIdentical(hkvVec3(-1,-1,-1)))
		{
			//grab mouse input
			int x = m_mouseInput.x/BG_WARRIOR_MODEL_WIDTH;
			int y = m_mouseInput.y/BG_WARRIOR_MODEL_WIDTH;

			if(m_selectedWarrior)
			{
				//deselect warrior if the same is clicked again
				if( x == (int)(m_selectedWarrior->GetPosition().x/BG_WARRIOR_MODEL_WIDTH) &&
					y == (int)(m_selectedWarrior->GetPosition().y/BG_WARRIOR_MODEL_WIDTH))
				{
					m_selectedWarrior = NULL;
					DeHighlightFields();
				}
				else
				{
					if(m_possibleMoves.Find(hkvVec2(x,y)) >= 0)
					{
						//start playing the move
						m_playingTheMoveStart = true;

						m_endPosition = hkvVec2(x,y);

						//set final target point
						m_selectedWarrior->GetControllerComponent()->SetTargetPoint(
							hkvVec3((x + 0.5)*BG_WARRIOR_MODEL_WIDTH,
									(y + 0.5)*BG_WARRIOR_MODEL_WIDTH,
									m_selectedWarrior->GetPosition().z)
							);


						//set targets to eliminate
						VArray<BG_WarriorEntity*> targets;
						VArray<hkvVec2> targetCoords = m_gameLogic->PlayMove(
							hkvVec2(m_startPosition.x, m_startPosition.y),
							hkvVec2(x,y),
							m_whiteNext);

						for(int i = 0; i<targetCoords.GetSize(); i++)
						{
							hkvVec2 targetCoord = targetCoords.GetAt(i);
							targets.Add(m_board[(int)targetCoord.x][(int)targetCoord.y]);
							//update board state
							m_board[(int)targetCoord.x][(int)targetCoord.y] = NULL;
						}

						m_selectedWarrior->GetControllerComponent()->SetTargets(targets);

						DeHighlightFields();
					}
				}			
			}
			else if(m_board[x][y])
			{
				//select warrior and set possible moves
				if(m_whiteNext)
				{
					if(m_board[x][y]->GetTypeId() == BG_BrightWarriorEntity::GetClassTypeId())
					{
						m_startPosition = hkvVec2(x,y);

						m_selectedWarrior = m_board[x][y];
						m_possibleMoves = m_gameLogic->PossibleMoves(hkvVec2(x,y), m_whiteNext);
						HighlightFields();
					}
				}
				else
				{
					if(m_board[x][y]->GetTypeId() == BG_DarkWarriorEntity::GetClassTypeId())
					{
						m_startPosition = hkvVec2(x,y);

						m_selectedWarrior = m_board[x][y];
						m_possibleMoves = m_gameLogic->PossibleMoves(hkvVec2(x,y), m_whiteNext);
						HighlightFields();
					}
				}
			}
		}
	}
	//if move is finished, update board state, alow next move to be played and check game status
	else if(m_playingTheMoveEnd)
	{
		//update warrior position on board		
		m_board[(int)m_endPosition.x][(int)m_endPosition.y]	= m_selectedWarrior;
		m_board[(int)m_startPosition.x][(int)m_startPosition.y]	= NULL;

		//deselect warrior
		m_selectedWarrior = NULL;

		//reset move flags
		m_playingTheMoveStart = false;
		m_playingTheMoveEnd = false;

		m_whiteNext = !m_whiteNext;

		//check if the game should end //TODO: ovo mozda prebacis u neku funkciju
		int gameStatus = m_gameLogic->GameOver();
		switch(gameStatus)
		{
			//game over bright wins
			case 1:
				//TODO: pobedio svetli
				m_bPlayingTheGame = false;
				return;
			//game over dark wins
			case 2:
				//TODO:
				m_bPlayingTheGame = false;
				return;
			//game over tie
			case 0:
				//TODO:
				m_bPlayingTheGame = false;
				return;
		}
	}
}


void GameManager::HighlightFields()
{
    VPrefab* highlight;

	m_highlights.RemoveAll();

	VPrefabInstanceInfo info;
    info.m_bOutputInstances = true;

	if(m_whiteNext)
	{
		highlight = VPrefabManager::GlobalManager().LoadPrefab("Prefabs/Highlight_green.vprefab");
	}
	else
	{
		highlight = VPrefabManager::GlobalManager().LoadPrefab("Prefabs/Highlight_blue.vprefab");
	}

	//highlight players position
	info.m_vInstancePos = hkvVec3((m_startPosition.x + 0.5)*BG_WARRIOR_MODEL_WIDTH, (m_startPosition.y + 0.5)*BG_WARRIOR_MODEL_WIDTH, 0.2);
	highlight->Instantiate(info);

	m_highlights.Add(vstatic_cast<VisBaseEntity_cl*>(info.m_Instances[0]));

	//highlight possible moves
	if(m_possibleMoves != NULL)
	{
		for(int i = 0; i < m_possibleMoves.GetSize(); i++)
		{
			info.m_vInstancePos = hkvVec3((m_possibleMoves.GetAt(i).x + 0.5)*BG_WARRIOR_MODEL_WIDTH, (m_possibleMoves.GetAt(i).y + 0.5)*BG_WARRIOR_MODEL_WIDTH, 0.2);
			highlight->Instantiate(info);

			m_highlights.Add(vstatic_cast<VisBaseEntity_cl*>(info.m_Instances[0]));
		}
	}
}

void GameManager::DeHighlightFields()
{
	if(m_highlights.GetSize() > 0)
	{
		for(int i = 0; i < m_highlights.GetSize(); i++)
		{
			m_highlights.GetAt(i)->DisposeObject();
		}

		m_highlights.RemoveAll();
	}
}

void GameManager::ShowDebugOutput()
{
	//print board state
	for(int i = 0; i< 8; i++)
		{
			for(int j = 0; j < 8; j++)
			{
				if(m_board[i][j] != NULL)
					Vision::Message.Print(1,610 + i * 10,(7-j)*10,"%c", m_board[i][j] != NULL ? (m_board[i][j]->GetTypeId() == BG_BrightWarriorEntity::GetClassTypeId() ? 'B' : 'D') : ' ');
			}
		}
	
	//print speed, current state and possible moves
	if(m_selectedWarrior)
	{
		//speed
		Vision::Message.Print(1,50,200, "MoveSpeed: %f", m_selectedWarrior->GetControllerComponent()->GetSpeed());
		
		//current state
		Vision::Message.Print(1,50,250, m_selectedWarrior->GetControllerComponent()->GetCurentStateName());
		
		//possible moves
		if(m_possibleMoves.GetSize() > 0)
		{
			for(int i = 0; i<m_possibleMoves.GetSize(); i++)
			{
				Vision::Message.Print(1, 50, 100+ i*33, "x: %d y: %d", (int)m_possibleMoves.GetAt(i).x, (int)m_possibleMoves.GetAt(i).y);
			}
		}
		else
		{
			Vision::Message.Print(1,50,100, "No possible moves for selected warrior");
		}

		//target
		if(m_selectedWarrior->GetControllerComponent()->GetTarget())
		{
			int x = m_selectedWarrior->GetControllerComponent()->GetTarget()->GetPosition().x/BG_WARRIOR_MODEL_WIDTH;
			int y = m_selectedWarrior->GetControllerComponent()->GetTarget()->GetPosition().y/BG_WARRIOR_MODEL_WIDTH;
			Vision::Message.Print(1,50,300, "Attacking x: %d y: %d", x, y);
		}
	}
	else
	{
		Vision::Message.Print(1,50,100, "No selected warrior");
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