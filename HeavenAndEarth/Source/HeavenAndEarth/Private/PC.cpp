#include "PC.h"
#include "Game.h"
#include "LoginSave.h"
#include "Kismet/GameplayStatics.h"
#include "UnrealNetwork.h"
#include "..\Public\PC.h"

void APC::BeginPlay() {
	bShowMouseCursor = true;
	Super::BeginPlay();
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
	if (HasAuthority()) AGame::addPC(this);
}

void APC::hostInit()
{
	if (IsLocalController()) {
		isGM = true;
		onHostInit();
	}
}

void APC::playerInit()
{
	onPlayerInit();
}

void APC::selectUnit(AUnit* u)
{
	if (!IsValid(u) || u == selected) return;
	if (!isGM && !u->isMyAccount(account)) return;
	if (selected) {
		selected->unselect();
		onUnselect();
	}
	selected = u;
	u->select();
	updateMoveMarkers();
	updatePathMarkers(selected->PATH);
	onSelect(u);
}

void APC::unselect()
{
	if (!selected) return;
	selected->unselect();
	onUnselect();
	selected = nullptr;
	updateMoveMarkers();
	for (auto i : pathMarkers) i->hide();
}

void APC::setCursor(const FGridIndex& pos)
{
	if (IsValid(cursorMarker)) cursorMarker->setPos(pos);
}

//void APC::onEndTurn_Implementation()
//{
//	updateMoveMarkers();
//}
//
//void APC::onBeginTurn_Implementation()
//{
//	updateMoveMarkers();
//}

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
	if (username == "") printMsg("Kicked: Username cannot be empty");
	if (password == "") printMsg("Kicked: Password cannot be empty");
	if (!AGame::canLogin(this, username, password)) printMsg("Kicked: Incorrect password");
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
	if (!canCommand(selected)) hideMoveMarkers();
	else {
		setMoveMarkers(selected->getPossibleMoves());
	}
}

bool APC::tryMoveSelected(const FGridIndex& destination)
{
	if (!AGame::isValidPos(destination) || !canOrder(selected)) {
		return false;
	}
	/*HexDirection dir;
	if (!AGame::gridIndicesToHexDirection(selected->getFinalPosition(), destination, dir)) {
		return false;
	}
	auto orders = AGame::getRotationOrdersTo(selected->getFinalDirection(), dir);
	orders.Emplace(Order::Move);*/
	auto orders = selected->getOrdersToMoveTo(destination);
	if (orders.Num() == 0) return false;
	orderUnit(selected, orders);
	return true;
}

bool APC::tryResetOrders()
{
	if (!canOrder(selected)) return false;
	resetOrders(selected);
	return true;
}

void APC::printMsg_Implementation(const FString& msg)
{
	debugFstr(msg);
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

}

void APC::endTurn()
{

}

bool APC::canCommand(const AUnit* unit) const
{
	if (!unit) return false;
	else return unit->isAcceptingCommandsFrom(this);
}

bool APC::canOrder(const AUnit* unit) const
{
	if (!unit) return false;
	else return unit->isAcceptingOrdersFrom(this);
}

void APC::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME(APC, account);
}

bool APC::getIsGM() const
{
	return isGM;
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

bool APC::orderUnit_Validate(AUnit* u, const TArray<Order>& orders)
{
	return true;
}

void APC::orderUnit_Implementation(AUnit* u, const TArray<Order>& orders)
{
	if (!u) return;
	else if (!u->acceptsOrders(orders, this)) return;
	else u->addOrders(orders);
}

bool APC::resetOrders_Validate(AUnit* u) {
	return true;
}

void APC::resetOrders_Implementation(AUnit* u)
{
	if (!canOrder(u)) return;
	else u->resetOrders();
}
