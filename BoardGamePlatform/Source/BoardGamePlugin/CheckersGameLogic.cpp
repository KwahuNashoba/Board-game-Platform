#include "BoardGamePluginPCH.h"
#include "CheckersGameLogic.h"
#include "BoardState.h"

CheckersGameLogic::CheckersGameLogic()
	:BG_GameLogic()
{
	m_gameBoard = BG_BoardState<board>::GetInstance();
	m_gameBoard->wp = CHECKERS_WHITEPLAYER_INIT;
	m_gameBoard->bp = CHECKERS_BLACKPLAYER_INIT;
	m_gameBoard->k = (unsigned int)0;
}

VArray<hkvVec2> CheckersGameLogic::PlayMove(hkvVec2& const from, hkvVec2& const to, bool whitePlayer)
{
	//TODO: smisli kako ces da implementiras pretvaranje u kralja ;)
	
	unsigned int fromBitboard = BitBoardHelper::MovesToBitboard(from);
	unsigned int toBitboard = BitBoardHelper::MovesToBitboard(to);
	
	unsigned int targetBitboard = BitBoardHelper::GetTarget(fromBitboard, toBitboard);
	
	unsigned int kingsMask = CHECKERS_KINGROWS_MASK;

	if(fromBitboard & m_gameBoard->k)				//if piece is king
	{
		m_gameBoard->k ^= fromBitboard;		//romeve from current position
		m_gameBoard->k |= toBitboard;			//move to new position			
	}
	else if(kingsMask & toBitboard)			//if it should become king
		m_gameBoard->k |= toBitboard;

	if(whitePlayer)
	{
		//move it
		m_gameBoard->wp ^= fromBitboard;
		m_gameBoard->wp |= toBitboard;
			
		//remove target
		m_gameBoard->bp ^= targetBitboard;
	}
	else
	{
		m_gameBoard->bp ^= fromBitboard;
		m_gameBoard->bp |= toBitboard;

		m_gameBoard->wp ^= targetBitboard;
	}

	//if you killed the king
	if(targetBitboard & m_gameBoard->k)
	{
		//bury him with honors <O
		m_gameBoard->k ^= targetBitboard;
	}

	return BitBoardHelper::BitboardToMoves(targetBitboard);
}

VArray<hkvVec2> CheckersGameLogic::PossibleMoves(hkvVec2& const from, bool whitePlayer)
{
	unsigned int moves, anyJump;

	unsigned int fromBitboard = BitBoardHelper::MovesToBitboard(from);
	
	//if player can jump, player must jump - rule of the game
	//check if selected warrior can jump
	moves = BitBoardHelper::PossibleJumpsBitboard(*m_gameBoard, fromBitboard, whitePlayer);
	//check if any other warrior for current player can jump
	anyJump = BitBoardHelper::PossibleJumpsBitboard(*m_gameBoard, (whitePlayer ? m_gameBoard->wp : m_gameBoard->bp), whitePlayer);

	if(moves)
	{
		return BitBoardHelper::BitboardToMoves(moves);
	}
	else if(anyJump)
	{
		return BitBoardHelper::BitboardToMoves((unsigned int)0);
	}

	moves = BitBoardHelper::PossibleMovesBitboard(*m_gameBoard, fromBitboard, whitePlayer);

	return BitBoardHelper::BitboardToMoves(moves);
}

int CheckersGameLogic::GameOver()
{
	unsigned int wMoves, bMoves;

	wMoves = BitBoardHelper::PossibleMovesBitboard(*m_gameBoard, m_gameBoard->wp, true) | 
		BitBoardHelper::PossibleJumpsBitboard(*m_gameBoard, m_gameBoard->wp, true);

	bMoves = BitBoardHelper::PossibleMovesBitboard(*m_gameBoard, m_gameBoard->bp, false) |
		BitBoardHelper::PossibleJumpsBitboard(*m_gameBoard, m_gameBoard->bp, false);

	if(wMoves && bMoves)
		return 3;
	else if(wMoves)
		return 1;
	else if(bMoves)
		return 2;
	else
		return 0;
}



VArray<hkvVec2> CheckersGameLogic::PossibleJumps(hkvVec2& from, bool whitePlayer)
{
	return BitBoardHelper::BitboardToMoves(BitBoardHelper::PossibleJumpsBitboard(*m_gameBoard, BitBoardHelper::MovesToBitboard(from), whitePlayer));
}



