#ifndef BG_GAME_LOGIC_H__
#define BG_GAME_LOGIC_H__

#include <BaseTyps.h>

class BG_GameLogic : public VRefCounter
{
public:
	BG_GameLogic();

	//returns array of possible moves for currently selected player
	virtual VArray<hkvVec2> PossibleMoves(hkvVec2& from, bool whitePlayer) = 0;

	//returns array of targets that should be eliminated from board
	//since derived classes should implement their own representation of board state(aside of matrix representation given in GameManager),
	//state should be alterd inside this function
	virtual VArray<hkvVec2> PlayMove(hkvVec2& from, hkvVec2& to, bool whitePlayer) = 0;

	//returns -1 if not over, 0 if it's tie, 1 if p1 wins and 2 if p2 wins
	virtual int GameOver() = 0;
};

#endif

//TODO: vidi gde ces da cuvas podatke o tome ko od igraca je komp
//mozes da ih cuvas ovde i iniciras ih kad kreiras klasu -> menadzer nema pojma da postoji AI
//mozes u menadzeru -> menadzer poziva direktno AI funkcije

