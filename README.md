# Unreal5BasicSteamLobby
Minimal working project that utilizes steam subsystems to connect two people over P2P via steam services

- In order to run it with steam sockets, enable steam socket plugin in editor and replace/add NetDriverDefinition settings under [/Script/Engine.GameEngine]

ie:
[/Script/Engine.GameEngine]
!NetDriverDefinitions=ClearArray
+NetDriverDefinitions=(DefName="GameNetDriver",DriverClassName="/Script/SteamSockets.SteamSocketsNetDriver",DriverClassNameFallback="/Script/SteamSockets.SteamNetSocketsNetDriver")

As per unreal docs ( https://docs.unrealengine.com/4.26/en-US/InteractiveExperiences/Networking/HowTo/SteamSockets/ )
