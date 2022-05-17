#pragma once
#define RVA(addr, size) ((uintptr_t)((UINT_PTR)(addr) + *(PINT)((UINT_PTR)(addr) + ((size) - sizeof(INT))) + (size)))
#define M_PI	3.14159265358979323846264338327950288419716939937510
#define _CRT_SECURE_NO_WARNINGS
#define COBJMACROS
#define WIN32_LEAN_AND_MEAN
#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_INDEX_BUFFER 128 * 1024
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT

#define NK_D3D11_IMPLEMENTATION
#define NK_IMPLEMENTATION
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_internal.h"
#include "vector3d.h"
#include "Defines.h"

#include "Offsets.h"
#include "lazyimporter.h"
#include "xor.h"

#include <windows.h>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <time.h>
#include <math.h>
#include <D3D11.h>
#include <codecvt>
#include <Psapi.h>
#include <list>

#include <wchar.h>
#include <tchar.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <ostream>
#include <regex>
#include <winuser.h>
#include <WinReg.h>
#include <winternl.h>

#include <TlHelp32.h>
#include <random>
#include <ctime>
#include <urlmon.h>

#pragma comment(lib, "Psapi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "Advapi32.lib")
#pragma comment(lib, "urlmon.lib")
#pragma comment(lib, "ntdll.lib")

float FOVAngle;

uintptr_t UWorld;
uintptr_t BoneMatrix;
uintptr_t FreeFn;
uintptr_t ProjectWorldToScreen;
uintptr_t LineOfS;
uintptr_t GetNameByIndex;







Vector3 CamLoc;
Vector3 CamRot;

namespace detail
{
	extern "C" void* _spoofer_stub();

	template <typename Ret, typename... Args>
	static inline auto shellcode_stub_helper(
		const void* shell,
		Args... args
	) -> Ret
	{
		auto fn = (Ret(*)(Args...))(shell);
		return fn(args...);
	}

	template <std::size_t Argc, typename>
	struct argument_remapper
	{
		template<
			typename Ret,
			typename First,
			typename Second,
			typename Third,
			typename Fourth,
			typename... Pack
		>
			static auto do_call(const void* shell, void* shell_param, First first, Second second,
				Third third, Fourth fourth, Pack... pack) -> Ret
		{
			return shellcode_stub_helper< Ret, First, Second, Third, Fourth, void*, void*, Pack... >(shell, first, second, third, fourth, shell_param, nullptr, pack...);
		}
	};

	template <std::size_t Argc>
	struct argument_remapper<Argc, std::enable_if_t<Argc <= 4>>
	{
		template<
			typename Ret,
			typename First = void*,
			typename Second = void*,
			typename Third = void*,
			typename Fourth = void*
		>
			static auto do_call(
				const void* shell,
				void* shell_param,
				First first = First{},
				Second second = Second{},
				Third third = Third{},
				Fourth fourth = Fourth{}
			) -> Ret
		{
			return shellcode_stub_helper<
				Ret,
				First,
				Second,
				Third,
				Fourth,
				void*,
				void*
			>(
				shell,
				first,
				second,
				third,
				fourth,
				shell_param,
				nullptr
				);
		}
	};
}
uintptr_t Iamgayaddr = (uintptr_t)LI_FN(GetModuleHandleA)(xorstr("FortniteClient-Win64-Shipping.exe"));

template <typename Ret, typename... Args>
static inline auto SpoofCall(Ret(*fn)(Args...), Args... args) -> Ret
{
	static const void* jmprbx = nullptr;
	if (!jmprbx) {
		const auto ntdll = reinterpret_cast<const unsigned char*>(Iamgayaddr);
		const auto dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(ntdll);
		const auto nt = reinterpret_cast<const IMAGE_NT_HEADERS*>(ntdll + dos->e_lfanew);
		const auto sections = IMAGE_FIRST_SECTION(nt);
		const auto num_sections = nt->FileHeader.NumberOfSections;

		constexpr char section_name[5]{ '.', 't', 'e', 'x', 't' };
		const auto     section = std::find_if(sections, sections + num_sections, [&](const auto& s) {
			return std::equal(s.Name, s.Name + 5, section_name);
			});

		constexpr unsigned char instr_bytes[2]{ 0xFF, 0x26 };
		const auto              va = ntdll + section->VirtualAddress;
		jmprbx = std::search(va, va + section->Misc.VirtualSize, instr_bytes, instr_bytes + 2);
	}

	struct shell_params
	{
		const void* trampoline;
		void* function;
		void* rdx;
	};

	shell_params p
	{
		jmprbx,
		reinterpret_cast<void*>(fn)
	};

	using mapper = detail::argument_remapper<sizeof...(Args), void>;
	return mapper::template do_call<Ret, Args...>((const void*)&detail::_spoofer_stub, &p, args...);
}

namespace SpoofRuntime {
	inline float acosf_(float x)
	{
		return SpoofCall(acosf, x);
	}


	inline float atan2f_(float x, float y)
	{
		return SpoofCall(atan2f, x, y);
	}

	inline float sqrtf_(float x)
	{
		union { float f; uint32_t i; } z = { x };
		z.i = 0x5f3759df - (z.i >> 1);
		z.f *= (1.5f - (x * 0.5f * z.f * z.f));
		z.i = 0x7EEEEEEE - z.i;
		return z.f;
	}


	double powf_(double x, int y)
	{
		double temp;
		if (y == 0)
			return 1;
		temp = powf_(x, y / 2);
		if ((y % 2) == 0) {
			return temp * temp;
		}
		else {
			if (y > 0)
				return x * temp * temp;
			else
				return (temp * temp) / x;
		}
	}

	inline float cosf_(float x)
	{
		return SpoofCall(cosf, x);
	}

	inline float sinf_(float x)
	{
		return SpoofCall(sinf, x);
	}

}



BOOL valid_pointer(DWORD64 address)
{
	if (!IsBadWritePtr((LPVOID)address, (UINT_PTR)8)) return TRUE;
	else return FALSE;
}

template<typename ReadT>
ReadT read(DWORD_PTR address, const ReadT& def = ReadT())
{
	if (valid_pointer(address)) {
		return *(ReadT*)(address);
	}
}

template<typename WriteT>
bool write(DWORD_PTR address, WriteT value, const WriteT& def = WriteT())
{
	if (valid_pointer(address)) {
		*(WriteT*)(address) = value;
		return true;
	}
	return false;
}

uintptr_t PlayerController;
uintptr_t LocalPawn;




#define get_array_size(array)    (sizeof(array) / sizeof(array[0]))
int random_int(int min, int max)
{
	int range = max - min;
	static bool seed = false;
	if (!seed)
	{
		srand((unsigned int)time(0));
		seed = true;
	}
	return rand() % range + min;
}

float distance(int x1, int y1, int x2, int y2)
{
	return (int)SpoofRuntime::sqrtf_((int)SpoofRuntime::powf_(x2 - x1, 2) +
		(int)SpoofRuntime::powf_(y2 - y1, 2) * 1.0);
}







