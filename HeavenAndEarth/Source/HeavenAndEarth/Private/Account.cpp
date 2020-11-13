#include "Account.h"
#include "Game.h"
#include "CampaignSave.h"
#include "Engine.h"

AAccount::AAccount()
{
	bReplicates = true;
	bAlwaysRelevant = true;
	bNetLoadOnClient = true;
}

void AAccount::BeginPlay()
{
	Super::BeginPlay();

}

void AAccount::init(const FString& pUsername, const FString& pPassword, const int32& i)
{
	if (id != -1) {
		debugStr("AAccount::init(): account was already initialized");
		return;
	}
	id = i;
	username = pUsername;
	password = pPassword;
}

void AAccount::load(const int32& i, UCampaignSave* saveGame)
{
	id = i;
	username = saveGame->accountUsernames[i];
	password = saveGame->accountPasswords[i];
}

FString AAccount::getUsername() const
{
	return username;
}

void AAccount::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAccount::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{

}

LoginResult AAccount::canLogin(const FString& u, const FString& p)
{
	if (username == u) {
		if (password == p) return LoginResult::Success;
		else return LoginResult::Fail;
	}
	else return LoginResult::NotRegistered;
}

void AAccount::save(UCampaignSave* saveGame)
{
	saveGame->accountUsernames.Emplace(username);
	saveGame->accountPasswords.Emplace(password);
}

FString AAccount::getSaveName() const
{
	return FString(TEXT("Account ")) + FString::FromInt(id) + FString(TEXT(" ")) + AGame::getCampaignName();
}

