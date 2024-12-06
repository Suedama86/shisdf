#include <dependencies/framework/imgui.h>

namespace jrt
{
	class settings_c
	{
	public:
		bool bAimbot = true;
		bool bPrediction = true;
		bool bVisibleCheck = true;
		bool IgnoreDowned = true;
		bool bRenderFOV = false;
		float iFovRadius = 15;
		float fSmooth = 5;
		int iHitBox = 1;
		int iAimbotType = 0;

		bool bTriggerbot = false;
		bool bShotgunOnly = true;
		bool bIgnoreKeybind = false;
		int iCustomDelay = 100;
		int iMaxDistance = 15;
		int iTriggerbotKeybind;
		int Reload;
		bool bUsername = true;
		bool bDistance = true;
		bool bHeldWeapon = true;
		bool bSkeletons = true;
		bool bBox = false;
		int iBoxType = 0;
		bool bBoxOutline = true;
		bool bTextOutline = true;
		float iSkeletonThickness = 2;
		float fOutlinedSize = 1.3;
		bool bFilledBox = false;
		float iBoxThickness = 2;
		bool bIndicators = true;
		float iRenderDistance = 280.f;
		bool bRanked = false;
		bool bPlatform = false;
		bool Mouse = true;
		bool Memory = false;
		bool bMemoryAim = false;
		bool bShowTeam = false;
		bool bEnable = true;
		bool bIgnoreDying = true;
		bool bMenuOpen;
		bool bShowRarity = true;
		int iAimbotKeybind;
		int iAimbone = 0;
		bool isAwaitingKeyPress = false;
		int iVisiblePlayers = 0;
		int iPlayerCount = 0;
		bool bOutlinedSkeleton = false;
		int iCurrentItem = 0;
		int iAimKeyPos = 3;
		int iTriggerKeyPos = 3;
		bool bMagicBullet = false;
		float fMagicBulletSize = 4;
		ImColor iVisibleColor = ImColor(166, 71, 255);
		ImColor iInvisibleColor = ImColor(235, 67, 52);
	};
	inline settings_c settings;
}