namespace SDK
{
	namespace Structs
	{
		struct FMatrix
		{
			double M[4][4];
		};
		static FMatrix* myMatrix = new FMatrix();


		typedef struct {
			double X, Y, Z;
		} FVector;
		

		struct FVector2D
		{
			FVector2D() : x(0.f), y(0.f)
			{

			}

			FVector2D(double _x, double _y) : x(_x), y(_y)
			{

			}
			~FVector2D()
			{

			}
			double x, y;
		};

		template<class T>
		struct TArray
		{
			friend struct FString;

		public:
			inline TArray()
			{
				Data = nullptr;
				Count = Max = 0;
			};

			inline int Num() const
			{
				return Count;
			};

			inline T& operator[](int i)
			{
				return Data[i];
			};

			inline const T& operator[](int i) const
			{
				return Data[i];
			};

			inline bool IsValidIndex(int i) const
			{
				return i < Num();
			}

		private:
			T* Data;
			int32_t Count;
			int32_t Max;
		};

		class FText {
		private:
			char _padding_[0x28];
			PWCHAR Name;
			DWORD Length;
		public:
			

			inline PWCHAR c_wstr() {
				return Name;
			}

			inline char* c_str()
			{

				char sBuff[255];
				wcstombs((char*)sBuff, (const wchar_t*)this->Name, sizeof(sBuff));
				return sBuff;
			}
		};

		struct FString : private TArray<wchar_t>
		{
			inline FString()
			{
			};

			FString(const wchar_t* other)
			{
				Max = Count = *other ? std::wcslen(other) + 1 : 0;

				if (Count)
				{
					Data = const_cast<wchar_t*>(other);
				}
			};

			inline bool IsValid() const
			{
				return Data != nullptr;
			}

			inline const wchar_t* c_str() const
			{
				return Data;
			}

			std::string ToString() const
			{
				auto length = std::wcslen(Data);

				std::string str(length, '\0');

				std::use_facet<std::ctype<wchar_t>>(std::locale()).narrow(Data, Data + length, '?', &str[0]);

				return str;
			}
		};
	}

	//Functions
	//typedef bool(__fastcall* WorldToScreen_t)(std::uint64_t PlayerController, Vector3 vWorldPos, Vector3* vScreenPos, char);
	//WorldToScreen_t fnWorldToScreen;

	namespace Classes
	{
		class APlayerCameraManager
		{
		public:
			static float GetFOVAngle(uintptr_t PlayerCameraManager)
			{
				auto GetFOVAngle = reinterpret_cast<float(*)(UINT64, char*)>(*(ULONG_PTR*)(*(ULONG_PTR*)PlayerCameraManager + 0x768));
				return SpoofCall(GetFOVAngle, (ULONG_PTR)PlayerCameraManager, (char*)0);
			}

			static Vector3 GetCameraLocation(uintptr_t PlayerCameraManager)
			{
				auto GetCameraLocation = reinterpret_cast<Vector3(*)(UINT64, char*)>(*(ULONG_PTR*)(*(ULONG_PTR*)PlayerCameraManager + 0x7B0));
				return SpoofCall(GetCameraLocation, (ULONG_PTR)PlayerCameraManager, (char*)0);
			}

			static Vector3 GetCameraRotation(uintptr_t PlayerCameraManager)
			{
				auto GetCameraRotation = reinterpret_cast<Vector3(*)(UINT64, char*)>(*(ULONG_PTR*)(*(ULONG_PTR*)PlayerCameraManager + 0x7A8));
				return SpoofCall(GetCameraRotation, (ULONG_PTR)PlayerCameraManager, (char*)0);
			}

			
			static BOOLEAN LineOfSightTo(PVOID PlayerController, PVOID Actor, Vector3* ViewPoint) {

				auto LOSTo = reinterpret_cast<bool(__fastcall*)(PVOID PlayerController, PVOID Actor, Vector3 * ViewPoint)>(LineOfS);

				return SpoofCall(LOSTo, PlayerController, Actor, ViewPoint);
			}

			static void FirstPerson(int h) {

				auto ClientSetCameraMode = (*(void(__fastcall**)(uintptr_t Controller, int h))(*(uintptr_t*)PlayerController + 0x940));                
				return SpoofCall(ClientSetCameraMode, (uintptr_t)PlayerController, h);
			}
			
			static bool GetPlayerViewPoint(uintptr_t PlayerController, Vector3* vCameraPos, Vector3* vCameraRot)
			{
				if (!PlayerController) return false;

				static uintptr_t pGetPlayerViewPoint = 0;
				if (!pGetPlayerViewPoint)
				{
					uintptr_t VTable = *(uintptr_t*)PlayerController;
					if (!VTable)  return false;

					pGetPlayerViewPoint = *(uintptr_t*)(VTable + 0x7A0);
					if (!pGetPlayerViewPoint)  return false;
				}

				auto GetPlayerViewPoint = reinterpret_cast<void(__fastcall*)(uintptr_t, Vector3*, Vector3*)>(pGetPlayerViewPoint);

				SpoofCall(GetPlayerViewPoint, (uintptr_t)PlayerController, vCameraPos, vCameraRot);

				return true;
			}
		};

		class UObject
		{
		public:
			static void FreeObjName(__int64 address)
			{
				auto func = reinterpret_cast<__int64(__fastcall*)(__int64 a1)>(FreeFn);

				SpoofCall(func, address);
			}

			static const char* GetObjectName(uintptr_t Object)
			{
				if (Object == NULL)
					return ("");

				auto fGetObjName = reinterpret_cast<SDK::Structs::FString * (__fastcall*)(int* index, SDK::Structs::FString * res)>(GetNameByIndex);

				//auto heheh = reinterpret_cast<SDK::Structs::FString * (__fastcall*)(int* index, SDK::Structs::FString * res)>(GetNameByIndex);


				int index = *(int*)(Object + 0x18);

				SDK::Structs::FString result;
				SpoofCall(fGetObjName, &index, &result);

				if (result.c_str() == NULL)
					return ("");

				auto result_str = result.ToString();

				if (result.c_str() != NULL)
					FreeObjName((__int64)result.c_str());

				return result_str.c_str();
			}

		};

		class USkeletalMeshComponent
		{
		public:
			static bool GetBoneLocation(uintptr_t CurrentActor, int id, Vector3* out)
			{
				uintptr_t mesh = read<uintptr_t>(CurrentActor + StaticOffsets::Mesh);
				if (!mesh) return false;

				auto fGetBoneMatrix = ((Structs::FMatrix * (__fastcall*)(uintptr_t, Structs::FMatrix*, int))(BoneMatrix));
				SpoofCall(fGetBoneMatrix, mesh, Structs::myMatrix, id);

				out->x = Structs::myMatrix->M[3][0];
				out->y = Structs::myMatrix->M[3][1];
				out->z = Structs::myMatrix->M[3][2];

				return true;
			}

