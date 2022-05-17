#pragma once
#include <Windows.h>

namespace StaticOffsets {
	uintptr_t OwningGameInstance = 0x1a8;
	uintptr_t LocalPlayers = 0x38;
	uintptr_t PlayerController = 0x30;
	uintptr_t PlayerCameraManager = 0x328;
	uintptr_t AcknowledgedPawn = 0x310;
	uintptr_t PrimaryPickupItemEntry = 0x2f8;
	uintptr_t ItemDefinition = 0x18;
	uintptr_t DisplayName = 0x90;
	uintptr_t Tier = 0x74;
	uintptr_t WeaponData = 0x3d8;
	uintptr_t LastFireTime = 0xa70;
	uintptr_t LastFireTimeVerified = 0xa74;
	uintptr_t LastFireAbilityTime = 0xd60;
	uintptr_t CurrentWeapon = 0x790;
	uintptr_t bADSWhileNotOnGround = 0x4805;

	uintptr_t Levels = 0x160;
	uintptr_t PersistentLevel = 0x30;
	uintptr_t AActors = 0x98;
	uintptr_t ActorCount = 0xA0;

	uintptr_t RootComponent = 0x188;
	uintptr_t FireStartLoc = 0x9f0;
	uintptr_t RelativeLocation = 0x128;
	uintptr_t RelativeRotation = 0x140;
	uintptr_t PlayerState = 0x290;
	uintptr_t Mesh = 0x2f0;
	uintptr_t TeamIndex = 0x1010;
}


struct FZiplinePawnState {
	struct AFortAthenaZiplineBase* Zipline; // 0x00(0x08)
	struct AFortAthenaZiplineBase* PreviousZipline; // 0x00(0x08)
	struct UPrimitiveComponent* InteractComponent; // 0x10(0x08)
	bool bIsZiplining; // 0x18(0x01)
	bool bJumped; // 0x19(0x01)
	bool bReachedEnd; // 0x1a(0x01)
	char pad_1B[0x1]; // 0x1b(0x01)
	int32_t AuthoritativeValue; // 0x1c(0x04)struct sdk::structs::FVector SocketOffset; // 0x20(0x0c)
	float TimeZipliningBegan; // 0x2c(0x04)
	float TimeZipliningEndedFromJump; // 0x30(0x04)
	char pad_34[0x4]; // 0x34(0x04)
};