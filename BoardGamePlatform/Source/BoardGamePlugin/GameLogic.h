#ifndef BG_GAME_LOGIC_H__
#define BG_GAME_LOGIC_H__

#include <BaseTyps.h>

class BG_GameLogic : public VRefCounter
{
public:
	//constructor of derived class should initiate board structure and its start state
	//using BG_BoardState template class
	BG_GameLogic();

	//returns array of possible moves for currently selected player
	virtual VArray<hkvVec2> PossibleMoves(hkvVec2& const from, bool whitePlayer) = 0;

	//returns array of targets that should be eliminated from board
	//since derived classes should implement their own representation of board state(aside of matrix representation given in GameManager),
	//state should be alterd inside this function
	virtual VArray<hkvVec2> PlayMove(hkvVec2& const from, hkvVec2& const to, bool whitePlayer) = 0;

	//returns 3(11) if not over, 0(00) if it's tie, 1(01) if bright wins and 2(10) if dark wins
	virtual int GameOver() = 0;
};

#endif

