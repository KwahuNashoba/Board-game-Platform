#include "BoardGamePluginPCH.h"
#include "BitBoardHelper.h"

unsigned int BitBoardHelper::PossibleMovesBitboard(board const boardState, unsigned int const from, bool whitePlayer)
{
	unsigned int l3 = CHECKERS_L3SHIFT_MASK;
	unsigned int l5 = CHECKERS_L5SHIFT_MASK;
	unsigned int r3 = CHECKERS_R3SHIFT_MASK;
	unsigned int r5 = CHECKERS_R5SHIFT_MASK;

	unsigned int possiblePositions;

	unsigned int freePositions = ~(boardState.bp | boardState.wp);
	
	if(whitePlayer)
	{
		possiblePositions = (from >> 4) & freePositions;
		possiblePositions |= ((from & r3) >> 3) & freePositions;
		possiblePositions |= ((from & r5) >> 5) & freePositions;

		if(from & boardState.k)
		{
			possiblePositions |= (from << 4) & freePositions;
			possiblePositions |= ((from & l3) << 3) & freePositions;
			possiblePositions |= ((from & l5) << 5) & freePositions;
		}
	}
	else
	{
		possiblePositions = (from << 4) & freePositions;
		possiblePositions |= ((from & l3) << 3) & freePositions;
		possiblePositions |= ((from & l5) << 5) & freePositions;

		if(from & boardState.k)
		{
			possiblePositions |= (from >> 4) & freePositions;
			possiblePositions |= ((from & r3) >> 3) & freePositions;
			possiblePositions |= ((from & r5) >> 5) & freePositions;
		}
	}

	return possiblePositions;
}

unsigned int BitBoardHelper::PossibleJumpsBitboard(board const boardState, unsigned int const from, bool whitePlayer)
{
	unsigned int jumps = 0;
	unsigned int tmp;

	unsigned int l3 = CHECKERS_L3SHIFT_MASK;
	unsigned int l5 = CHECKERS_L5SHIFT_MASK;
	unsigned int r3 = CHECKERS_R3SHIFT_MASK;
	unsigned int r5 = CHECKERS_R5SHIFT_MASK;

	unsigned int freePositions = ~(boardState.bp | boardState.wp);
	if(whitePlayer)
	{
		//check jumps in one direction
		tmp = (from >> 4) & boardState.bp;
		if(tmp)		//if there are black pieces that can be jumped over
			jumps |= (((tmp & r3) >> 3) | ((tmp & r5) >> 5)) & freePositions;
		//check jumps in other direction
		tmp = (((from & r3) >> 3) | ((from & r5) >> 5)) & boardState.bp;
		if(tmp)
			jumps |= (tmp >> 4) & freePositions;
		//check white king(s)
		unsigned int kings = from & boardState.k;
		if(kings)
		{
			tmp = (kings << 4) & boardState.bp;
			if(tmp)
				jumps |= (((tmp & l3) << 3) & freePositions) | (((tmp & l5) << 5) & freePositions);
			tmp = (((kings & l3) << 3) | ((kings & l5) << 5)) & boardState.bp;
			if(tmp)
				jumps |= (tmp << 4) & freePositions;
		}
	}
	else
	{
		tmp = (from << 4) & boardState.wp;
		if(tmp)
			jumps |= (((tmp & l3) << 3) | ((tmp & l5) << 5)) & freePositions;
		tmp = (((from & l3) << 3) | ((from & l5) << 5)) & boardState.wp;
		if(tmp)
			jumps |= (tmp << 4) & freePositions;

		unsigned int kings = from & boardState.k;
		if(kings)
		{
			tmp = (kings >> 4) & boardState.wp;
			if(tmp)
				jumps |= (((tmp & r3) >> 3) | ((tmp & r5) >> 5)) & freePositions;
			tmp = (((kings & r3) >> 3) | ((kings & r5) >> 5)) & boardState.wp;
			if(tmp)
				jumps |= (tmp >> 4) & freePositions;
		}
	}

	return jumps;
}

