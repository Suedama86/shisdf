#include "combat.hpp"
#include <core/game/sdk.hpp>
#include <dependencies/random.h>

auto powf_(float _X, float _Y) -> float { return (_mm_cvtss_f32(_mm_pow_ps(_mm_set_ss(_X), _mm_set_ss(_Y)))); };
auto GetCrossDistance(double x1, double y1, double x2, double y2) -> double { return sqrtf(powf((x2 - x1), 2) + powf_((y2 - y1), 2)); };
template<typename T> static inline T ImLerp(T a, T b, float t) { return (T)(a + (b - a) * t); }

auto SetMousePosition(FVector2D vTarget) -> void
{
	FVector2D vScreenCenter(jrt::screen.fWidth / 2, jrt::screen.fHeight / 2);
	FVector2D vDestination(0, 0);
	int iSmoothFactor = jrt::settings.fSmooth;

	if (vTarget.x != 0)
	{
		vDestination.x = (vTarget.x > vScreenCenter.x) ? -(vScreenCenter.x - vTarget.x) : (vTarget.x - vScreenCenter.x);
		vDestination.x /= iSmoothFactor;
		vDestination.x = (vDestination.x + vScreenCenter.x > jrt::screen.fWidth) ? 0 : vDestination.x;
	}

	if (vTarget.y != 0)
	{
		vDestination.y = (vTarget.y > vScreenCenter.y) ? -(vScreenCenter.y - vTarget.y) : (vTarget.y - vScreenCenter.y);
		vDestination.y /= iSmoothFactor;
		vDestination.y = (vDestination.y + vScreenCenter.y > jrt::screen.fHeight) ? 0 : vDestination.y;
	}

	mouse_event(MOUSEEVENTF_MOVE, vDestination.x, vDestination.y, NULL, NULL);
}

FVector2D vTargetScreen;
FVector vTargetBone;
__int64 pTargetBoneMap = 0;

inline bool bCanClick = false;
std::chrono::steady_clock::time_point tCachedTime;
bool bHasRecentlyClicked = false;
int iTimeSince;
std::chrono::steady_clock::time_point tpBeginning;
std::chrono::steady_clock::time_point tpEnding;
auto jrt::combat_c::TriggerBot() -> void
{
	if (jrt::settings.bTriggerbot)
	{
		auto tCurrentTime = std::chrono::steady_clock::now();
		auto fTimeElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(tCurrentTime - tCachedTime).count();

		if (fTimeElapsed > 150)
			bCanClick = true;

		int iBoneIndex = 0;
		switch (jrt::settings.iHitBox)
		{
		case 0:
			iBoneIndex = 110;
			break;
		case 1:
			iBoneIndex = 67;
			break;
		case 2:
			iBoneIndex = 7;
			break;
		case 3:
			iBoneIndex = 2;
			break;
		}

		float fDistance = Camera::Location.Distance(Cached::TargetPawn->GetRootComponent()->GetRelativeLocation()) / 100.f;
		bool bIsKeyDown = SPOOF_CALL(GetAsyncKeyState)(jrt::settings.iTriggerbotKeybind);
		if (Cached::TargetPawn && (fDistance <= jrt::settings.iMaxDistance) && Cached::PlayerController->IsTargetUnderReticle(Cached::TargetPawn, pTargetBoneMap, iBoneIndex) && bIsKeyDown)
		{
			if (jrt::settings.bShotgunOnly)
			{
				if (Cached::FortWeaponType == EFortWeaponType::Shotgun)
				{
					if (bHasRecentlyClicked)
					{
						tpBeginning = std::chrono::steady_clock::now();
						bHasRecentlyClicked = false;
					}

					tpEnding = std::chrono::steady_clock::now();
					iTimeSince = std::chrono::duration_cast<std::chrono::milliseconds>(tpEnding - tpBeginning).count();

					if (iTimeSince >= jrt::settings.iCustomDelay)
					{
						if (bCanClick)
						{
							mouse_event(MOUSE_LEFT_BUTTON_DOWN, NULL, NULL, NULL, NULL);
							mouse_event(MOUSE_LEFT_BUTTON_UP, NULL, NULL, NULL, NULL);

							bHasRecentlyClicked = true;
							bCanClick = false;
							tCachedTime = std::chrono::steady_clock::now();
						}
					}
				}
			}
			else
			{
				if (bHasRecentlyClicked)
				{
					tpBeginning = std::chrono::steady_clock::now();
					bHasRecentlyClicked = false;
				}

				tpEnding = std::chrono::steady_clock::now();
				iTimeSince = std::chrono::duration_cast<std::chrono::milliseconds>(tpEnding - tpBeginning).count();

				if (iTimeSince >= jrt::settings.iCustomDelay)
				{
					if (bCanClick)
					{

						mouse_event(MOUSE_LEFT_BUTTON_DOWN, NULL, NULL, NULL, NULL);
						mouse_event(MOUSE_LEFT_BUTTON_UP, NULL, NULL, NULL, NULL);

						bHasRecentlyClicked = true;
						bCanClick = false;
						tCachedTime = std::chrono::steady_clock::now();
					}
				}
			}
		}
	}
}