			static Vector3 GetBoneDebug(uintptr_t CurrentActor, int id)
			{
				uintptr_t mesh = read<uintptr_t>(CurrentActor + StaticOffsets::Mesh);
				if (!mesh) return Vector3(0, 0, 0);

				auto fGetBoneMatrix = ((Structs::FMatrix * (__fastcall*)(uintptr_t, Structs::FMatrix*, int))(BoneMatrix));
				SpoofCall(fGetBoneMatrix, mesh, Structs::myMatrix, id);

				Vector3 out;

				out.x = Structs::myMatrix->M[3][0];
				out.y = Structs::myMatrix->M[3][1];
				out.z = Structs::myMatrix->M[3][2];

				return out;
			}

		};



		class AController
		{
		public:
			static bool WorldToScreen(Vector3 WorldLocation, Vector3* out)
			{
				
				//if (!fnWorldToScreen) {
				//	fnWorldToScreen = reinterpret_cast<WorldToScreen_t>(ProjectWorldToScreen);
				//}
				auto WorldToScreen = reinterpret_cast<bool(__fastcall*)(uintptr_t pPlayerController, Vector3 vWorldPos, Vector3 * vScreenPosOut, char)>(ProjectWorldToScreen);

				SpoofCall(WorldToScreen, (uintptr_t)PlayerController, WorldLocation, out, (char)0);

				return true;
			}


			static void SetControlRotation(Vector3 NewRotation, bool bResetCamera = false)
			{
				auto SetControlRotation_ = (*(void(__fastcall**)(uintptr_t Controller, Vector3 NewRotation, bool bResetCamera))(*(uintptr_t*)PlayerController + 0x710));
				SpoofCall(SetControlRotation_, PlayerController, NewRotation, bResetCamera);
			}
		};
	}

	namespace Utils
	{
		static ImVec2 ProjectWorldLocationToScreen(Vector3 world_location)
		{
			if (!FOVAngle) return  { 0, 0 };
			ImVec2 screen_location;
			float delta[3];
			float sp = 0, cp = 0, sy = 0, cy = 0, sr = 0, cr = 0;
			float axisx[3];
			float axisy[3];
			float axisz[3];
			float transformed[3];

			screen_location.x = 0;
			screen_location.y = 0;

			delta[0] = world_location.x - CamLoc.x;
			delta[1] = world_location.y - CamLoc.y;
			delta[2] = world_location.z - CamLoc.z;

			sp = SpoofCall(sinf, float(CamRot.x * M_PI / 180));
			cp = SpoofCall(cosf, float(CamRot.x * M_PI / 180));
			sy = SpoofCall(sinf, float(CamRot.y * M_PI / 180));
			cy = SpoofCall(cosf, float(CamRot.y * M_PI / 180));
			sr = SpoofCall(sinf, float(CamRot.z * M_PI / 180));
			cr = SpoofCall(cosf, float(CamRot.z * M_PI / 180));

			axisx[0] = cp * cy;
			axisx[1] = cp * sy;
			axisx[2] = sp;

			axisy[0] = sr * sp * cy - cr * sy;
			axisy[1] = sr * sp * sy + cr * cy;
			axisy[2] = -sr * cp;

			axisz[0] = -(cr * sp * cy + sr * sy);
			axisz[1] = cy * sr - cr * sp * sy;
			axisz[2] = cr * cp;

			transformed[0] = delta[0] * axisy[0] + delta[1] * axisy[1] + delta[2] * axisy[2];
			transformed[1] = delta[0] * axisz[0] + delta[1] * axisz[1] + delta[2] * axisz[2];
			transformed[2] = delta[0] * axisx[0] + delta[1] * axisx[1] + delta[2] * axisx[2];

			if (transformed[2] < 1)
				transformed[2] = 1;

			if (transformed[2] / 100 > 0)
			{
				float tmpx = 0, tmpy = 0;

				tmpx = (float)(Renderer_Defines::Width / 2);
				tmpy = (float)(Renderer_Defines::Height / 2);

				screen_location.x = tmpx + transformed[0] * tmpx / SpoofCall(tanf, float(FOVAngle * M_PI / 360)) / transformed[2];
				screen_location.y = tmpy - transformed[1] * tmpx / SpoofCall(tanf, float(FOVAngle * M_PI / 360)) / transformed[2];

				return screen_location;
			}

			return { 0, 0 };
		}

		ImVec2 WorldToScreen2(uintptr_t PlayerController, Vector3 WorldLocation)
		{
			if (!valid_pointer(PlayerController)) return { 0, 0 };
			return ProjectWorldLocationToScreen(WorldLocation);
		}

		double GetCrossDistance(double x1, double y1, double x2, double y2)
		{
			return SpoofRuntime::sqrtf_(SpoofRuntime::powf_((float)(x1 - x2), (float)2) + SpoofRuntime::powf_((float)(y1 - y2), (float)2));
		}

		inline float GetDistLength(Vector3 from, Vector3 to)
		{
			return float(SpoofRuntime::sqrtf_(SpoofRuntime::powf_(to.x - from.x, 2.0) + SpoofRuntime::powf_(to.y - from.y, 2.0) + SpoofRuntime::powf_(to.z - from.z, 2.0)));
		}

		Vector3 AimbotPrediction(float bulletVelocity, float bulletGravity, float targetDistance, Vector3 targetPosition, Vector3 targetVelocity) {
			Vector3 recalculated = targetPosition;
			float gravity = fabs(bulletGravity);
			float time = targetDistance / fabs(bulletVelocity);
			float bulletDrop = (gravity / 250) * time * time;
			recalculated.z += bulletDrop * 120;
			recalculated.x += time * (targetVelocity.x);
			recalculated.y += time * (targetVelocity.y);
			recalculated.z += time * (targetVelocity.z);
			return recalculated;
		}

		bool CheckInScreen(uintptr_t CurrentActor, int Width, int Height) {
			Vector3 Pos;
			Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 98, &Pos);
			Classes::AController::WorldToScreen(Pos, &Pos);
			if (CurrentActor)
			{
				if (((Pos.x <= 0 or Pos.x > Width) and (Pos.y <= 0 or Pos.y > Height)) or ((Pos.x <= 0 or Pos.x > Width) or (Pos.y <= 0 or Pos.y > Height))) {
					return false;
				}
				else {
					return true;
				}

			}
		}

		bool CheckItemInScreen(uintptr_t CurrentActor, int Width, int Height) {
			Vector3 Pos;
			//MessageBoxA(NULL, "Before RootComponent", "", MB_OK);
			uintptr_t RootComponent = read<uintptr_t>(CurrentActor + StaticOffsets::RootComponent);
			//std::cout << "TheOmegaLul: " << RootComponent << "\n";
			//MessageBoxA(NULL, "Before RelativeLocation", "", MB_OK);
			//if (!RootComponent) return false;
			Vector3 RelativeLocation = read<Vector3>(RootComponent + StaticOffsets::RelativeLocation);
			//MessageBoxA(NULL, "Before WorldToScreen", "", MB_OK);
			Classes::AController::WorldToScreen(RelativeLocation, &Pos);
			if (CurrentActor)
			{
				//MessageBoxA(NULL, "Before THE OMEGALUL IF", "", MB_OK);
				if (((Pos.x <= 0 or Pos.x > Width) and (Pos.y <= 0 or Pos.y > Height)) or ((Pos.x <= 0 or Pos.x > Width) or (Pos.y <= 0 or Pos.y > Height))) {
					return false;
				}
				else {
					return true;
				}

			}
		}

		

