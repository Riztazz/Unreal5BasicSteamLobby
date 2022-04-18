
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"

#include "MyGameInstanceSubsystem.generated.h"

UENUM( BlueprintType )
enum TestNetworkState
{
    None = 0,
    Matchmaking,
    CreatingSession,
    Joining,
    Waiting
};

static const FName NetworkTestSessionName( TEXT( "NetworkTestSessionName" ) );

class FOnlineSessionSearch;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam( FTestNetworkStateUpdateDelegate, TestNetworkState, state );

UCLASS()
class NETWORKTEST_API UMyGameInstanceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
    void Initialize( FSubsystemCollectionBase& Collection ) override;
    void Deinitialize();
    void OnCreateSessionComplete( FName SessionName, bool Result );
    void OnDestroySessionComplete( FName SessionName, bool Result );
    void OnFindSessionsComplete( bool Result );
    void OnJoinSessionsComplete( FName SessionName, EOnJoinSessionCompleteResult::Type );

    UFUNCTION( BlueprintCallable )
        void StartMatchmakingProcess();

    UPROPERTY( BlueprintAssignable, BlueprintReadWrite )
        FTestNetworkStateUpdateDelegate OnNetworkStateChangedDelegate;

    TestNetworkState GetNetworkState() const { return m_state; }

protected:

    void UpdateState( TestNetworkState  state );

private:
    TestNetworkState m_state;
    IOnlineSessionPtr m_sessionInterface;
    TSharedPtr<FOnlineSessionSearch> m_sessionSearch;
};
