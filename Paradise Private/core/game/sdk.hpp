#include "structures.hpp"
#include <map>
#include <unordered_map>
#include <utilities/obfuscation/spoof.h>

inline uintptr_t VaText = 0;

namespace Camera
{
	inline FVector Location;
	inline FVector Rotation;
	inline float Fov;
}

namespace Addresses
{
	inline uintptr_t UWorld;
}

inline bool Lobby;

inline HWND hWindowHandle1 = 0;
inline HWND hwnd = 0;
inline HWND overlay = 0;

struct RankInfo
{
	std::string name;
	ImColor color;
};

inline auto AddArrowIndicator(ImVec2 vScreenPosition) -> void
{
	auto vCenter = ImVec2{ jrt::screen.fWidth / 2, jrt::screen.fHeight / 2 };
	float fAngle = atan2f(vScreenPosition.y - vCenter.y, vScreenPosition.x - vCenter.x);

	ImVec2 vArrowPosition;
	vArrowPosition.x = vCenter.x + ((jrt::settings.iFovRadius * 10) + 10.f) * cosf(fAngle);
	vArrowPosition.y = vCenter.y + ((jrt::settings.iFovRadius * 10) + 10.f) * sinf(fAngle);

	float fRotationAngle = fAngle - ImGui::GetIO().DeltaTime;

	ImVec2 vVertex[3] =
	{
		ImVec2(vArrowPosition.x + cosf(fRotationAngle) * 25.0f, vArrowPosition.y + sinf(fRotationAngle) * 25.0f),
		ImVec2(vArrowPosition.x + cosf(fRotationAngle - 1.5f) * 8.0f, vArrowPosition.y + sinf(fRotationAngle - 1.5f) * 8.0f),
		ImVec2(vArrowPosition.x + cosf(fRotationAngle + 1.5f) * 8.0f, vArrowPosition.y + sinf(fRotationAngle + 1.5f) * 8.0f)
	};

	ImGui::GetForegroundDrawList()->AddTriangleFilled(vVertex[0], vVertex[1], vVertex[2], ImColor(255, 255, 255, 255));
}

struct FRankedProgressReplicatedData
{
	char pad_0[0x10];
	int32_t Rank;
	char pad_14[0x4];
};

inline RankInfo GetRankName(int32_t Rank)
{
	switch (Rank) {
	case 0: return { _s("Bronze 1"), ImColor(223, 124, 77) };
	case 1: return { _s("Bronze 2"), ImColor(223, 124, 77) };
	case 2: return { _s("Bronze 3"), ImColor(223, 124, 77) };
	case 3: return { _s("Silver 1"), ImColor(70, 85, 115) };
	case 4: return { _s("Silver 2"), ImColor(70, 85, 115) };
	case 5: return { _s("Silver 3"), ImColor(70, 85, 115) };
	case 6: return { _s("Gold 1"), ImColor(255, 215, 0) };
	case 7: return { _s("Gold 2"), ImColor(255, 215, 0) };
	case 8: return { _s("Gold 3"), ImColor(255, 215, 0) };
	case 9: return { _s("Platinum 1"), ImColor(158, 255, 255) };
	case 10: return { _s("Platinum 2"), ImColor(158, 255, 255) };
	case 11: return { _s("Platinum 3"), ImColor(158, 255, 255) };
	case 12: return { _s("Diamond 1"), ImColor(185, 242, 255) };
	case 13: return { _s("Diamond 2"), ImColor(185, 242, 255) };
	case 14: return { _s("Diamond 3"), ImColor(185, 242, 255) };
	case 15: return { _s("Elite"), ImColor(54, 58, 52) };
	case 16: return { _s("Champion"), ImColor(146, 38, 9) };
	case 17: return { _s("Unreal"), ImColor(126, 98, 250) };
	default: return { _s("Unranked"), ImColor(250, 250, 250, 250) };

	}
}