		bool CheckIfInFOV(uintptr_t CurrentActor, float& max)
		{
			Vector3 Pos;
			Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, Settings::aimbone, &Pos);
			Classes::AController::WorldToScreen(Pos, &Pos);
			if (CurrentActor)
			{
				float Dist = GetCrossDistance(Pos.x, Pos.y, (Renderer_Defines::Width / 2), (Renderer_Defines::Height / 2));

				if (Dist < max)
				{
					float Radius = (Settings::FovCircle_Value * Renderer_Defines::Width / FOVAngle) / 2;

					if (Pos.x <= ((Renderer_Defines::Width / 2) + Radius) &&
						Pos.x >= ((Renderer_Defines::Width / 2) - Radius) &&
						Pos.y <= ((Renderer_Defines::Height / 2) + Radius) &&
						Pos.y >= ((Renderer_Defines::Height / 2) - Radius))
					{
						max = Dist;
						return true;
					}

					return false;
				}
			}

			return false;
		}

		Vector3 CalculateNewRotation(uintptr_t CurrentActor, Vector3 LocalRelativeLocation, bool prediction)
		{
			Vector3 RetVector = { 0,0,0 };

			Vector3 rootHead;
			Vector3 Headbox;
			Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, Settings::aimbone, &rootHead);
			Classes::AController::WorldToScreen(Vector3(rootHead.x, rootHead.y, rootHead.z + 20), &Headbox);


			Vector3 calculated;


			if (prediction) {
				float distance = Utils::GetDistLength(LocalRelativeLocation, Headbox) / 250;
				uint64_t CurrentActorRootComponent = read<uint64_t>(CurrentActor + 0x138);
				Vector3 vellocity = read<Vector3>(CurrentActorRootComponent + 0x140);
				Vector3 Predicted = Utils::AimbotPrediction(30000, -504, distance, rootHead, vellocity);

				calculated = Predicted;

			}
			else {
				calculated = rootHead;
			}



			if (calculated.x == 0 && calculated.y == 0) return Vector3(0, 0, 0);

			Vector3 VectorPos = calculated - CamLoc;

			

			float distance = VectorPos.Length();
			
			RetVector.x = -((SpoofRuntime::acosf_(VectorPos.z / distance) * (float)(180.0f / M_PI)) - 90.f);
			RetVector.y = SpoofRuntime::atan2f_(VectorPos.y, VectorPos.x) * (float)(180.0f / M_PI);

			return RetVector;
		}

		Vector3 SmoothAngles(Vector3 rot1, Vector3 rot2)
		{
			Vector3 ret;
			auto currentRotation = rot1;

			ret.x = (rot2.x - rot1.x) / Settings::smooth + rot1.x;
			ret.y = (rot2.y - rot1.y) / Settings::smooth + rot1.y;

			return ret;
		}
	}
}
/*
namespace DiscordHelper
{
	uintptr_t module = (uintptr_t)LI_FN(GetModuleHandleA)(xorstr("DiscordHook64.dll"));

	std::vector<uintptr_t> pCreatedHooksArray;
	bool CreateHook(uintptr_t pOriginal, uintptr_t pHookedFunction, uintptr_t pOriginalCall)
	{
		static uintptr_t addrCreateHook = NULL;

		if (!addrCreateHook)
			addrCreateHook = MemoryHelper::PatternScanW(module, xorstr("41 57 41 56 56 57 55 53 48 83 EC 68 4D 89 C6 49 89 D7"));

		if (!addrCreateHook)
			return false;

		using CreateHook_t = uint64_t(__fastcall*)(LPVOID, LPVOID, LPVOID*);
		auto fnCreateHook = (CreateHook_t)addrCreateHook;

		return SpoofCall(fnCreateHook, (void*)pOriginal, (void*)pHookedFunction, (void**)pOriginalCall) == 0 ? true : false;
	}

	bool EnableHookQue()
	{
		static uintptr_t addrEnableHookQueu = NULL;

		if (!addrEnableHookQueu)
			addrEnableHookQueu = MemoryHelper::PatternScanW(module, xorstr("41 57 41 56 41 55 41 54 56 57 55 53 48 83 EC 38 48 ? ? ? ? ? ? 48 31 E0 48 89 44 24 30 BE 01 00 00 00 31 C0 F0 ? ? ? ? ? ? ? 74 2B"));

		if (!addrEnableHookQueu)
			return false;

		using EnableHookQueu_t = uint64_t(__stdcall*)(VOID);
		auto fnEnableHookQueu = (EnableHookQueu_t)addrEnableHookQueu;

		return SpoofCall(fnEnableHookQueu) == 0 ? true : false;
	}

	short GetAsyncKeyState(int key)
	{
		static uintptr_t GetAsyncKeyState_addr;
		if (!GetAsyncKeyState_addr)
			GetAsyncKeyState_addr = MemoryHelper::PatternScanW(module, xorstr("48 FF ? ? ? ? ? CC CC CC CC CC CC CC CC CC 48 FF ? ? ? ? ? CC CC CC CC CC CC CC CC CC 48 83 EC 28 48 ? ? ? ? ? ? 48 85 C9"));
		if (!GetAsyncKeyState_addr)
			return false;

		auto queuehook = ((short(__fastcall*)(int))(GetAsyncKeyState_addr));
		return SpoofCall(queuehook, key);
	}

	bool EnableHook(uintptr_t pTarget, bool bIsEnabled)
	{
		static uintptr_t addrEnableHook = NULL;

		if (!addrEnableHook)
			addrEnableHook = MemoryHelper::PatternScanW(module, xorstr("41 56 56 57 53 48 83 EC 28 49 89 CE BF 01 00 00 00 31 C0 F0 ? ? ? ? ? ? ? 74"));

		if (!addrEnableHook)
			return false;

		using EnableHook_t = uint64_t(__fastcall*)(LPVOID, bool);
		auto fnEnableHook = (EnableHook_t)addrEnableHook;

		return SpoofCall(fnEnableHook, (void*)pTarget, bIsEnabled) == 0 ? true : false;
	}


	short SetCursorPos(int x, int y)
	{
		static uintptr_t addrSetCursorPos = NULL;

		if (!addrSetCursorPos)
		{
			addrSetCursorPos = MemoryHelper::PatternScanW(module,
				xorstr("8A 05 ? ? ? ? 84 C0 74 12"));
		}

		if (!addrSetCursorPos)
			return false;

		using SetCursorPos_t = short(__fastcall*)(int, int);
		auto fnSetCursorPos = (SetCursorPos_t)addrSetCursorPos;

		return fnSetCursorPos(x, y);
	}

	bool InsertHook(uintptr_t pOriginal, uintptr_t pHookedFunction, uintptr_t pOriginalCall)
	{
		bool bAlreadyCreated = false;
		for (auto _Hook : pCreatedHooksArray)
		{
			if (_Hook == pOriginal)
			{
				bAlreadyCreated = true;
				break;
			}
		}

		if (!bAlreadyCreated)
			bAlreadyCreated = CreateHook(pOriginal, pHookedFunction, pOriginalCall);

		if (bAlreadyCreated)
			if (EnableHook(pOriginal, true))
				if (EnableHookQue())
					return true;

		return false;
	}
}*/


