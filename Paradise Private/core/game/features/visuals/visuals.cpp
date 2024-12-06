#include "visuals.hpp"
#include <core/game/sdk.hpp>
#include <dependencies/random.h>

auto jrt::visuals_c::ActorLoop( ) -> void
{
	ImDrawList* iDrawList = ImGui::GetBackgroundDrawList( );

    APawn* TemporaryClosestPlayer = 0;
    int VisibleCount = 0;
    int PlayerCount = 0;

	for ( auto& Actor : ActorList )
	{
		auto Pawn = Actor.Pawn;
		auto Mesh = Actor.Mesh;
		auto pBoneArray = Actor.pBoneArray;

        if ( !Pawn )
            continue;

        if ( !Mesh )
            continue;

        if (!Pawn)
            continue;

        if (!Mesh)
            continue;

     
		
		Cached::ViewState->UpdateCamera( );

        PlayerCount++;

        auto GameInstance = Cached::GWorld->GetGameInstance();

        auto LocalPlayer = GameInstance->GetLocalPlayer();

		auto vHeadWorld = Mesh->GetBoneMatrix( pBoneArray, EBoneIndex::Head );

		auto vHeadScreen = Cached::ViewState->WorldToScreen( { vHeadWorld.x, vHeadWorld.y, vHeadWorld.z + 15 } );

		auto vRootWorld = Mesh->GetBoneMatrix( pBoneArray, EBoneIndex::Root );
		auto vRootScreen = Cached::ViewState->WorldToScreen( vRootWorld );

        auto GetServerTime = Cached::GameState->GetServerWorldTimeSecondsDelta();

        auto ClientSettingsRecord = read<uintptr_t>(std::uintptr_t(LocalPlayer) + Offset::ClientSettingsRecord);

        float HudScale = read<float>(ClientSettingsRecord + Offset::HudScale);

        auto fDistance = Camera::Location.Distance( vRootWorld ) / 100.f;

        bool bVisible = jrt::settings.bVisibleCheck ? Mesh->IsVisible() : true;

        if (fDistance < Cached::ClosestDistance)
        {
            TemporaryClosestPlayer = Pawn;
            Cached::ClosestDistance = fDistance;
        }

        GetAllKeys();
     
        if ( fDistance >= jrt::settings.iRenderDistance )
            continue;

        if (jrt::settings.bIgnoreDying)
        {
            if (Pawn->IsDespawning())
                continue;
        }

        if ( Cached::Pawn )
        {
            if (!jrt::settings.bShowTeam )
            {
                if (Cached::TeamIndex == Actor.PlayerState->GetTeamIndex())
                    continue;
            }
        }

        if (!vHeadScreen.OnScreen() && !vRootScreen.OnScreen())
        {
            if (jrt::settings.bIndicators)
                AddArrowIndicator(vHeadScreen.ToImVec2());
            continue;
        }

        if (GetServerTime == 0.0f)
        {
            Lobby = true;
        }
        else
        {
            Lobby = false;
        }

        if (bVisible)
        {
            VisibleCount++;
        }

        jrt::settings.iVisiblePlayers = VisibleCount++;
        jrt::settings.iPlayerCount = PlayerCount++;

		auto fBoxSize = ( abs( vHeadScreen.y - vRootScreen.y ) ) * 0.40;
		auto bBounds = Mesh->GetStaticBounds( vHeadScreen, vRootScreen, fBoxSize );

        ImColor iColor = bVisible ? jrt::settings.iVisibleColor : jrt::settings.iInvisibleColor;

        if (jrt::settings.bEnable)
        {
            if (jrt::settings.bUsername)
            {
                std::string Username = Actor.Username.empty() ? _s("Bot") : Actor.Username;

                if (Lobby == true)
                {
                    Username += _(" (In Lobby)");
                }

                auto vSize = ImGui::CalcTextSize(Username.c_str());

                iDrawList->AddText(ImVec2(((bBounds.left + bBounds.right) / 2) - (vSize.x / 2) - 1, bBounds.top - 20.f), ImColor(255, 255, 255), Username.c_str(), 0, jrt::settings.bTextOutline);
            }

            if (jrt::settings.bRanked)
            {
                std::string RankedString = _s("Unranked");
                uintptr_t HabaneroComponent = read<uintptr_t>((uintptr_t)Actor.PlayerState + Offset::HabaneroComponent);
                if (HabaneroComponent)
                {
                    uint32_t RankProgress = read<uint32_t>(HabaneroComponent + 0xb8 + 0x10);

                    static const std::string Ranks[] =
                    {
                        _s("Bronze 1"), _s("Bronze 2"), _s("Bronze 3"),
                        _s("Silver 1"), _s("Silver 2"), _s("Silver 3"),
                        _s("Gold 1"), _s("Gold 2"), _s("Gold 3"),
                        _s("Platinum 1"), _s("Platinum 2"), _s("Platinum 3"),
                        _s("Diamond 1"), _s("Diamond 2"), _s("Diamond 3"),
                        _s("Elite"), _s("Champion"), _s("Unreal"), _s("Unranked")
                    };

                    if (RankProgress >= 0 && RankProgress < 18)
                    {
                        RankedString = Ranks[RankProgress];
                    }
                }

                if (RankedString.find(_s("Bronze")) != std::string::npos)
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.5f, 0.2f, 1.0f));
                }
                else if (RankedString.find(_s("Silver")) != std::string::npos)
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.75f, 0.75f, 0.75f, 1.0f));
                }
                else if (RankedString.find(_s("Gold")) != std::string::npos)
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.84f, 0.0f, 1.0f));
                }
                else if (RankedString.find(_s("Platinum")) != std::string::npos)
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 1.0f, 1.0f));
                }
                else if (RankedString.find(_s("Diamond")) != std::string::npos)
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 1.0f, 1.0f));
                }
                else if (RankedString == (_s("Elite")))
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
                }
                else if (RankedString == (_s("Champion")))
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.5f, 0.0f, 1.0f));
                }
                else if (RankedString == (_s("Unreal")))
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.5f, 1.0f, 1.0f));
                }
                else
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
                }

                float fPadding = jrt::settings.bUsername ? 33.f : 21.f;
                auto vSize = ImGui::CalcTextSize(RankedString.c_str());
                ImGui::GetBackgroundDrawList()->AddText(ImVec2(((bBounds.left + bBounds.right) / 2) - (vSize.x / 2) - 1, bBounds.top - fPadding), ImColor(255, 255, 255), RankedString.c_str(), 0, jrt::settings.bTextOutline);
            }

            if (jrt::settings.bMagicBullet)
            {
                uintptr_t Instigator = read<uintptr_t>((uintptr_t)Pawn + 0x10);
                uintptr_t DefaultPawn = read<uintptr_t>(Instigator + 0x110);
                uintptr_t DefaultMesh = read<uintptr_t>(DefaultPawn + Offset::Mesh);
                uintptr_t StaticMesh = read<uintptr_t>(DefaultMesh + 0x508);
                uintptr_t Bone = read<uintptr_t>(StaticMesh + 0x338);


                FVector ModifiedHitsize = { jrt::settings.fMagicBulletSize, jrt::settings.fMagicBulletSize, jrt::settings.fMagicBulletSize };

                FVector CurrentHitsize = read<FVector>(Bone + 0x1E0 + 0x40);


                if (CurrentHitsize.x != ModifiedHitsize.x ||
                    CurrentHitsize.y != ModifiedHitsize.y ||
                    CurrentHitsize.z != ModifiedHitsize.z)
                {
                    write<FVector>(Bone + 0x1E0 + 0x40, ModifiedHitsize);
                }
            }

            if (jrt::settings.bPlatform)
            {
                auto Platform = Pawn->GetPlayerState()->GetPlatform();

                float fPadding = jrt::settings.bRanked ? 45.f : 30.f;

                if (!Platform.empty() && !Actor.Username.empty())
                {
                    if (strstr(Platform.c_str(), _("WIN")))
                    {
                        std::string Plat = _s("Windows");
                        auto vSize = ImGui::CalcTextSize(Plat.c_str());
                        iDrawList->AddText(ImVec2(((bBounds.left + bBounds.right) / 2) - (vSize.x / 2) - 1, bBounds.top - fPadding), ImColor(255, 255, 255), Plat.c_str(), 0, jrt::settings.bTextOutline);
                    }
                    else if (strstr(Platform.c_str(), _("XBL")) || strstr(Platform.c_str(), _("XSX")))
                    {
                        std::string Plat = _s("Xbox");
                        auto vSize = ImGui::CalcTextSize(Plat.c_str());
                        iDrawList->AddText(ImVec2(((bBounds.left + bBounds.right) / 2) - (vSize.x / 2) - 1, bBounds.top - fPadding), ImColor(13, 105, 13), Plat.c_str(), 0, jrt::settings.bTextOutline);
                    }
                    else if (strstr(Platform.c_str(), _("PSN")) || strstr(Platform.c_str(), _("PS5")))
                    {
                        std::string Plat = _s("Playstation");
                        auto vSize = ImGui::CalcTextSize(Plat.c_str());
                        iDrawList->AddText(ImVec2(((bBounds.left + bBounds.right) / 2) - (vSize.x / 2) - 1, bBounds.top - fPadding), ImColor(0, 130, 244), Plat.c_str(), 0, jrt::settings.bTextOutline);
                    }
                    else if (strstr(Platform.c_str(), _("SWT")))
                    {
                        std::string Plat = _s("NintendoSwitch");
                        auto vSize = ImGui::CalcTextSize(Plat.c_str());
                        iDrawList->AddText(ImVec2(((bBounds.left + bBounds.right) / 2) - (vSize.x / 2) - 1, bBounds.top - fPadding), ImColor(194, 0, 12), Plat.c_str(), 0, jrt::settings.bTextOutline);
                    }
                    else if (strstr(Platform.c_str(), _("MAC")))
                    {
                        std::string Plat = _s("MacOS");
                        auto vSize = ImGui::CalcTextSize(Plat.c_str());
                        iDrawList->AddText(ImVec2(((bBounds.left + bBounds.right) / 2) - (vSize.x / 2) - 1, bBounds.top - fPadding), ImColor(87, 110, 205), Plat.c_str(), 0, jrt::settings.bTextOutline);
                    }
                    else if (strstr(Platform.c_str(), _("LNX")))
                    {
                        std::string Plat = _s("Linux");
                        auto vSize = ImGui::CalcTextSize(Plat.c_str());
                        iDrawList->AddText(ImVec2(((bBounds.left + bBounds.right) / 2) - (vSize.x / 2) - 1, bBounds.top - fPadding), ImColor(241, 192, 37), Plat.c_str(), 0, jrt::settings.bTextOutline);
                    }
                    else if (strstr(Platform.c_str(), _("IOS")))
                    {
                        std::string Plat = _s("IOS");
                        auto vSize = ImGui::CalcTextSize(Plat.c_str());
                        iDrawList->AddText(ImVec2(((bBounds.left + bBounds.right) / 2) - (vSize.x / 2) - 1, bBounds.top - fPadding), ImColor(83, 85, 89), Plat.c_str(), 0, jrt::settings.bTextOutline);
                    }
                    else if (strstr(Platform.c_str(), _("AND")))
                    {
                        std::string Plat = _s("Android");
                        auto vSize = ImGui::CalcTextSize(Plat.c_str());
                        iDrawList->AddText(ImVec2(((bBounds.left + bBounds.right) / 2) - (vSize.x / 2) - 1, bBounds.top - fPadding), ImColor(153, 204, 0), Plat.c_str(), 0, jrt::settings.bTextOutline);
                    }
                }
            }

           /* if (1)
            {
                FVector2D bonePositionsOut[111];
                for (int i = 0; i < 111; ++i) {
                    bonePositionsOut[i] = Cached::ViewState->WorldToScreen(Mesh->GetBoneMatrix(pBoneArray, i));

                    const FVector2D& bonePosition = bonePositionsOut[i];
                    ImVec2 textPos = ImVec2(bonePosition.x, bonePosition.y);
                    ImGui::GetForegroundDrawList()->AddText(textPos, IM_COL32_WHITE, std::to_string(i).c_str());
                }
            }*/

            if (jrt::settings.bFilledBox)
            {
                ImVec2 p1 = ImVec2(bBounds.left, bBounds.top);
                ImVec2 p2 = ImVec2(bBounds.right, bBounds.bottom);
                iDrawList->AddRectFilled(p1, p2, ImColor(0, 0, 0, 100));
            }

            if (jrt::settings.bDistance)
            {
                auto sDistance = _s("") + std::to_string(int(fDistance)) + _s("m");
                auto vSize = ImGui::CalcTextSize(sDistance.c_str());
                iDrawList->AddText(ImVec2(((bBounds.left + bBounds.right) / 2) - (vSize.x / 2) - 1, bBounds.bottom + 3.f), ImColor(255, 255, 255), sDistance.c_str(), 0, jrt::settings.bTextOutline);
            }

            if (jrt::settings.bHeldWeapon)
            {
                auto WeaponData = Pawn->GetCurrentWeapon()->GetWeaponData();

                auto iBuildingState = read<uint8_t>(std::uintptr_t(Pawn) + Offset::BuildingState);

                std::string sBottomText;

                if (Cached::Pawn)
                {
                    if (iBuildingState == 2)
                    {
                        sBottomText = WeaponData->GetDisplayName();
                    }
                    else
                    {
                        sBottomText = _s("None");
                    }
                }
                else
                {
                    sBottomText = _s("Building Plan");
                }

                float fPadding = jrt::settings.bDistance ? 15.f : 3.f;
                auto vSize = ImGui::CalcTextSize(sBottomText.c_str());

                iDrawList->AddText(ImVec2(((bBounds.left + bBounds.right) / 2) - (vSize.x / 2) - 1, bBounds.bottom + 3.f + fPadding), GetWeaponDataColor(std::uintptr_t(Pawn)), sBottomText.c_str(), 0, jrt::settings.bTextOutline);
               
            }

            if (jrt::settings.bSkeletons)
            {
                for (const auto& ePair : eBonePairs)
                {
                    auto vBone1 = Cached::ViewState->WorldToScreen(Mesh->GetBoneMatrix(pBoneArray, ePair.first));
                    auto vBone2 = Cached::ViewState->WorldToScreen(Mesh->GetBoneMatrix(pBoneArray, ePair.second));

                    iDrawList->AddLine(ImVec2(vBone1.x, vBone1.y), ImVec2(vBone2.x, vBone2.y), iColor, jrt::settings.iSkeletonThickness);

                    if (jrt::settings.bOutlinedSkeleton)
                    {
                        iDrawList->AddLine(ImVec2(vBone1.x - jrt::settings.fOutlinedSize, vBone1.y - jrt::settings.fOutlinedSize), ImVec2(vBone2.x - jrt::settings.fOutlinedSize, vBone2.y - jrt::settings.fOutlinedSize), ImColor(0, 0, 0));
                    }
                }
            }

            if (jrt::settings.bBox)
            {
                if (!jrt::settings.iBoxType)
                {
                    if (jrt::settings.bBoxOutline)
                        iDrawList->AddRect(ImVec2(bBounds.left, bBounds.top), ImVec2(bBounds.right, bBounds.bottom), ImColor(0, 0, 0, 200), 2.5f, ImDrawFlags_RoundCornersAll, jrt::settings.iBoxThickness * 2.f);

                    if (jrt::settings.bFilledBox)
                        iDrawList->AddRectFilled(ImVec2(bBounds.left, bBounds.top), ImVec2(bBounds.right, bBounds.bottom), ImColor(0, 0, 0, 70), 2.5f, ImDrawFlags_RoundCornersAll);

                    iDrawList->AddRect(ImVec2(bBounds.left, bBounds.top), ImVec2(bBounds.right, bBounds.bottom), iColor, 2.5f, ImDrawFlags_RoundCornersAll, jrt::settings.iBoxThickness);
                }
                else if (jrt::settings.iBoxType)
                {
                    auto DrawCorneredBox = [&](int x, int y, int w, int h, const ImColor color, int thickness) -> void
                        {
                            float fLineWidth = (w / 4);
                            float fLineHeight = (h / 4);
                            iDrawList->AddLine(ImVec2(x, y), ImVec2(x, y + fLineHeight), color, thickness);
                            iDrawList->AddLine(ImVec2(x, y), ImVec2(x + fLineWidth, y), color, thickness);
                            iDrawList->AddLine(ImVec2(x + w - fLineWidth, y), ImVec2(x + w, y), color, thickness);
                            iDrawList->AddLine(ImVec2(x + w, y), ImVec2(x + w, y + fLineHeight), color, thickness);
                            iDrawList->AddLine(ImVec2(x, y + h - fLineHeight), ImVec2(x, y + h), color, thickness);
                            iDrawList->AddLine(ImVec2(x, y + h), ImVec2(x + fLineWidth, y + h), color, thickness);
                            iDrawList->AddLine(ImVec2(x + w - fLineWidth, y + h), ImVec2(x + w, y + h), color, thickness);
                            iDrawList->AddLine(ImVec2(x + w, y + h - fLineHeight), ImVec2(x + w, y + h), color, thickness);
                        };

                    if (jrt::settings.bBoxOutline)
                        DrawCorneredBox(bBounds.left, bBounds.top, bBounds.right - bBounds.left, bBounds.bottom - bBounds.top, ImColor(0, 0, 0, 200), jrt::settings.iBoxThickness * 1.5f);

                    if (jrt::settings.bBoxOutline)
                        iDrawList->AddRect(ImVec2(bBounds.left, bBounds.top), ImVec2(bBounds.right, bBounds.bottom), ImColor(0, 0, 0, 200), 2.5f, ImDrawFlags_RoundCornersAll, jrt::settings.iBoxThickness * 2.f);

                    if (jrt::settings.bFilledBox)
                        iDrawList->AddRectFilled(ImVec2(bBounds.left, bBounds.top), ImVec2(bBounds.right, bBounds.bottom), ImColor(0, 0, 0, 70), 2.5f, ImDrawFlags_RoundCornersAll);

                    DrawCorneredBox(bBounds.left, bBounds.top, bBounds.right - bBounds.left, bBounds.bottom - bBounds.top, iColor, jrt::settings.iBoxThickness);
                }
            }
        }

        Cached::ClosestPawn = TemporaryClosestPlayer;
        Cached::ClosestDistance = FLT_MAX;
    }
}