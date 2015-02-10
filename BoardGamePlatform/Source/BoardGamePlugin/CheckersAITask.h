#ifndef BG_AI_H__
#define BG_AI_H__

#include "AITask.h"

class CheckersAITask : public BG_AITask
{
public:
	CheckersAITask(int const searchDepth, bool whiteNext);

private:
	board MinMax(board const state, int const searchDepth, bool const whitePlayer);
	int Max(board const state, int currentDepth, int const maxDepth, int alpha, int beta);
	int Min(board const state, int currentDepth, int const maxDepth, int alpha, int beta);

	int Evaluate(board const state);

	VArray<board> GetChildren(board const state, bool const whiteNext);

	//BG_AITask overrides
	void GetBestMove(bool const whitePlayer) HKV_OVERRIDE;
	void UpdateBoardState() HKV_OVERRIDE;
	hkvVec2 GetFrom(bool const whitePlayer) HKV_OVERRIDE;
	hkvVec2 GetTo(bool const whitePlayer) HKV_OVERRIDE;
	VArray<hkvVec2> GetTargets(hkvVec2 const& const from, hkvVec2 const& const to) HKV_OVERRIDE;

private:
	board* m_gameBoard;
	board m_bestMove;

};
#endif

