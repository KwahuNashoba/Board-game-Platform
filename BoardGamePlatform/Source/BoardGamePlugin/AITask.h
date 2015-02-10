#ifndef BG_GAME_AI_H__
#define BG_GAME_AI_H__

#include "BoardState.h"
#include <Vision\Runtime\Base\System\Threading\ThreadManager\VThreadedTask.hpp>

class hkvVec2;
class VThreadedTask;
class VMenagedThread;

class BG_AITask : public VThreadedTask
{
public:
	//constructor of derived class should get board state instance using BG_BoardState template class
	BG_AITask( int const searchDepth, bool whiteNext);

	void SetSearchDepth(int const newDepth);

	//VThreadedTask overrides
	void Run(VManagedThread *pThread) HKV_OVERRIDE;
	void OnFinished(VManagedThread *pThread) HKV_OVERRIDE;

private:
	//calculates and sets best move into a local variable - this is where custom search algorith should be called
	virtual void GetBestMove(bool const whitePlayer) = 0;
	
	//updates board state - switch current state with the one that search algorithm calculated
	virtual void UpdateBoardState() = 0;

	//Gets from(where) and to(where) position based on the result of custom AI search algorithm
	virtual hkvVec2 GetFrom(bool const whitePlayer) = 0;
	virtual hkvVec2 GetTo(bool const whitePlayer) = 0;

	//calculates targets to be eliminated - same as functions above
	virtual VArray<hkvVec2> GetTargets(hkvVec2 const& const from, hkvVec2 const& const to) = 0;

protected:
	int m_searchDepth;
	bool m_whitePlayer;
};

#endif

