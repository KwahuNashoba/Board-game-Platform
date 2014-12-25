#ifndef BG_GAME_AI_H__
#define BG_GAME_AI_H__

class GameAI : public VRefCounter
{
public:
	GameAI( int const searchDepth);

	virtual VArray<hkvVec2> PlayMove(board boardState, hkvVec2& const from,hkvVec2& const to, bool whitePlayer) = 0;

	void SetSearchDepth(int const newDepth);

protected:
	int m_searchDepth;
};

#endif

