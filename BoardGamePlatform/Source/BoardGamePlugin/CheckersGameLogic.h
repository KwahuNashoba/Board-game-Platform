#ifndef CHECKERS_GAME_LOGIC_H__
#define CHECKERS_GAME_LOGIC_H__

#include "GameLogic.h"
#include "BitBoardHelper.h"

class CheckersGameLogic : public BG_GameLogic
{
public:
	CheckersGameLogic();

	VArray<hkvVec2> PlayMove(hkvVec2& const from, hkvVec2& const to, bool whitePlayer) HKV_OVERRIDE;

	VArray<hkvVec2> PossibleMoves(hkvVec2& const from, bool whitePlayer) HKV_OVERRIDE;

	int GameOver() HKV_OVERRIDE;

private:
	VArray<hkvVec2> PossibleJumps(hkvVec2& from, bool whitePlayer);		//returns possible jumps	

private:
	board *m_gameBoard;
};

#endif