namespace MemoryHelper {



	uintptr_t PatternScanW(uintptr_t pModuleBaseAddress, const char* sSignature, size_t nSelectResultIndex = 0)
	{
		static auto patternToByte = [](const char* pattern)
		{
			auto       bytes = std::vector<int>{};
			const auto start = const_cast<char*>(pattern);
			const auto end = const_cast<char*>(pattern) + SpoofCall(strlen, pattern);

			for (auto current = start; current < end; ++current)
			{
				if (*current == '?')
				{
					++current;
					if (*current == '?')
						++current;
					bytes.push_back(-1);
				}
				else
					bytes.push_back(strtoul(current, &current, 16));
			}
			return bytes;
		};

		const auto dosHeader = (PIMAGE_DOS_HEADER)pModuleBaseAddress;
		const auto ntHeaders = (PIMAGE_NT_HEADERS)((std::uint8_t*)pModuleBaseAddress + dosHeader->e_lfanew);

		const auto sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
		auto       patternBytes = patternToByte(sSignature);
		const auto scanBytes = reinterpret_cast<std::uint8_t*>(pModuleBaseAddress);

		const auto s = patternBytes.size();
		const auto d = patternBytes.data();

		size_t nFoundResults = 0;

		for (auto i = 0ul; i < sizeOfImage - s; ++i)
		{
			bool found = true;

			for (auto j = 0ul; j < s; ++j)
			{
				if (scanBytes[i + j] != d[j] && d[j] != -1)
				{
					found = false;
					break;
				}
			}

			if (found)
			{
				if (nSelectResultIndex != 0)
				{
					if (nFoundResults < nSelectResultIndex)
					{
						nFoundResults++;
						found = false;
					}
					else
						return reinterpret_cast<uintptr_t>(&scanBytes[i]);
				}
				else
					return reinterpret_cast<uintptr_t>(&scanBytes[i]);
			}
		}

		return NULL;
	}

	uintptr_t PatternScan(const char* sSignature, size_t nSelectResultIndex = 0)
	{
		static bool bIsSetted = false;

		static MODULEINFO info = { 0 };

		if (!bIsSetted)
		{

			LI_FN(GetModuleInformation)(LI_FN(GetCurrentProcess)(), LI_FN(GetModuleHandleA)(xorstr("FortniteClient-Win64-Shipping.exe")), &info, sizeof(info));
			bIsSetted = true;
		}
		return SpoofCall(PatternScanW, (uintptr_t)info.lpBaseOfDll, sSignature, nSelectResultIndex);
	}
}


namespace HookHelper {

	void* memcpy_(void* _Dst, void const* _Src, size_t _Size)
	{
		auto csrc = (char*)_Src;
		auto cdest = (char*)_Dst;

		for (int i = 0; i < _Size; i++)
		{
			cdest[i] = csrc[i];
		}
		return _Dst;
	}
	
	uintptr_t DiscordModule = (uintptr_t)LI_FN(GetModuleHandleA)(xorstr("DiscordHook64.dll"));
	std::vector<uintptr_t> pCreatedHooksArray;
	bool CreateHook(uintptr_t pOriginal, uintptr_t pHookedFunction, uintptr_t pOriginalCall)
	{
		static uintptr_t addrCreateHook = NULL;

		if (!addrCreateHook)
			addrCreateHook = MemoryHelper::PatternScanW(DiscordModule, xorstr("41 57 41 56 56 57 55 53 48 83 EC 68 4D 89 C6 49 89 D7"));

		if (!addrCreateHook)
			return false;

		using CreateHook_t = uint64_t(__fastcall*)(LPVOID, LPVOID, LPVOID*);
		auto fnCreateHook = (CreateHook_t)addrCreateHook;

		return SpoofCall(fnCreateHook, (void*)pOriginal, (void*)pHookedFunction, (void**)pOriginalCall) == 0 ? true : false;
	}

	bool EnableHookQue()
	{
		static uintptr_t addrEnableHookQueu = NULL;

		if (!addrEnableHookQueu)
			addrEnableHookQueu = MemoryHelper::PatternScanW(DiscordModule, xorstr("41 57 41 56 41 55 41 54 56 57 55 53 48 83 EC 38 48 ? ? ? ? ? ? 48 31 E0 48 89 44 24 30 BE 01 00 00 00 31 C0 F0 ? ? ? ? ? ? ? 74 2B"));

		if (!addrEnableHookQueu)
			return false;

		using EnableHookQueu_t = uint64_t(__stdcall*)(VOID);
		auto fnEnableHookQueu = (EnableHookQueu_t)addrEnableHookQueu;

		return SpoofCall(fnEnableHookQueu) == 0 ? true : false;
	}


	bool EnableHook(uintptr_t pTarget, bool bIsEnabled)
	{
		static uintptr_t addrEnableHook = NULL;

		if (!addrEnableHook)
			addrEnableHook = MemoryHelper::PatternScanW(DiscordModule, xorstr("41 56 56 57 53 48 83 EC 28 49 89 CE BF 01 00 00 00 31 C0 F0 ? ? ? ? ? ? ? 74"));

		if (!addrEnableHook)
			return false;

		using EnableHook_t = uint64_t(__fastcall*)(LPVOID, bool);
		auto fnEnableHook = (EnableHook_t)addrEnableHook;

		return SpoofCall(fnEnableHook, (void*)pTarget, bIsEnabled) == 0 ? true : false;
	}

	bool InsertHook(uintptr_t pOriginal, uintptr_t pHookedFunction, uintptr_t pOriginalCall)
	{
		bool bAlreadyCreated = false;
		for (auto _Hook : pCreatedHooksArray)
		{
			if (_Hook == pOriginal)
			{
				bAlreadyCreated = true;
				break;
			}
		}

		if (!bAlreadyCreated)
			bAlreadyCreated = CreateHook(pOriginal, pHookedFunction, pOriginalCall);

		if (bAlreadyCreated)
			if (EnableHook(pOriginal, true))
				if (EnableHookQue())
					return true;

		return false;
	}


}

template<class T>
struct TArray
{
	friend struct FString;

public:
	inline TArray()
	{
		Data = nullptr;
		Count = Max = 0;
	};

	inline int Num() const
	{
		return Count;
	};

	inline T& operator[](int i)
	{
		return Data[i];
	};

	inline const T& operator[](int i) const
	{
		return Data[i];
	};

	inline bool IsValidIndex(int i) const
	{
		return i < Num();
	}

private:
	T* Data;
	int32_t Count;
	int32_t Max;
};

struct FString : private TArray<wchar_t>
{
	inline FString()
	{
	};

	FString(const wchar_t* other)
	{
		Max = Count = *other ? SpoofCall(std::wcslen, other) + 1 : 0;

		if (Count)
		{
			Data = const_cast<wchar_t*>(other);
		}
	};

