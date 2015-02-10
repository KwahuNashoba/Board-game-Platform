#include "BoardGamePluginPCH.h"
#include "BitBoardHelper.h"
#include "CheckersAITask.h"

CheckersAITask::CheckersAITask(int const searchDepth, bool whiteNext)
	: BG_AITask(searchDepth, whiteNext)
{
	m_gameBoard = BG_BoardState<board>::GetInstance();
}

void CheckersAITask::GetBestMove(bool const whitePlayer)
{
	m_bestMove = MinMax(*m_gameBoard, m_searchDepth, whitePlayer);
}

void CheckersAITask::UpdateBoardState()
{
	*m_gameBoard = m_bestMove;
}

hkvVec2 CheckersAITask::GetFrom(bool const whitePlayer)
{
	unsigned int from, diff;

	if(whitePlayer)
	{
		diff = m_gameBoard->wp ^ m_bestMove.wp;

		from = m_gameBoard->wp & diff;
	}
	else
	{
		diff = m_gameBoard->bp ^ m_bestMove.bp;

		from = m_gameBoard->bp & diff;
	}

	return BitBoardHelper::BitboardToMoves(from).GetAt(0);
}

hkvVec2 CheckersAITask::GetTo(bool const whitePlayer)
{
	unsigned int to, diff;

	if(whitePlayer)
	{
		diff = m_gameBoard->wp ^ m_bestMove.wp;

		to = m_bestMove.wp & diff;
	}
	else
	{
		diff = m_gameBoard->bp ^ m_bestMove.bp;

		to = m_bestMove.bp & diff;
	}

	return BitBoardHelper::BitboardToMoves(to).GetAt(0);
}

VArray<hkvVec2> CheckersAITask::GetTargets(hkvVec2 const& const from, hkvVec2 const& const to)
{
	return BitBoardHelper::BitboardToMoves(
		BitBoardHelper::GetTarget(BitBoardHelper::MovesToBitboard(from), BitBoardHelper::MovesToBitboard(to)));
}

board CheckersAITask::MinMax(board const state, int const searchDepth, bool const whitePlayer)
{
	board bestMove;
	VArray<board> children;

	int alpha = -2147483648;
	int beta = 2147483647;
	int value;
	
	children = GetChildren(state, whitePlayer);

	if(whitePlayer)
	{
		for(int i = 0; i< children.GetSize(); i++)
		{
			value = Min(children.GetAt(i), 0, searchDepth, alpha, beta);
			if(alpha < value)
			{
				alpha = value;
				bestMove = children.GetAt(i);
			}
		}
		return bestMove;
	}
	else
	{
		for(int i = 0; i< children.GetSize(); i++)
		{
			value = Max(children.GetAt(i), 0, searchDepth, alpha, beta);
			if(beta > value)
			{
				beta = value;
				bestMove = children.GetAt(i);
			}
		}
		return bestMove;
	}
}
int CheckersAITask::Max(board const state, int currentDepth, int const maxDepth, int alpha, int beta)
{
	int value;

	VArray<board> children = GetChildren(state, false);

	if(currentDepth == maxDepth || children.GetSize() == 0)
	{
		return Evaluate(state);
	}
	
	for(int i = 0; i< children.GetSize(); i++)
	{
		value = Min(children.GetAt(i), currentDepth + 1, maxDepth, alpha, beta);
		alpha = alpha >= value ? alpha : value;
		if(alpha >= beta)	//prune test
		{
			return beta;	//prune
		}
	}
	return alpha;
}
int CheckersAITask::Min(board const state, int currentDepth, int const maxDepth, int alpha, int beta)
{	
	int value;

	VArray<board> children = GetChildren(state, false);

	if(currentDepth == maxDepth || children.GetSize() == 0)
	{
		return Evaluate(state);
	}
	
	for(int i = 0; i< children.GetSize(); i++)
	{
		value = Max(children.GetAt(i), currentDepth + 1, maxDepth, alpha, beta);
		beta = beta <= value ? beta : value;
		if(beta <= alpha)	//prune test
		{
			return alpha;	//prune
		}
	}
	return beta;
}

int CheckersAITask::Evaluate(board const state)
{
	int totalCount = BitBoardHelper::CountPopulation(state.wp) - BitBoardHelper::CountPopulation(state.bp);
	int kingsCount = BitBoardHelper::CountPopulation(state.wp & state.k) - BitBoardHelper::CountPopulation(state.bp & state.k);
	int jumpsCount = BitBoardHelper::CountPopulation(BitBoardHelper::PossibleJumpsBitboard(state, state.wp, true))
		- BitBoardHelper::CountPopulation(BitBoardHelper::PossibleJumpsBitboard(state, state.bp, false));

	int value = 1*totalCount + 25*kingsCount + 50*jumpsCount;

	return value;
}

VArray<board> CheckersAITask::GetChildren(board const state, bool const whiteNext)
{
	VArray<board> jumps, moves;
	board child;
	unsigned int from, to, target;
	unsigned int mvs;
	unsigned int kingsMask = CHECKERS_KINGROWS_MASK;
	bool jumpsFlag; //force children generator to cutoff generating moves if there are jumps

	unsigned int pieces = whiteNext ? state.wp : state.bp;

	jumpsFlag = false;

	while(pieces)
	{
		from = (pieces & (pieces - 1)) ^ pieces; //extract piece

		mvs = BitBoardHelper::PossibleJumpsBitboard(state, from, whiteNext); //check for the jumps
		if(mvs)
		{
			jumpsFlag = true; //cutoff moves

			while(mvs)
			{		
				child = state;

				to = (mvs & (mvs - 1)) ^ mvs;	//extract move

				target = BitBoardHelper::GetTarget(from, to);

				if(from & child.k)				//if piece is king
				{
					child.k ^= from;			//romeve from current position
					child.k |= to;				//move to new position			
				}
				else if(kingsMask & to)			//if it should become king
					child.k |= to;

				if(whiteNext)
				{
					//move it
					child.wp ^= from;
					child.wp |= to;
			
					//remove target
					child.bp ^= target;
				}
				else
				{
					child.bp ^= from;
					child.bp |= to;

					child.wp ^= target;
				}

				//if you killed the king
				if(target & child.k)
				{
					//bury him with honors <O
					child.k ^= target;
				}

				mvs = (mvs - 1) & mvs;

				jumps.Append(child);
			}
		}
		else if (!jumpsFlag)
		{
			mvs = BitBoardHelper::PossibleMovesBitboard(state, from, whiteNext);

			while(mvs)
			{		
				child = state;

				to = (mvs & (mvs - 1)) ^ mvs;	//extract move

				if(from & child.k)					//if piece is king
				{
					child.k ^= from;				//romeve from current position
					child.k |= to;					//move to new position			
				}
				else if(kingsMask & to)				//if it should become king
					child.k |= to;

				if(whiteNext)
				{
					//move it
					child.wp ^= from;
					child.wp |= to;
				}
				else
				{
					child.bp ^= from;
					child.bp |= to;
				}

				mvs = (mvs - 1) & mvs;

				moves.Append(child);
			}
		}
		pieces = (pieces - 1) & pieces;
	}

	if(jumps.GetSize() > 0)
	{
		return jumps;
	}
	else
	{
		return moves;
	}
}