class USceneViewState
{
public:
	auto UpdateCamera( ) -> void
	{
		auto mProjection = read<FMatrix>( CURRENT_CLASS + 0x900 );

		Camera::Rotation.x = RadiansToDegrees( std::asin( mProjection.ZPlane.W ) );
		Camera::Rotation.y = RadiansToDegrees( std::atan2( mProjection.YPlane.W, mProjection.XPlane.W ) );
		Camera::Rotation.z = 0.0;

		Camera::Location.x = mProjection.m[ 3 ][ 0 ];
		Camera::Location.y = mProjection.m[ 3 ][ 1 ];
		Camera::Location.z = mProjection.m[ 3 ][ 2 ];

		float FieldOfView = atanf( 1 / read<double>( CURRENT_CLASS + 0x700 ) ) * 2;
		Camera::Fov = ( FieldOfView ) * ( 180.f / M_PI );
	}

	auto WorldToScreen( FVector WorldLocation ) -> FVector2D
	{
		auto mMatrix = Matrix( Camera::Rotation );
		auto XAxis = FVector( mMatrix.m[ 0 ][ 0 ], mMatrix.m[ 0 ][ 1 ], mMatrix.m[ 0 ][ 2 ] );
		auto YAxis = FVector( mMatrix.m[ 1 ][ 0 ], mMatrix.m[ 1 ][ 1 ], mMatrix.m[ 1 ][ 2 ] );
		auto ZAxis = FVector( mMatrix.m[ 2 ][ 0 ], mMatrix.m[ 2 ][ 1 ], mMatrix.m[ 2 ][ 2 ] );

		auto vDeltaCoordinates = WorldLocation - Camera::Location;
		auto tTransform = FVector( vDeltaCoordinates.Dot( YAxis ), vDeltaCoordinates.Dot( ZAxis ), vDeltaCoordinates.Dot( XAxis ) );

		if ( tTransform.z < 0.1 )
			tTransform.z = 0.1;

		FVector2D vScreen;
		vScreen.x = ( jrt::screen.fWidth / 2 ) + tTransform.x * ( ( jrt::screen.fWidth / 2 ) / tan( Camera::Fov * 3.14159265358979323846 / 360 ) ) / tTransform.z;
		vScreen.y = ( jrt::screen.fHeight / 2 ) - tTransform.y * ( ( jrt::screen.fWidth / 2 ) / tan( Camera::Fov * 3.14159265358979323846 / 360 ) ) / tTransform.z;
		return vScreen;
	}
};

namespace Cached
{
	inline USceneViewState* ViewState;
}

class UFortWeaponItemDefinition
{
public:
	auto GetDisplayName() -> std::string
	{
		auto pDisplayName = read<uintptr_t>(CURRENT_CLASS + Offset::ItemName);
		if (!pDisplayName)
			return _s("None");

		auto iWeaponLength = read<uint32_t>(pDisplayName + 0x20);

		std::vector<wchar_t> wcWeaponName(iWeaponLength + 1);

		if (wcWeaponName.data())
			Driver::ReadPhysicalMemory(PVOID(read<uintptr_t>(pDisplayName + 0x18)), wcWeaponName.data(), iWeaponLength * sizeof(wchar_t));

		std::wstring wWeaponString(wcWeaponName.data());
		wcWeaponName.clear();
		return std::string(wWeaponString.begin(), wWeaponString.end());
	}
};

class AFortWeapon
{
public:
	auto GetProjectileSpeed( ) -> float
	{
		return read<float>( CURRENT_CLASS + Offset::ProjectileSpeed );
	}

	auto GetAmmoCount() -> int32
	{
		return read<int32>(CURRENT_CLASS + Offset::AmmoCount);
	}

	auto GetProjectileGravity( ) -> float
	{
		return read<float>( CURRENT_CLASS + Offset::ProjectileGravity );
	}

	auto GetWeaponData( ) -> UFortWeaponItemDefinition*
	{
		return read< UFortWeaponItemDefinition*>( CURRENT_CLASS + Offset::WeaponData );
	}