	inline bool IsValid() const
	{
		return Data != nullptr;
	}

	inline const wchar_t* c_str() const
	{
		return Data;
	}

	std::string ToString() const
	{
		auto length = std::wcslen(Data);

		std::string str(length, '\0');

		std::use_facet<std::ctype<wchar_t>>(std::locale()).narrow(Data, Data + length, '?', &str[0]);

		return str;
	}
};

class UClass {
public:
	BYTE _padding_0[0x40];
	UClass* SuperClass;
};

class UObject {
public:
	PVOID VTableObject;
	DWORD ObjectFlags;
	DWORD InternalIndex;
	UClass* Class;
	BYTE _padding_0[0x8];
	UObject* Outer;

	inline BOOLEAN IsA(PVOID parentClass) {
		for (auto super = this->Class; super; super = super->SuperClass) {
			if (super == parentClass) {
				return TRUE;
			}
		}

		return FALSE;
	}
};

class FUObjectItem {
public:
	UObject* Object;
	DWORD Flags;
	DWORD ClusterIndex;
	DWORD SerialNumber;
	DWORD SerialNumber2;
};

class TUObjectArray {
public:
	FUObjectItem* Objects[9];
};

class GObjects {
public:
	TUObjectArray* ObjectArray;
	BYTE _padding_0[0xC];
	DWORD ObjectCount;
};

namespace StructsParams {

	struct AActor_K2_TeleportTo_Params
	{
		Vector3										DestLocation;                                             // (Parm, ZeroConstructor, IsPlainOldData)
		Vector3										DestRotation;                                             // (Parm, ZeroConstructor, IsPlainOldData)
		bool										ReturnValue;                                              // (Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData)
	};
}

BOOL IsValidPointer(uintptr_t address)
{
	if (!SpoofCall(IsBadWritePtr, (LPVOID)address, (UINT_PTR)8)) return TRUE;
	else return FALSE;
}

namespace FN {

	static GObjects* objects = nullptr;

	static void FreeFN(__int64 address)
	{
		auto func = reinterpret_cast<__int64(__fastcall*)(__int64 a1)>(FreeFn);

		SpoofCall(func, address);
	}

	static const char* GetObjectName(uintptr_t Object)
	{
		if (Object == NULL)
			return ("");

		auto fGetObjName = reinterpret_cast<FString * (__fastcall*)(int* index, FString * res)>(GetNameByIndex);

		int index = *(int*)(Object + 0x18);

		FString result;
		SpoofCall(fGetObjName, &index, &result);

		if (result.c_str() == NULL)
			return ("");

		auto result_str = result.ToString();

		if (result.c_str() != NULL)
			FreeFN((__int64)result.c_str());

		return result_str.c_str();
	}


	static const char* GetUObjectNameLoop(UObject* Object) {
		std::string name("");

		for (auto i = 0; Object; Object = Object->Outer, ++i) {

			auto fGetObjName = reinterpret_cast<FString * (__fastcall*)(int* index, FString * res)>(GetNameByIndex);

			int index = *(int*)(reinterpret_cast<uint64_t>(Object) + 0x18);

			FString internalName;
			SpoofCall(fGetObjName, &index, &internalName);

			if (internalName.c_str() == NULL) {
				break;
			}

			auto objectName = internalName.ToString();


			name = objectName.c_str() + std::string(i > 0 ? xorstr(".") : xorstr("")) + name;
			FreeFN((__int64)internalName.c_str());
		}

		return name.c_str();
	}

	static PVOID FindObject(const char* name) {

		for (auto array : objects->ObjectArray->Objects) {
			auto fuObject = array;
			std::cout << "";
			for (auto i = 0; i < 0x10000 && fuObject->Object; ++i, ++fuObject)
			{
				std::cout << "";
				auto object = fuObject->Object;

				if (object->ObjectFlags != 0x41) {
					continue;
				}
				std::cout << "";

				if (strstr(GetUObjectNameLoop(object), name)) {
					return object;
				}
			}
		}

		return 0;
	}

	static PVOID FindObject2(const char* name, const char* name2) {

		for (auto array : objects->ObjectArray->Objects) {
			auto fuObject = array;
			std::cout << "";
			for (auto i = 0; i < 0x10000 && fuObject->Object; ++i, ++fuObject)
			{
				std::cout << "";
				auto object = fuObject->Object;

				if (object->ObjectFlags != 0x41) {
					continue;
				}
				std::cout << "";

				if ((strstr(GetUObjectNameLoop(object), name)) and (strstr(GetUObjectNameLoop(object), name2))) {
					return object;
				}
			}
		}

		return 0;
	}


	static BOOL ProcessEvent(uintptr_t address, void* fnobject, void* parms)
	{
		if (!valid_pointer(address)) return FALSE;
		auto index = *reinterpret_cast<void***>(address); if (!index) return FALSE;
		auto fProcessEvent = static_cast<void(*)(void* address, void* fnobject, void* parms)>(index[0x4C]); if (!fProcessEvent) return FALSE;
		SpoofCall(fProcessEvent, (void*)address, (void*)fnobject, (void*)parms);
		return TRUE;
	}
}
namespace utilities
{
	
	inline uintptr_t imageBase()
	{
		return *(uintptr_t*)(__readgsqword(0x60) + 0x10);
	}

	inline bool isValidPointer(uintptr_t address)
	{
		if (!SpoofCall(IsBadWritePtr, (LPVOID)address, (UINT_PTR)8))
			return true;
		else
			return false;
	}

	template <typename R>
	inline R read(uintptr_t address)
	{
		R buffer{};

		if (!isValidPointer(address))
			return buffer;

		buffer = *(R*)address;

		return buffer;
	}

	template <typename W>
	inline void write(uintptr_t address, const W& value)
	{
		W buffer{};

		if (!isValidPointer(address))
			return;

		*(W*)address = value;
	}


	inline float custom_sqrtf(float _X)
	{
		return _mm_cvtss_f32(_mm_sqrt_ss(_mm_set_ss(_X)));
	}

	inline float custom_sinf(float _X)
	{
		return _mm_cvtss_f32(_mm_sin_ps(_mm_set_ss(_X)));
	}

	inline float custom_cosf(float _X)
	{
		return _mm_cvtss_f32(_mm_cos_ps(_mm_set_ss(_X)));
	}

	inline float custom_acosf(float _X)
	{
		return _mm_cvtss_f32(_mm_acos_ps(_mm_set_ss(_X)));
	}

	inline float custom_tanf(float _X)
	{
		return _mm_cvtss_f32(_mm_tan_ps(_mm_set_ss(_X)));
	}

	inline float custom_atan2f(float _X, float _Y)
	{
		return _mm_cvtss_f32(_mm_atan2_ps(_mm_set_ss(_X), _mm_set_ss(_Y)));
	}

	inline int custom_compare(const char* X, const char* Y)
	{
		while (*X && *Y) {
			if (*X != *Y) {
				return 0;
			}
			X++;
			Y++;
		}

		return (*Y == '\0');
	}

