#ifndef BG_BITBOARD_HELPER_H__
#define BG_BITBOARD_HELPER_H__

#define CHECKERS_WHITEPLAYER_INIT 4293918720;
#define CHECKERS_BLACKPLAYER_INIT 4095;
#define CHECKERS_KINGROWS_MASK 4026531855;
#define CHECKERS_R3SHIFT_MASK 1886417008;
#define CHECKERS_R5SHIFT_MASK 235802112;
#define CHECKERS_L3SHIFT_MASK 235802126;
#define CHECKERS_L5SHIFT_MASK 7368816;
#define CHECKERS_EVENROWS_MASK 252645135;
#define CHECKERS_ODDRAWS_MASK 4042322160;

//board state representation
struct board {
	unsigned int wp;	//white pieces
	unsigned int bp;	//black pieces
	unsigned int k;		//kings (black & white)
};

class BitBoardHelper
{
public:
	//returns bitboard of possible moves
	static unsigned int PossibleMovesBitboard(board const boardState, unsigned int const from, bool whitePlayer);

	//returns bitboard of possible jumps	
	static unsigned int PossibleJumpsBitboard(board const boardState, unsigned int const from, bool whitePlayer);

	//converts coordinates from X,Y coordinates to big-endian bitboard
	static unsigned int MovesToBitboard(hkvVec2 position);

	//convert bit board to array of hkvVec2
	static VArray<hkvVec2> BitboardToMoves(unsigned int bitBoard);

	//returns oponents piece to be removed after jump
	static unsigned int GetTarget(unsigned int const from, unsigned int const to);

	//made to work with numbers that are power of two only, USE WITH CAUTION
	static int GetPowerOfTwo(unsigned int number);
	
	//returns count of 1s in bitset
	static int CountPopulation(unsigned int bitset);
};
#endif
