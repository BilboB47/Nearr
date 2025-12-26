#include "defs.hpp"

#include <cstdint>
#include <intrin.h> // Obs³uga funkcji typu intrinsics (sprzêtowa akceleracja operacji bitowych)

//============================funkcje pomocnicze do operacji bitowych==============================================
inline uint8_t pop_lsb(uint64_t* bitboard)
{
	unsigned long index;
	if (_BitScanForward64(&index, *bitboard))
	{
		*bitboard &= *bitboard - 1;
		return static_cast<uint8_t>(index);
	}
	return NO_SQUARE;
}
inline uint8_t get_lsb(uint64_t* bitboard)
{
	unsigned long index;
	if (_BitScanForward64(&index, *bitboard)) {
		return static_cast<uint8_t>(index);
	}
	return 64;
}
inline int count_set_bits(uint64_t bb) {
	return (int)__popcnt64(bb);
}
