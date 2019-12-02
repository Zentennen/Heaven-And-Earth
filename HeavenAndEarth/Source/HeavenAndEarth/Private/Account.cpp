#include "Account.h"
#include "Game.h"
#include "AccountSave.h"
#include "Engine.h"

AAccount::AAccount()
{
	bReplicates = true;
	bReplicateMovement = true;
	bAlwaysRelevant = true;
	bNetLoadOnClient = true;
}

void AAccount::BeginPlay()
{
	Super::BeginPlay();

}

void AAccount::init(const FString& pUsername, const FString& pPassword)
{
	if (id != -1) {
		debugStr("AAccount::init(): account was already initialized");
		return;
	}
	id = AGame::addAccount(this);
	if (id == -1) {
		debugStr("AAccount::init(): could not add account");
		AGame::removeAccount(this);
		return;
	}
	if (UGameplayStatics::LoadGameFromSlot(getSaveName(), 0)) debugStr("AAccount::init(): save already exists");
	save = Cast<UAccountSave>(UGameplayStatics::CreateSaveGameObject(UAccountSave::StaticClass()));
	username = pUsername;
	password = pPassword;
}

void AAccount::load(const int32& pId)
{
	id = pId;
	save = Cast<UAccountSave>(UGameplayStatics::LoadGameFromSlot(getSaveName(), 0));
	username = save->username;
	password = save->password;
	id = AGame::addAccount(this);
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

void AAccount::saveAccount()
{
	save->username = username;
	save->password = password;
	UGameplayStatics::SaveGameToSlot(save, getSaveName(), 0);
}

FString AAccount::getSaveName() const
{
	return FString(TEXT("Account ")) + FString::FromInt(id) + FString(TEXT(" ")) + AGame::getCampaignName();
}

