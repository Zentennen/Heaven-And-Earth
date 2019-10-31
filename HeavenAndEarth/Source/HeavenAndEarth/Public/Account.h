// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Account.generated.h"

UENUM(BlueprintType)
enum class LoginResult : uint8 {
	Success, Fail, NotRegistered
};

UCLASS()
class HEAVENANDEARTH_API AAccount : public AActor
{
	GENERATED_BODY()
	
public:	
	AAccount();

protected:
	virtual void BeginPlay() override;

public:	
	UPROPERTY(BlueprintReadWrite, EditAnywhere) FString username;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) FString password;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
	LoginResult canLogin(const FString& username, const FString& password);
};
