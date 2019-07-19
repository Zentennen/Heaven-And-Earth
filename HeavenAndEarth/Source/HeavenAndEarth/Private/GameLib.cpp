#include "GameLib.h"
#include "..\Public\GameLib.h"

int32 GameLib::roll(uint8 num, uint8 max)
{
	int32 result = 0;
	for (uint8 i = 0; i < num; i++) {
		result += FMath::RandRange(1, max);
	}
	return result;
}