	auto GetWeaponAnimation( ) -> uint8_t
	{
		return read<uint8_t>( CURRENT_CLASS + Offset::WeaponCoreAnimation );
	}
	auto SimplifyWeaponType( uint8_t WeaponCoreAnim ) -> EFortWeaponType
	{
		switch ( WeaponCoreAnim )
		{
		case EFortWeaponCoreAnimation__Melee:
			return EFortWeaponType::Melee;
			break;
		case EFortWeaponCoreAnimation__Pistol:
			return EFortWeaponType::Pistol;
			break;
		case EFortWeaponCoreAnimation__Shotgun:
			return EFortWeaponType::Shotgun;
			break;
		case EFortWeaponCoreAnimation__Rifle:
			return EFortWeaponType::Rifle;
			break;
		case EFortWeaponCoreAnimation__MeleeOneHand:
			return EFortWeaponType::Melee;
			break;
		case EFortWeaponCoreAnimation__MachinePistol:
			return EFortWeaponType::Smg;
			break;
		case EFortWeaponCoreAnimation__AssaultRifle:
			return EFortWeaponType::Rifle;
			break;
		case EFortWeaponCoreAnimation__TacticalShotgun:
			return EFortWeaponType::Shotgun;
			break;
		case EFortWeaponCoreAnimation__SniperRifle:
			return EFortWeaponType::Sniper;
			break;
		case EFortWeaponCoreAnimation__Crossbow:
			return EFortWeaponType::Sniper;
			break;
		case EFortWeaponCoreAnimation__DualWield:
			return EFortWeaponType::Pistol;
			break;
		case EFortWeaponCoreAnimation__SMG_P90:
			return EFortWeaponType::Smg;
			break;
		case EFortWeaponCoreAnimation__AR_DrumGun:
			return EFortWeaponType::Rifle;
			break;
		default:
			return EFortWeaponType::Unarmed;
			break;
		}
	}
};

class USceneComponent
{
public:
	auto GetRelativeLocation( ) -> FVector
	{
		return read<FVector>( CURRENT_CLASS + Offset::RelativeLocation );
	}
};

class USkeletalMeshComponent
{
public:
	auto GetBoneMatrix( __int64 pBoneArray, int iIndex ) -> FVector
	{
		auto BoneTransform = read<FTransform>( pBoneArray + 0x60 * iIndex );
		auto ComponentToWorld = read<FTransform>( CURRENT_CLASS + Offset::ComponentToWorld );
		auto Matrix = MatrixMultiplication( BoneTransform.ToMatrixWithScale( ), ComponentToWorld.ToMatrixWithScale( ) );
		return FVector( Matrix._41, Matrix._42, Matrix._43 );
	}
	auto IsVisible( ) -> bool
	{
		double fLastSubmitTime = read<double>(Addresses::UWorld + 0x148 );
		auto fLastRenderTime = read<float>( CURRENT_CLASS + Offset::LastRenderTimeOnScreen);
		return ( fLastRenderTime + 0.06f >= fLastSubmitTime );
	}

	auto GetStaticBounds( FVector2D vHead, FVector2D vRoot, float fSize ) -> FBounds
	{
		FBounds bBounds;
		bBounds.top = vHead.y;
		bBounds.bottom = vRoot.y;
		bBounds.left = ( vHead.x - ( fSize / 2 ) );
		bBounds.right = vRoot.x + ( fSize / 2 );
		return bBounds;
	}
};

class AFortPlayerState
{
public:

	auto GetTeamIndex( ) -> int
	{
		return read<int>( CURRENT_CLASS + Offset::TeamIndex );
	}

	std::string GetPlatform()
	{
		uintptr_t TestPlatform = read<uintptr_t>(CURRENT_CLASS + Offset::Platform);
		wchar_t Platform[64]{ 0 };
		Driver::ReadPhysicalMemory(reinterpret_cast<PVOID>(TestPlatform), reinterpret_cast<uint8_t*>(Platform), sizeof(Platform));
		std::wstring PlatformWString(Platform);
		std::string PlatformString(PlatformWString.begin(), PlatformWString.end());
		return PlatformString;
	}

