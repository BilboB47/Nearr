#pragma once
#include <cstdint>
#include "defs.hpp"

struct Move
{
	uint32_t from : 6;		//0-63 <=> a1-h8
	uint32_t to : 6;			//0-63 <=> a1-h8
	uint32_t piece : 4;		//ruch
	uint32_t captured : 4;	//zbito 0-11 , 
	uint32_t flags : 4; //flagi 
	uint32_t promotion : 4; //promocja na ....    

	Move()
		: from(0), to(0), piece(NO_PIECE), captured(0), flags(0), promotion(0) {
	}

	Move(uint8_t from, uint8_t to, uint8_t piece,
		uint8_t captured = NO_PIECE, uint8_t flags = 0, uint8_t promotion = 0)
		: from(from), to(to), piece(piece), captured(captured), flags(flags), promotion(promotion){
	}
};