unsigned int BitBoardHelper::MovesToBitboard(hkvVec2 position)
{
	int x = 7-position.x;	//mirror X coordinate horizonatally
	int y = position.y;
	int degree = 4*x + y/2;
	unsigned int result = 1 << degree;

	return result;
}

VArray<hkvVec2> BitBoardHelper::BitboardToMoves(unsigned int bitBoard)
{
	VArray<hkvVec2> moves;
	int count = 0;
	int x, y;
	while(bitBoard)
	{
		unsigned int pos = bitBoard ^ (bitBoard & (bitBoard-1));	//extracts least significant 1 from bitboard
		bitBoard &= bitBoard - 1;									//removes least significant 1 from bitboard

		//transforms bitboard position into matrix position
		int degree = 0; //degree of number 2
		while(!(pos & 1))
		{
			degree++;
			pos = pos >> 1;
		}
		x = degree / 4;
		if( x%2 == 0) //calculation is different for even and odd rows 
			y = (degree*2)%8;
		else
			y = (degree*2)%8 + 1;

		x = 7 - x;
		moves.Add(hkvVec2(x,y));
	}
	return moves;
}


unsigned int BitBoardHelper::GetTarget(unsigned int const from, unsigned int const to)
{
	//converting  x,y coordinates to power of two will make computing target position easier
	int fromAsPowerOfTwo, toAsPowerOfTwo,targetAsPowerOfTwo = 0;
	//target as bitboard
	unsigned int targetBitboard = 0;

	/*fromAsPowerOfTwo = (7-(int)from.x)*4 + (int)from.y/2%4;
	toAsPowerOfTwo = (7-(int)to.x)*4 + (int)to.y/2%4;*/

	fromAsPowerOfTwo = GetPowerOfTwo(from);
	toAsPowerOfTwo = GetPowerOfTwo(to);

	int tmp = fromAsPowerOfTwo > toAsPowerOfTwo ? fromAsPowerOfTwo - toAsPowerOfTwo : toAsPowerOfTwo - fromAsPowerOfTwo;

	int evenRows = CHECKERS_EVENROWS_MASK;
	int oddRows = CHECKERS_ODDRAWS_MASK;

	if(tmp == 7)
	{
		if((1 << fromAsPowerOfTwo) & evenRows)
		{
			targetAsPowerOfTwo = fromAsPowerOfTwo < toAsPowerOfTwo ? fromAsPowerOfTwo + 3 : fromAsPowerOfTwo - 4;
		}
		else
		{
			targetAsPowerOfTwo = fromAsPowerOfTwo < toAsPowerOfTwo ? fromAsPowerOfTwo + 4 : fromAsPowerOfTwo - 3;
		}
		targetBitboard = 1 << targetAsPowerOfTwo;
	}
	else if(tmp == 9)
	{
		if((1 << fromAsPowerOfTwo) & evenRows)
		{
			targetAsPowerOfTwo = fromAsPowerOfTwo < toAsPowerOfTwo ? fromAsPowerOfTwo + 4 : fromAsPowerOfTwo - 5;
		}
		else
		{
			targetAsPowerOfTwo = fromAsPowerOfTwo < toAsPowerOfTwo ? fromAsPowerOfTwo + 5 : fromAsPowerOfTwo - 4;
		}
		targetBitboard = 1 << targetAsPowerOfTwo;
	}

	return targetBitboard;
}

int BitBoardHelper::GetPowerOfTwo(unsigned int number)
{
	int powerOfTwo = -1;
	
	while(number)
	{
		powerOfTwo++;

		number >>= 1;
	}

	return powerOfTwo;
}

int BitBoardHelper::CountPopulation(unsigned int bitset)
{
	int count = 0;
	
	while(bitset)
	{
		count++;

		bitset &= bitset-1;
	}

	return count;
}