	auto GetPlayerName() -> std::string
	{
		int pNameLength; // rsi
		WORD* pNameBufferPointer;
		int i;      // ecx
		char v25; // al
		int v26;  // er8
		int v29;  // eax

		char16_t* pNameBuffer;


		uintptr_t pNameStructure = read<uintptr_t>(CURRENT_CLASS + Offset::NameStructure); // + 0x30 or - 0x30
		pNameLength = (read<int>)(pNameStructure + 0x10);
		if (pNameLength <= 0)
			return _s("");

		pNameBuffer = new char16_t[pNameLength];
		uintptr_t pNameEncryptedBuffer = (read<uintptr_t>)(pNameStructure + 0x8);

		Driver::ReadPhysicalMemory((PVOID)pNameEncryptedBuffer, pNameBuffer, pNameLength * sizeof(wchar_t));


		v25 = pNameLength - 1;
		v26 = 0;
		pNameBufferPointer = (WORD*)pNameBuffer;

		for (i = (v25) & 3;; *pNameBufferPointer++ += i & 7) {
			v29 = pNameLength - 1;
			if (!(DWORD)pNameLength)
				v29 = 0;

			if (v26 >= v29)
				break;

			i += 3;
			++v26;
		}

		std::u16string temp_wstring(pNameBuffer);
		delete[] pNameBuffer;
		return std::string(temp_wstring.begin(), temp_wstring.end());
	}

	auto GetHabaneroComponent() -> uintptr_t
	{
		return read<uintptr_t>(CURRENT_CLASS + Offset::HabaneroComponent);
	}
};

class APawn
{
public:

	auto GetCurrentVehicle() -> APawn*
	{
		return read<APawn*>((uintptr_t)this + Offset::CurrentVehicle);
	}

	auto GetCurrentWeapon( ) -> AFortWeapon*
	{
		return read<AFortWeapon*>( CURRENT_CLASS + Offset::CurrentWeapon );
	}

	auto GetRootComponent( ) -> USceneComponent*
	{
		return read<USceneComponent*>( CURRENT_CLASS + Offset::RootComponent );
	}

	auto GetMesh( ) -> USkeletalMeshComponent*
	{
		return read<USkeletalMeshComponent*>( CURRENT_CLASS + Offset::Mesh );
	}
	auto GetPlayerState( ) -> AFortPlayerState*
	{
		return read<AFortPlayerState*>( CURRENT_CLASS + Offset::PlayerState );
	}

	auto IsDespawning( ) -> bool
	{
		return ( read<char>( CURRENT_CLASS + Offset::bIsDying ) >> 5 & 1 );
	}

	auto bAlreadySearched() -> bool
	{
		return (read<char>(CURRENT_CLASS + Offset::bAlreadySearched) >> 2 & 1);
	}

	auto IsDBNO() -> bool
	{
		return (read<char>(CURRENT_CLASS + Offset::bIsDBNO) >> 5 & 1);
	}

};

class APlayerState : public AFortPlayerState
{
public:
	auto GetPawnPrivate( ) -> APawn*
	{
		return read<APawn*>( CURRENT_CLASS + Offset::PawnPrivate );
	}
}; 

class APlayerController
{
public:
	auto GetPawn( ) -> APawn*
	{
		return read<APawn*>( CURRENT_CLASS + Offset::AcknowledgedPawn );
	}
	auto IsTargetUnderReticle( APawn* Target, __int64 pBoneMap, int BoneIndex ) -> bool
	{
		auto vLocationUnderReticle = this->GetLocationUnderReticle( );
		auto vTargetBone = Target->GetMesh( )->GetBoneMatrix( pBoneMap, BoneIndex );

		float fOffset = 20.0f;

		if ( vLocationUnderReticle.x >= vTargetBone.x - fOffset && vLocationUnderReticle.x <= vTargetBone.x + fOffset &&
			vLocationUnderReticle.y >= vTargetBone.y - fOffset && vLocationUnderReticle.y <= vTargetBone.y + fOffset &&
			vLocationUnderReticle.z >= vTargetBone.z - fOffset && vLocationUnderReticle.z <= vTargetBone.z + fOffset )
			return true;
		else
			return false;
	}
	auto GetLocationUnderReticle( ) -> FVector
	{
		return read<FVector>( CURRENT_CLASS + Offset::LocationUnderReticle );
	}
};

