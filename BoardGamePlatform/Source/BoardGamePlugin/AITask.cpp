#include "BoardGamePluginPCH.h"
#include "BitBoardHelper.h"
#include "AITask.h"
#include "GameManager.h"
#include "WarriorEntity.h"
#include "ControllerComponent.h"

BG_AITask::BG_AITask(int const searchDepth, bool whiteNext)
	: VThreadedTask()
{
	m_searchDepth = searchDepth;
	m_whitePlayer = whiteNext;
}

void BG_AITask::SetSearchDepth(int const searchDepth)
{
	m_searchDepth = searchDepth;
}

void BG_AITask::Run(VManagedThread *pThread)
{
	VArray<hkvVec2> targetCoords;
	VArray<BG_WarriorEntity*> targets;

	//calculate best move - should put result in derived class state
	GetBestMove(m_whitePlayer);

	//set this position based on result from function above
	hkvVec2 from = GetFrom(m_whitePlayer);
	hkvVec2 to = GetTo(m_whitePlayer);

	GameManager::GlobalManager().SetStartPosition(from);
	GameManager::GlobalManager().SetEndPosition(to);

	//get targets
	targetCoords = GetTargets(from, to);

	//update board state
	UpdateBoardState();

	BG_WarriorEntity* warrior = GameManager::GlobalManager().GetWarrior(from.x, from.y);
	
	warrior->GetControllerComponent()->SetTargetPoint(
		hkvVec3((to.x + 0.5)*BG_WARRIOR_MODEL_WIDTH,
				(to.y + 0.5)*BG_WARRIOR_MODEL_WIDTH,
				warrior->GetPosition().z));

	for(int i = 0; i<targetCoords.GetSize(); i++)
	{
		hkvVec2 targetCoord = targetCoords.GetAt(i);
		targets.Add(GameManager::GlobalManager().GetWarrior(targetCoord.x,targetCoord.y));
	}

	warrior->GetControllerComponent()->SetTargets(targets);
}

void BG_AITask::OnFinished(VManagedThread *pThread)
{
	this->DisposeObject();
}