#pragma once
#include <cstdint>
#include "defs.hpp"


struct Move 
{
	uint16_t from : 6;		//0-63 <=> a1-h8
	uint16_t to : 6;		//0-63 <=> a1-h8
	uint16_t flags : 4;		//0-15 flagi

	Move() : from(0), to(0), flags(0) {}
	
	Move(uint8_t from, uint8_t to, uint8_t flags = FLAG_NORMAL)
		: from(from), to(to), flags(flags) {
	}

	bool operator==(const Move& other) const {
		return (from == other.from &&
			to == other.to &&
			flags == other.flags);
	}

};


struct UndoInfo 
{
	uint64_t zobristKey;

	uint8_t capturePiece;		//zbity pion WHITE_PAWN ITD.
	uint8_t epCapturedSquare;	//pole na którym sta³ pion zbity przez epPassant

	uint8_t castlingRights;		//prawa roszady
	uint8_t enPassantSquare;	//0-63 na jakim polu jest prawo do epPassnat

	uint8_t halfmoveClock;	
};