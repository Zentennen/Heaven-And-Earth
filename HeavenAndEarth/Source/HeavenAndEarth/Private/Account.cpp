#include "Account.h"

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
		if (password == p) {
			return LoginResult::Success;
		}
		else return LoginResult::Fail;
	}
	else return LoginResult::NotRegistered;
}

