#include "cache.hpp"
#include <core/game/sdk.hpp>

auto jrt::cache_c::Data( ) -> void
{
	for ( ;; )
	{
		Cached::GWorld = UWorld::Get( );

		if ( !Cached::GWorld )
			continue;

        Addresses::UWorld = uintptr_t(Cached::GWorld);

		Cached::GameState = Cached::GWorld->GetGameState( );
		if ( !Cached::GameState )
			continue;

        auto GameInstance = Cached::GWorld->GetGameInstance( );

		auto LocalPlayer = GameInstance->GetLocalPlayer( );

		Cached::PlayerController = LocalPlayer->GetPlayerController( );
		Cached::ViewState = LocalPlayer->GetViewState ( );

		Cached::Pawn = Cached::PlayerController->GetPawn( );

        auto CurrentWeapon = Cached::Pawn->GetCurrentWeapon( );
        if ( CurrentWeapon )
        {
            auto iWeaponAnimation = CurrentWeapon->GetWeaponAnimation( );
            Cached::FortWeaponType = CurrentWeapon->SimplifyWeaponType( iWeaponAnimation );
        }

        std::this_thread::sleep_for( std::chrono::milliseconds( 200 ) );
	}
}

auto jrt::cache_c::Entities( ) -> void
{
    for ( ;; )
    {

        auto ActorArray = Cached::GameState->GetPlayerArray( );
        for ( int i = 0; i < ActorArray.iCount; ++i )
        {
            auto PlayerState = ActorArray.Get( i );
            auto Pawn = PlayerState->GetPawnPrivate( );
            auto Mesh = Pawn->GetMesh( );

            if ( !Pawn )
                continue;

            if ( !Mesh )
                continue;

            if ( Pawn == Cached::Pawn )
            {
                if (!jrt::settings.bShowTeam)
                {
                    Cached::TeamIndex = PlayerState->GetTeamIndex();

                    continue;
                }
           
            }

            auto aBoneArray = read<TArray<std::uintptr_t>>( ( uintptr_t )Mesh + BoneArray + ( read<int>( ( uintptr_t )Mesh + ( BoneArrayCache ) ) * 0x10 ) );
            if ( !aBoneArray.tData )
                continue;

            Actor CachedActor{ Pawn, PlayerState, Mesh, reinterpret_cast< std::uintptr_t >( aBoneArray.tData), PlayerState->GetPlayerName() };
            TemporaryActorList.insert( TemporaryActorList.end( ), CachedActor );
        }

        ActorList.swap( TemporaryActorList );
        TemporaryActorList.clear( );
        std::this_thread::sleep_for( std::chrono::seconds( 2 ) );
    }
}