auto jrt::combat_c::CombatThread() -> void
{
	for (;; )
	{
		int iBoneIDs[4] = { EBoneIndex::Head, EBoneIndex::Neck, EBoneIndex::Chest, EBoneIndex::Pelvis };
		vTargetScreen = { };
		vTargetBone = { };
		pTargetBoneMap = 0;

		auto tCachedTime = std::chrono::high_resolution_clock::now();

		for (;; )
		{
			auto tCurrentTime = std::chrono::high_resolution_clock::now();
			if (std::chrono::duration_cast<std::chrono::microseconds>(tCurrentTime - tCachedTime).count() < 5000) continue;
			tCachedTime = tCurrentTime;

			this->TriggerBot();

			if (Cached::Pawn)
			{
				Cached::TargetDistance = FLT_MAX;
				Cached::TargetPawn = NULL;

				for (auto& Actor : ActorList)
				{
					auto Pawn = Actor.Pawn;
					auto Mesh = Pawn->GetMesh();
					auto pBoneArray = Actor.pBoneArray;

					bool bIsVisible = false;

					if (jrt::settings.bVisibleCheck)
						bIsVisible = Mesh->IsVisible();
					else
						bIsVisible = true;

					if (jrt::settings.bVisibleCheck)
						if (!bIsVisible)
							continue;

					if (Pawn->IsDespawning())
						continue;

					if (Pawn->IsDBNO())
						continue;

					if (Cached::TeamIndex == Actor.PlayerState->GetTeamIndex())
						continue;

					

					if (jrt::settings.bAimbot)
					{
						FVector2D vHeadScreen = Cached::ViewState->WorldToScreen(Mesh->GetBoneMatrix(pBoneArray, 110));
						FVector2D vDistanceFromCrosshair = FVector2D(vHeadScreen.x - (jrt::screen.fWidth / 2), vHeadScreen.y - (jrt::screen.fHeight / 2));

						auto fDist = sqrtf(vDistanceFromCrosshair.x * vDistanceFromCrosshair.x + vDistanceFromCrosshair.y * vDistanceFromCrosshair.y);
						if (fDist < jrt::settings.iFovRadius * 10 && fDist < Cached::TargetDistance)
						{
							Cached::TargetDistance = fDist;
							Cached::TargetPawn = Pawn;
							pTargetBoneMap = pBoneArray;
						}
					}
				}

				if (Cached::TargetPawn)
				{

					if (jrt::settings.iAimbone == 0)
					{
						vTargetBone = Cached::TargetPawn->GetMesh()->GetBoneMatrix(pTargetBoneMap, 67);
						vTargetScreen = Cached::ViewState->WorldToScreen(vTargetBone);
					}
					else if (jrt::settings.iAimbone == 1)
					{
						vTargetBone = Cached::TargetPawn->GetMesh()->GetBoneMatrix(pTargetBoneMap, 110);
						vTargetScreen = Cached::ViewState->WorldToScreen(vTargetBone);
					}
					else if (jrt::settings.iAimbone == 2)
					{
						vTargetBone = Cached::TargetPawn->GetMesh()->GetBoneMatrix(pTargetBoneMap, 7);
						vTargetScreen = Cached::ViewState->WorldToScreen(vTargetBone);
					}
					else if (jrt::settings.iAimbone == 3)
					{
						vTargetBone = Cached::TargetPawn->GetMesh()->GetBoneMatrix(pTargetBoneMap, 2);
						vTargetScreen = Cached::ViewState->WorldToScreen(vTargetBone);
					}
				}
			}

			if (Cached::TargetPawn && Cached::Pawn && SPOOF_CALL(SPOOF_CALL(GetAsyncKeyState))(jrt::settings.iAimbotKeybind) && !vTargetScreen.IsZero() && (GetCrossDistance(vTargetScreen.x, vTargetScreen.y, (jrt::screen.fWidth / 2), (jrt::screen.fHeight / 2) <= jrt::settings.iFovRadius)))
			{
				auto CurrentWeapon = Cached::Pawn->GetCurrentWeapon();
				auto fTargetDistance = Camera::Location.Distance(vTargetBone);
				auto fProjectileSpeed = CurrentWeapon->GetProjectileSpeed();
				auto fProjectileGravity = CurrentWeapon->GetProjectileGravity();

				if (Cached::FortWeaponType == EFortWeaponType::Melee)
				{
					continue;
				}


				if (fProjectileSpeed && fProjectileGravity && CurrentWeapon && jrt::settings.bPrediction)
				{
					auto pTargetRoot = read<uintptr_t>((uintptr_t)Cached::TargetPawn + Offset::RootComponent);
					auto vTargetVelocity = read<FVector>(pTargetRoot + Offset::ComponentVelocity);
					vTargetBone = vTargetBone.Predict(vTargetBone, vTargetVelocity, fTargetDistance, fProjectileSpeed, fProjectileGravity);
					vTargetScreen = Cached::ViewState->WorldToScreen(vTargetBone);
				}

				SetMousePosition(vTargetScreen);

			}
			else
			{
				vTargetBone = FVector(0, 0, 0);
				vTargetScreen = FVector2D(0, 0);
				Cached::TargetPawn = NULL;
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}
		}
	}
}