	inline int custom_wcompare(const wchar_t* X, const wchar_t* Y)
	{
		while (*X && *Y) {
			if (*X != *Y) {
				return 0;
			}
			X++;
			Y++;
		}

		return (*Y == L'\0');
	}

	inline const wchar_t* custom_wcsstr(const wchar_t* X, const wchar_t* Y)
	{
		while (*X != L'\0') {
			if ((*X == *Y) && custom_wcompare(X, Y)) {
				return X;
			}
			X++;
		}
		return NULL;
	}

	inline const char* custom_strstr(const char* X, const char* Y)
	{
		while (*X != '\0') {
			if ((*X == *Y) && custom_compare(X, Y)) {
				return X;
			}
			X++;
		}
		return NULL;
	}

	inline int custom_strlen(const char* string)
	{
		int cnt = 0;
		if (string)
		{
			for (; *string != 0; ++string) ++cnt;
		}
		return cnt;
	}

	inline int custom_wcslen(const wchar_t* string)
	{
		int cnt = 0;
		if (string)
		{
			for (; *string != 0; ++string) ++cnt;
		}
		return cnt;
	}
}
namespace sdk::classes
{
	class UClass {
	public:
		BYTE _padding_0[0x40];
		UClass* SuperClass;
	};

	class UObject {
	public:
		PVOID VTableObject;
		DWORD ObjectFlags;
		DWORD InternalIndex;
		UClass* Class;
		BYTE _padding_0[0x8];
		UObject* Outer;

		inline BOOLEAN IsA(PVOID parentClass)
		{
			for (auto super = this->Class; super; super = super->SuperClass)
				if (super == parentClass)
					return true;

			return false;
		}
	};

	class FUObjectItem {
	public:
		UObject* Object;
		DWORD Flags;
		DWORD ClusterIndex;
		DWORD SerialNumber;
		DWORD SerialNumber2;
	};

	class TUObjectArray {
	public:
		FUObjectItem* Objects[9];
	};

	class GObjects {
	public:
		TUObjectArray* ObjectArray;
		BYTE _padding_0[0xC];
		DWORD ObjectCount;
	};

	inline GObjects* objects = nullptr;
}
namespace sdk::structs
{
	struct FMatrix
	{
		float M[4][4];
	};
	static FMatrix* myMatrix = new FMatrix();

	template<class T>
	struct TArray
	{
		friend struct FString;

	public:
		inline TArray()
		{
			Data = nullptr;
			Count = Max = 0;
		};

		inline int Num() const
		{
			return Count;
		};

		inline T& operator[](int i)
		{
			return Data[i];
		};

		inline const T& operator[](int i) const
		{
			return Data[i];
		};

		inline bool IsValidIndex(int i) const
		{
			return i < Num();
		}

	private:
		T* Data;
		int32_t Count;
		int32_t Max;
	};

	struct FString : private TArray<wchar_t>
	{
		inline FString()
		{
		};

		FString(const wchar_t* other)
		{
			Max = Count = *other ? utilities::custom_wcslen(other) + 1 : 0;

			if (Count)
			{
				Data = const_cast<wchar_t*>(other);
			}
		};

		inline bool IsValid() const
		{
			return Data != nullptr;
		}

		inline const wchar_t* c_str() const
		{
			return Data;
		}
	};

	struct FName
	{
		uint32_t ComparisonIndex;
		uint32_t DisplayIndex;
	};

	struct FKey
	{
		FName KeyName;
		unsigned char Padding_00[0x10];
	};

	struct FLinearColor
	{
		float R;
		float G;
		float B;
		float A;
	};

	struct FVector2D
	{
		double X;
		double Y;
	};

	struct FVector
	{
		double X;
		double Y;
		double Z;

		
	};

	struct FRotator
	{
		double Pitch;
		double Yaw;
		double Roll;
	};

	struct FMinimalViewInfo {
		sdk::structs::FVector Location;
		sdk::structs::FRotator Rotation;
		float FOV;
		float OrthoWidth;
		float OrthoNearClipPlane;
		float OrthoFarClipPlane;
		float AspectRatio;
	};

	class FText {
	private:
		char _padding_[0x28];
		PWCHAR Name;
		DWORD Length;
	public:

		inline PWCHAR c_str() {
			return Name;
		}
	};

	struct FPawnHighlight
	{
		float                                              Priority_28_E2E1B5344846E187B9C11B863A7F0698;             // 0x0000(0x0004) (Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData)
		FLinearColor                                Inner_21_4CC2801147EA190DE16F59B34F36853E;                // 0x0004(0x0010) (Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData)
		FLinearColor                                Outer_22_5A1D7D0543D303E8B54B66A7F7BD2E2E;                // 0x0014(0x0010) (Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData)
		float                                              FresnelBrightness_23_52B0F96447FF640F47DF2895B0602E92;    // 0x0024(0x0004) (Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData)
		float                                              FresnelExponent_24_B427CF0C441AA37ED49833BF7579DE6D;      // 0x0028(0x0004) (Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData)
		float                                              UsesPulse_25_E29229F64E540F0617E4C4987AD77605;            // 0x002C(0x0004) (Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData)
	};

	struct FViewTargetTransitionParams
	{
		float BlendTime;
		int BlendFunction;
		float BlendExp;
		bool bLockOutgoing;
	};

	enum EViewTargetBlendFunction
	{
		VTBlend_Linear, // Camera does a simple linear interpolation.
		VTBlend_Cubic, // Camera has a slight ease in and ease out, but amount of ease cannot be tweaked.
		VTBlend_EaseIn, // Camera immediately accelerates, but smoothly decelerates into the target.  Ease amount controlled by BlendExp.
		VTBlend_EaseOut, // Camera smoothly accelerates, but does not decelerate into the target.  Ease amount controlled by BlendExp.
		VTBlend_EaseInOut, // Camera smoothly accelerates and decelerates.  Ease amount controlled by BlendExp.
		VTBlend_PreBlended, // The game's camera system has already performed the blending. Engine should not blend at all
		VTBlend_MAX,
	};

	class AHUD
	{
	public:
		unsigned char Padding_d24Be[0x228];
		char bLostFocusPaused : 1; // 0x228(0x1)
		char bShowHUD : 1; // 0x228(0x1)
		char bShowDebugInfo : 1; // 0x228(0x1)
		unsigned char UnknownBuffer_HaB72 : 5; // 0x228(0x1)
		unsigned char UnknownBuffer_7vY26[0x3]; // 0x229(0x3)
		int32_t CurrentTargetIndex; // 0x22c(0x4)
		char bShowHitBoxDebugInfo : 1; // 0x230(0x1)
		char bShowOverlays : 1; // 0x230(0x1)
		char bEnableDebugTextShadow : 1; // 0x230(0x1)
		unsigned char UnknownBuffer_2aNx7 : 5; // 0x230(0x1)
		unsigned char UnknownBuffer_8HAfk[0x7]; // 0x231(0x7)
		TArray<struct AActor*> PostRenderedActors; // 0x238(0x10)
		unsigned char UnknownBuffer_h7Nua[0x8]; // 0x248(0x8)
		TArray<struct FName> DebugDisplay; // 0x250(0x10)
		TArray<struct FName> ToggledDebugCategories; // 0x260(0x10)
		uintptr_t Canvas; // 0x270(0x8)
		uintptr_t DebugCanvas; // 0x278(0x8)
	};
}
namespace sdk::objects
{
	inline void free_object(uintptr_t address)
	{
		if (!utilities::isValidPointer(address)) return;
		auto function = reinterpret_cast<void(__fastcall*)(__int64)>(FreeFn);
		return SpoofCall(function, (__int64)address);
	}

