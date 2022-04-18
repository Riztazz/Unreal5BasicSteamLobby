
#include "MyGameInstanceSubsystem.h"
#include "OnlineSessionSettings.h"

// Notes: The UpdateState is not necessary so please ignore it, it's just for me to visualize it in world
void UMyGameInstanceSubsystem::Initialize( FSubsystemCollectionBase& Collection )
{
    Super::Initialize( Collection );

    IOnlineSubsystem* SubSystem = IOnlineSubsystem::Get();
    m_sessionInterface = SubSystem->GetSessionInterface();
    // Bind network callbacks to our functions, make sure you do not place this in constructor due to how unreal builds objects
    if ( m_sessionInterface.IsValid() )
    {
        m_sessionInterface->OnCreateSessionCompleteDelegates.AddUObject( this, &UMyGameInstanceSubsystem::OnCreateSessionComplete );
        m_sessionInterface->OnDestroySessionCompleteDelegates.AddUObject( this, &UMyGameInstanceSubsystem::OnDestroySessionComplete );
        m_sessionInterface->OnFindSessionsCompleteDelegates.AddUObject( this, &UMyGameInstanceSubsystem::OnFindSessionsComplete );
        m_sessionInterface->OnJoinSessionCompleteDelegates.AddUObject( this, &UMyGameInstanceSubsystem::OnJoinSessionsComplete );
    }
}

void UMyGameInstanceSubsystem::StartMatchmakingProcess()
{
    if ( m_sessionInterface )
    {
        UpdateState( Matchmaking );
        m_sessionSearch = MakeShareable( new FOnlineSessionSearch() );

        m_sessionSearch->bIsLanQuery = false;
        m_sessionSearch->MaxSearchResults = 2000;
        //m_sessionSearchResult->PingBucketSize = 250;
        // this option is necessary when using lobbies
        m_sessionSearch->QuerySettings.Set( SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals );

        auto localPlayer = GetGameInstance()->GetLocalPlayerByIndex(0);
        if ( !localPlayer )
        {
            UE_LOG( LogTemp, Warning, TEXT( "FindMatchingSessions: Failed to get local player" ) );
            return;
        }

        auto userId = localPlayer->GetPreferredUniqueNetId();
        m_sessionInterface->FindSessions( *userId.GetUniqueNetId(), m_sessionSearch.ToSharedRef() );
    }
    else
    {
        UE_LOG( LogTemp, Warning, TEXT( "Find matching sessions called with invalid m_sessionInterface pointer" ) );
    }
}

void UMyGameInstanceSubsystem::UpdateState( TestNetworkState state )
{
    OnNetworkStateChangedDelegate.Broadcast( state );
    m_state = state;
}

void UMyGameInstanceSubsystem::OnFindSessionsComplete( bool Result )
{
    UE_LOG( LogTemp, Warning, TEXT( "OnFindSessionsComplete: Result %u and searchresults num: %u" ), Result, m_sessionSearch ? m_sessionSearch->SearchResults.Num() : 0 );
    // if we didnt fail the find session and we have any search results
    if ( Result && m_sessionSearch && m_sessionSearch->SearchResults.Num() )
    {
        UpdateState( Joining );
        // join first available lobby
        auto const& lobby = m_sessionSearch->SearchResults[0];
        auto localPlayer = GetGameInstance()->GetLocalPlayerByIndex( 0 );
        if ( !localPlayer )
        {
            UE_LOG( LogTemp, Warning, TEXT( "OnFindSessionsComplete: Failed to get local player" ) );
            return;
        }

        auto userId = localPlayer->GetPreferredUniqueNetId();
        m_sessionInterface->JoinSession( *userId.GetUniqueNetId(), NetworkTestSessionName, lobby );
    }
    else // create session and await joins
    {
        UpdateState( CreatingSession );
        UE_LOG( LogTemp, Warning, TEXT( "Find sessions failed with result: %u and sessesionSearch num: %u" ),
                Result, m_sessionSearch ? m_sessionSearch->SearchResults.Num() : 0 );

        FOnlineSessionSettings SessionSettings;
        SessionSettings.bIsLANMatch = false;
        SessionSettings.NumPublicConnections = 5;
        SessionSettings.bShouldAdvertise = true;
        SessionSettings.bUsesPresence = true;
        SessionSettings.bUseLobbiesIfAvailable = true;
        SessionSettings.Set( TEXT("NetworkTestSessionNameKey"), NetworkTestSessionName.ToString(), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
        m_sessionInterface->CreateSession( 0, NetworkTestSessionName, SessionSettings );

        UE_LOG( LogTemp, Warning, TEXT( "OnFindSessionComplete: creating session and awaiting joins" ) );
    }
}

void UMyGameInstanceSubsystem::Deinitialize()
{
    if ( m_sessionInterface )
        m_sessionInterface->DestroySession( NetworkTestSessionName );

    Super::Deinitialize();
}

void UMyGameInstanceSubsystem::OnJoinSessionsComplete( FName SessionName, EOnJoinSessionCompleteResult::Type result )
{
    UE_LOG( LogTemp, Warning, TEXT( "OnJoinSessionsComplete: Result of session (%s) is: %u " ), *SessionName.ToString(), result );

    if ( result == EOnJoinSessionCompleteResult::Success )
    {
        // Get the url for that session f﻿irst
        FString Url;
        if ( !m_sessionInterface->GetResolvedConnectString( NetworkTestSessionName, Url ) )
        {
            UE_LOG( LogTemp, Warning, TEXT( "OnJoinSessionsComplete: Failed to resolve NetworkTestSessionName to valid URL!" ) );
            return;
        }

        UE_LOG( LogTemp, Warning, TEXT( "OnJoinSessionsComplete: Traveling to URL: %s" ), *Url );

        APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
        PlayerController->ClientTravel( Url, ETravelType::TRAVEL_Absolute );
    }
    else
        check( false );
}

void UMyGameInstanceSubsystem::OnCreateSessionComplete( FName SessionName, bool Result )
{
    if ( Result )
    {
        UE_LOG( LogTemp, Warning, TEXT( "OnCreateSessionComplete: %s successfuly created" ), *SessionName.ToString() );
        UpdateState( Waiting );
    }
    else
        check( false );
}

void UMyGameInstanceSubsystem::OnDestroySessionComplete( FName SessionName, bool Result )
{
    UE_LOG( LogTemp, Warning, TEXT( "%s destruction result %u" ), *SessionName.ToString(), Result );
}
