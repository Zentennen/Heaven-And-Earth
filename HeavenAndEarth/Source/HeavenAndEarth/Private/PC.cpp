#include "PC.h"
#include "Game.h"
#include "LoginSave.h"
#include "Kismet/GameplayStatics.h"
#include "UnrealNetwork.h"
#include "..\Public\PC.h"

void APC::BeginPlay() {
	bShowMouseCursor = true;
	if (IsValid(markerClass) && IsLocalController()) {
		cursorMarker = GetWorld()->SpawnActor<AHexMarker>(markerClass);
		cursorMarker->setType(MarkerType::Cursor);
		for (uint8 i = 0; i < 6; i++) {
			auto m = GetWorld()->SpawnActor<AHexMarker>(markerClass);
			moveMarkers.Emplace(m);
			m->setType(MarkerType::Move);
			m->hide();
		}
	}
	else if (!IsValid(markerClass)) debugStr("APC::BeginPlay(): Invalid markerClass");
	Super::BeginPlay();
	if (HasAuthority()) {
		if (IsLocalController()) {
			isGM = true;
			hostInit();
		}
		AGame::addPC(this);
	}
}

bool APC::canCommandUnit(AUnit* u) const
{
	if (!u) return false;
	else if (AGame::isExecuting()) return false;
	else if (!u->isMyAccount(account) && !isGM) return false;
	else return true;
}

void APC::selectUnit(AUnit* u)
{
	if (!u || u == unit) return;
	if (!canCommandUnit(u)) return;
	if (unit) {
		unit->unselect();
		onUnselect();
	}
	unit = u;
	u->select();
	updateMoveMarkers();
	updatePathMarkers(unit->path);
	onSelect(u);
}

void APC::unselect()
{
	if (!unit) return;
	unit->unselect();
	onUnselect();
	unit = nullptr;
	updateMoveMarkers();
	for (auto i : pathMarkers) i->hide();
}

void APC::setCursor(const FGridIndex& pos)
{
	if (IsValid(cursorMarker)) cursorMarker->setPos(pos);
}

bool APC::requestAccount_Validate(const FString& username, const FString& password)
{
	return username != "" && password != "" && AGame::canLogin(this, username, password);
}

void APC::requestAccount_Implementation(const FString& username, const FString& password)
{
	if (AGame::createAccount(this, username, password)) returnLoginResult(LoginResult::Success);
}

bool APC::tryLogin_Validate(const FString& username, const FString& password)
{
	return username != "" && password != "" && AGame::canLogin(this, username, password);
}

void APC::tryLogin_Implementation(const FString& username, const FString& password)
{
	returnLoginResult(AGame::login(this, username, password));
}

void APC::returnLoginResult_Implementation(LoginResult result)
{
	onLoginAttempt(result);
}

void APC::setMoveMarkers(const TArray<FGridIndex>& positions)
{
	for (uint8 i = 0; i < moveMarkers.Num(); i++) {
		if (positions.Num() > i) {
			moveMarkers[i]->setPos(positions[i]);
			moveMarkers[i]->show();
		}
		else {
			moveMarkers[i]->hide();
		}
	}
}

void APC::hideMoveMarkers()
{
	for (auto i : moveMarkers) i->hide();
}

void APC::updateMoveMarkers()
{
	if (!canCommandUnit(unit)) hideMoveMarkers();
	else setMoveMarkers(unit->getPossibleMoves());
}

void APC::initialize()
{
	onInit();
}

bool APC::createUnit(FString name, AAccount* acc, FUnitStats stats, FGridIndex position, HexDirection direction)
{
	if (!HasAuthority() || !AGame::isValidPos(position)) return false;
	AUnit* u = nullptr;
	spawnUnit(u, AGame::gridIndexToVector(position), AGame::hexDirectionToRotation(direction));
	if (!u) return false;
	u->init(name, acc, stats, position, direction);
	return true;
}

void APC::requestLogin_Implementation()
{
	auto save = Cast<ULoginSave, USaveGame>(UGameplayStatics::LoadGameFromSlot("Login", 0));
	FString username;
	FString password;
	if (save) {
		username = save->username;
		password = save->password;
	}
	tryLogin(username, password);
}

void APC::beginTurn()
{
	updateMoveMarkers();
}

void APC::endTurn()
{
	updateMoveMarkers();
}

void APC::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME(APC, account);
}

void APC::updatePathMarkers(const TArray<FGridIndex>& newPath)
{
	if (!IsValid(markerClass) || !IsLocalController()) return;
	uint8 i = 0;
	for (; i < newPath.Num(); i++) {
		if (i < pathMarkers.Num()) {
			pathMarkers[i]->setPos(newPath[i]);
			pathMarkers[i]->show();
		}
		else {
			auto m = GetWorld()->SpawnActor<AHexMarker>(markerClass);
			m->setPos(newPath[i]);
			pathMarkers.Emplace(m);
			m->setType(MarkerType::Path);
		}
	}
	for (; i < pathMarkers.Num(); i++) {
		pathMarkers[i]->hide();
	}
}

bool APC::moveCommand_Validate(AUnit* u, FGridIndex destination)
{
	return true;
}

void APC::moveCommand_Implementation(AUnit* u, FGridIndex destination)
{
	if (!canCommandUnit(u)) return;
	u->queueMove(destination);
}