#pragma once
#include <cstdint>
#include "defs.hpp"

struct Move
{
	uint8_t from;		//0-63 <=> a1-h8
	uint8_t to;			//0-63 <=> a1-h8
	uint8_t piece;		//ruch
	uint8_t captured;	//zbito 0-11 , 
	uint8_t promotion;

	Move()
		: from(0), to(0), piece(NO_PIECE), captured(0), promotion(0) {
	}

	Move(uint8_t from, uint8_t to, uint8_t piece,
		uint8_t captured = NO_PIECE, uint8_t promotion = 0)
		: from(from), to(to), piece(piece), captured(captured), promotion(promotion) {
	}
};