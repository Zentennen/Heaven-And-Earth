#include "Armor.h"
#include "..\Public\Armor.h"

void FArmorLayer::damage(FArmorLayer layer, uint32 dmg)
{
	layer.condition -= dmg;
	if (layer.condition < 0) layer.condition = 0;
	layer.protection = layer.baseProtection * layer.condition / layer.maxCondition;
}

void FArmorLayer::repair(FArmorLayer layer, uint32 rep)
{
	layer.condition += rep;
	if (layer.condition > layer.maxCondition) layer.condition = layer.maxCondition;
	layer.protection = layer.baseProtection * layer.condition / layer.maxCondition;
}

void FArmorLayer::repairPercentage(FArmorLayer layer, uint8 percent)
{
	repair(layer, layer.maxCondition * percent / 100);
}
