#include "BoardGamePluginPCH.h"
#include "BitBoardHelper.h"
#include "GameAI.h"

GameAI::GameAI(int const searchDepth)
{
	m_searchDepth = searchDepth;
}

void GameAI::SetSearchDepth(int const searchDepth)
{
	m_searchDepth = searchDepth;
}
