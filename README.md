# Unreal5BasicSteamLobby
Minimal working project that utilizes steam subsystems to connect two people over P2P via steam services
Entire logic is contained within this [file](https://github.com/Riztazz/Unreal5BasicSteamLobby/blob/master/Source/NetworkTest/MyGameInstanceSubsystem.cpp)
- In order to run it with steam sockets, enable steam socket plugin in editor and replace/add NetDriverDefinition settings under [/Script/Engine.GameEngine]

ie:
[/Script/Engine.GameEngine]
!NetDriverDefinitions=ClearArray
+NetDriverDefinitions=(DefName="GameNetDriver",DriverClassName="/Script/SteamSockets.SteamSocketsNetDriver",DriverClassNameFallback="/Script/SteamSockets.SteamNetSocketsNetDriver")

As per unreal docs ( https://docs.unrealengine.com/4.26/en-US/InteractiveExperiences/Networking/HowTo/SteamSockets/ )

Notes worth considering;
- When you implement steam lobbies and you can detect other sessions but fail to join them (timeout on client travel), check if your map has ?listen option
it is required for p2p connection
- When searching for lobbies, SEARCH_PRESENCE key setting is required to find other lobbies

How to run for nonunreal people:
- This project has been tested using Visual Studio 2022 and Unreal Engine 5.0.0.
  Right click on NetworkTest.uproject and Generate visual studio project files, once completed open NetworkTest.sln using Visual Studio 2022.
  In the upper corner, select build tab and build the solution, once that completes you can launch the uproject file

-- if you found it useful, consider buying me a coffee :) Thanks!
https://www.buymeacoffee.com/Riztazz