	inline const wchar_t* get_name_by_index(int idx)
	{
		std::wstring name = xorstr(L"");

		if (!idx) return xorstr(L"");

		auto function = reinterpret_cast<structs::FString * (__fastcall*)(int* index, structs::FString * res)>(GetNameByIndex);

		int index = idx;

		structs::FString fObjectName;
		SpoofCall(function, &index, &fObjectName);

		if (!fObjectName.IsValid()) return xorstr(L"");

		auto objectName = fObjectName.c_str();

		name = objectName;
		free_object((uintptr_t)fObjectName.c_str());

		return name.c_str();
	}

	inline const wchar_t* get_object_name(classes::UObject* object)
	{
		std::wstring name = xorstr(L"");

		if (!object) return xorstr(L"");

		auto function = reinterpret_cast<structs::FString * (__fastcall*)(int* index, structs::FString * res)>(GetNameByIndex);

		int index = utilities::read<int>(reinterpret_cast<uint64_t>(object) + 0x18);

		structs::FString fObjectName;
		SpoofCall(function, &index, &fObjectName);

		if (!fObjectName.IsValid()) return xorstr(L"");

		auto objectName = fObjectName.c_str();

		name = objectName;
		free_object((uintptr_t)fObjectName.c_str());

		return name.c_str();
	}

	inline const wchar_t* get_full_object_name(classes::UObject* object)
	{
		std::wstring name = xorstr(L"");

		for (auto i = 0; object; object = object->Outer, i++)
		{
			if (!object) return xorstr(L"");

			auto function = reinterpret_cast<structs::FString * (__fastcall*)(int* index, structs::FString * res)>(GetNameByIndex);

			int index = utilities::read<int>(reinterpret_cast<uint64_t>(object) + 0x18);

			structs::FString fObjectName;
			SpoofCall(function, &index, &fObjectName);

			if (!fObjectName.IsValid()) break;

			auto objectName = fObjectName.c_str();

			name = objectName + std::wstring(i > 0 ? xorstr(L".") : xorstr(L"")) + name;
			free_object((uintptr_t)fObjectName.c_str());
		}

		return name.c_str();
	}

	inline void NumChunks(sdk::classes::TUObjectArray* ObjectArray, int* start, int* end)
	{
		int cStart = 0, cEnd = 0;

		if (!cEnd)
		{
			while (true)
			{
				if (ObjectArray->Objects[cStart] == nullptr)
				{
					cStart++;
				}
				else
				{
					break;
				}
			}

			cEnd = cStart;
			while (true)
			{
				if (ObjectArray->Objects[cEnd] == nullptr)
				{
					break;
				}
				cEnd++;
			}
		}

		*start = cStart;
		*end = cEnd;
	}

	inline sdk::classes::UObject* GetObjByIndex(sdk::classes::TUObjectArray* ObjectArray, int32_t index)
	{
		int cStart = 0, cEnd = 0;
		int chunkIndex, chunkSize = 0xFFFF, chunkPos;
		sdk::classes::FUObjectItem* Object;

		NumChunks(ObjectArray, &cStart, &cEnd);

		chunkIndex = index / chunkSize;
		if (chunkSize * chunkIndex != 0 &&
			chunkSize * chunkIndex == index)
		{
			chunkIndex--;
		}

		chunkPos = cStart + chunkIndex;
		if (chunkPos < cEnd)
		{
			Object = ObjectArray->Objects[chunkPos] + (index - chunkSize * chunkIndex);
			if (!Object) { return nullptr; }

			return Object->Object;
		}

		return nullptr;
	}


	inline const wchar_t* fix_objectname(const wchar_t* objname)
	{
		wchar_t* _4 = (wchar_t*)objname;

		for (int i = 0; i < utilities::custom_wcslen(_4); i++)
		{
			if (_4[i] == L'_')
			{
				if (_4[i + 1] == L'0' ||
					_4[i + 1] == L'1' ||
					_4[i + 1] == L'2' ||
					_4[i + 1] == L'3' ||
					_4[i + 1] == L'4' ||
					_4[i + 1] == L'5' ||
					_4[i + 1] == L'6' ||
					_4[i + 1] == L'7' ||
					_4[i + 1] == L'8' ||
					_4[i + 1] == L'9')
					_4[i] = L'$';

				for (int idx = 1; idx < 7; idx++) {

					if (_4[i + idx] == L'.') break;

					if (_4[i + idx] == L'0' ||
						_4[i + idx] == L'1' ||
						_4[i + idx] == L'2' ||
						_4[i + idx] == L'3' ||
						_4[i + idx] == L'4' ||
						_4[i + idx] == L'5' ||
						_4[i + idx] == L'6' ||
						_4[i + idx] == L'7' ||
						_4[i + idx] == L'8' ||
						_4[i + idx] == L'9') {
						_4[i + idx] = L'$';
					}
				}

			}
		}

		std::wstring str = _4;
		str.erase(std::remove(str.begin(), str.end(), '$'), str.end());
		return str.c_str();
	}

	inline PVOID find_object(const wchar_t* objectname)
	{
		for (auto index = 0x0; index < classes::objects->ObjectCount; index++)
		{
			auto object = GetObjByIndex(classes::objects->ObjectArray, index);
			const wchar_t* tmp_name = get_full_object_name(object);
			const wchar_t* name = fix_objectname(tmp_name);

			if ((utilities::custom_wcsstr(name, objectname)))
			{
				return object;
			}
		}

		return 0;
	}


	//inline sdk::structs::FName Conv_StringToName(const wchar_t* String)
	//{
	//	struct
	//	{
	//		sdk::structs::FString                                     inString;                                                 // (Parm, ZeroConstructor)
	//		sdk::structs::FName                                       ReturnValue;                                              // (Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData)
	//	} params;

	//	params.inString = String;

	//	FN::ProcessEvent((uintptr_t)sdk::cached::objects::KismetStringLibrary, sdk::cached::objects::Conv_StringToName, &params);

	//	return params.ReturnValue;
	//}
}

namespace ObjectsAddresses {
	static PVOID FOV;
	static PVOID AddYawInput;
	static PVOID AddPitchInput;
	static PVOID GetCameraLocation;
	static PVOID IsDead;
	static PVOID ApplyPawnHighlight;
}

BOOL FOV(uintptr_t playercontroller, float NewFOV) {
	if (!IsValidPointer((uintptr_t)playercontroller)) return 0;
	FN::ProcessEvent((uintptr_t)playercontroller, ObjectsAddresses::FOV, &NewFOV);
	return TRUE;
}