class ULocalPlayer
{
public:
	auto GetPlayerController( ) -> APlayerController*
	{
		return read<APlayerController*>( CURRENT_CLASS + Offset::PlayerController );
	}
	auto GetViewState( ) -> USceneViewState*
	{
		TArray<USceneViewState*> ViewState = read<TArray<USceneViewState*>>( CURRENT_CLASS + 0xd0 );
		return ViewState.Get( 1 );
	}
};

class UGameInstance
{
public:
	auto GetLocalPlayer( ) -> ULocalPlayer*
	{
		return read<ULocalPlayer*>( read<uintptr_t>( CURRENT_CLASS + Offset::LocalPlayers ) );
	}
};

class AGameStateBase
{
public:
	auto GetPlayerArray( ) -> TArray<APlayerState*>
	{
		return read<TArray<APlayerState*>>( CURRENT_CLASS + Offset::PlayerArray );
	}

	auto GetServerWorldTimeSecondsDelta() -> float
	{
		return read<float>(CURRENT_CLASS + Offset::ServerWorldTimeSecondsDelta);
	}
};

class UWorld
{
public:
	static auto Get( ) -> UWorld*
	{
		return read<UWorld*>(Base + Offset::Uworld);
	}

	auto GetGameState( ) -> AGameStateBase*
	{
		return read<AGameStateBase*>( CURRENT_CLASS + Offset::GameState );
	}
	auto GetGameInstance( ) -> UGameInstance*
	{
		return read<UGameInstance*>( CURRENT_CLASS + Offset::GameInstance );
	}
};

namespace Cached
{
	inline UWorld* GWorld;
	inline AGameStateBase* GameState;
	inline bool bAlreadySearched;
	inline APlayerController* PlayerController;
	inline APawn* Pawn;
	inline int TeamIndex;
	inline std::string Platform;
	inline ImVec2 RadarPosition;

	inline APawn* ClosestPawn;
	inline APawn* TargetPawn;
	inline float TargetDistance;
	inline float ClosestDistance;
	inline FVector Bone;
	inline EFortWeaponType FortWeaponType;
}

class Actor
{
public:
	APawn* Pawn;
	APlayerState* PlayerState;
	USkeletalMeshComponent* Mesh;
	std::uintptr_t pBoneArray;
	std::string Username;
	std::string PlatformActor;
	FRankedProgressReplicatedData RankInformation;
};

inline std::vector<Actor> ActorList;
inline std::vector<Actor> TemporaryActorList;

inline ImColor GetWeaponColor(uintptr_t WeaponData)
{
	uint8_t Tier = read<uint8_t>(WeaponData + Offset::ItemRarity);

	switch (Tier)
	{
	case 0:
		return { 200, 200, 200, 233 };
		break;
	case 1:
		return { 50, 215, 50, 255 };
		break;
	case 2:
		return { 65, 105, 225, 255 };
		break;
	case 3:
		return { 163, 53, 238, 255 };
		break;
	case 4:
		return { 250, 135, 0, 255 };
		break;
	case 5:
		return { 255, 255, 0, 255 };
		break;

	case 6:
		return { 0, 200, 255, 255 };
		break;
	}

	return { 255, 255, 255 };
}

inline ImColor GetWeaponDataColor(uintptr_t Player)
{
	uint64_t PlayersCurrentWeapon = read<uint64_t>(Player + Offset::CurrentWeapon);
	ImColor ReturnedColor = ImColor(255, 255, 255);

	if (PlayersCurrentWeapon)
	{
		uint64_t WeaponData = read<uint64_t>(PlayersCurrentWeapon + Offset::WeaponData);

		if (WeaponData)
		{
			ReturnedColor = GetWeaponColor(WeaponData);
		}
	}

	return ReturnedColor;
}