#include "GM.h"
#include "PC.h"
#include "Config.h"
#include "Engine.h"

void AGM::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	auto world = GetWorld();
	if (world) {
		if (NewPlayer == world->GetFirstPlayerController()) return;
	}
	auto pc = Cast<APC, APlayerController>(NewPlayer);
	if (!pc) {
		debugStr("AGN::PostLogin(): Could not cast to APC");
		return;
	}
	pc->requestLogin();
}