#include "StdAfx.h"
#include "Player.h"

#include "Movement/PlayerMovement.h"
#include "Input/PlayerInput.h"
#include "View/PlayerView.h"

#include "GamePlugin.h"
#include "Game/GameRules.h"

#include <CryRenderer/IRenderAuxGeom.h>



#include "CrySystem/ISystem.h"
#include "CryRenderer/IRenderer.h"
#include "CryRenderer/IRenderAuxGeom.h"


//enum GameUserPacketDefinitions
//{
//	eGUPD_LobbyStartCountdownTimer = CRYLOBBY_USER_PACKET_START,
//	eGUPD_LobbyGameHasStarted,
//	eGUPD_LobbyEndGame,
//	eGUPD_LobbyEndGameResponse,
//	eGUPD_LobbyUpdatedSessionInfo,
//	eGUPD_LobbyMoveSession,
//	eGUPD_SquadJoin,
//	eGUPD_SquadJoinGame,
//	eGUPD_SquadNotInGame,
//	eGUPD_SetTeamAndRank,
//	eGUPD_SendChatMessage,								// Clients request to send a message to other players
//	eGUPD_ChatMessage,										// Server sent message to all appropriate other players.
//	eGUPD_ReservationRequest,							// Sent by squad leader client after joined game to identify self as leader and to tell the game server to reserve slots for its members
//	eGUPD_ReservationClientIdentify,			// Sent by clients after joined game to identify self to game server so it can check if client passes reserve checks (if any)
//	eGUPD_ReservationsMade,								// Sent to a squad leader by a server when requested reservations have been successfully made upon receipt of a eGUPD_ReservationRequest packet 
//	eGUPD_ReservationFailedSessionFull,		// Can be sent to clients when there's no room for them in a game session. Generally causes them to "kick" themselves by deleting their own session
//	eGUPD_SyncPlaylistContents,						// Sync entire playlist
//	eGUPD_SetGameVariant,
//	eGUPD_SyncPlaylistRotation,
//	eGUPD_SquadLeaveGame,									// Squad: Tell all members in the squad to leave (game host will leave last)
//	eGUPD_SquadNotSupported,							// Squads not suported in current gamemode
//	eGUPD_UpdateCountdownTimer,
//	eGUPD_RequestAdvancePlaylist,
//	eGUPD_SyncExtendedServerDetails,
//	eGUPD_DetailedServerInfoRequest,
//	eGUPD_DetailedServerInfoResponse,
//	eGUPD_SquadDifferentVersion,					// Response to SquadJoin packet sent when the client is on a different patch
//	eGUPD_SquadKick,
//	eGUPD_SquadMerge,
//	eGUPD_SetupDedicatedServer,
//	eGUPD_SetupJoinCommand,
//	eGUPD_SetAutoStartingGame,
//	eGUPD_SetupArbitratedLobby,
//	eGUPD_TeamBalancingSetup,
//	eGUPD_TeamBalancingAddPlayer,
//	eGUPD_TeamBalancingRemovePlayer,
//	eGUPD_TeamBalancingUpdatePlayer,
//
//	eGUPD_End
//};
//


class CPlayerRegistrator
	: public IEntityRegistrator
{
	virtual void Register() override
	{
		CGamePlugin::RegisterEntityWithDefaultComponent<CPlayer>("Player");
	}

	virtual void Unregister() override {}
};

CPlayerRegistrator g_playerRegistrator;

CPlayer::CPlayer()
	: m_pInput(nullptr)
	, m_pMovement(nullptr)
	, m_pView(nullptr)
{
}

CPlayer::~CPlayer()
{
	gEnv->pGameFramework->GetIActorSystem()->RemoveActor(GetEntityId());
}
struct SNanoSuitEvent
{
	int event;
	float fParam;
};
IMPLEMENT_RMI(CPlayer, DoRequestDoSomething)
{
	RMIParam RMIP;
	RMIP.dir = Vec3(2.2f, 3.3f, 4.4f);

	
	//this->SendActorSuitEvent(suitEvent);

	return true;    // Always return true - false will drop connection
}

bool CPlayer::Init(IGameObject *pGameObject)
{
	SetGameObject(pGameObject);
	// test lobby
	{
		ECryLobbyServiceFeatures features = ECryLobbyServiceFeatures(eCLSO_All & ~(eCLSO_Voice));
		bool isDedicated = gEnv->IsDedicated();
		//ECryLobbyError error;
		gEnv->pNetwork->SetMultithreadingMode(INetwork::NETWORK_MT_PRIORITY_NORMAL);
		if (!gEnv->pSystem->InitializeEngineModule("CryLobby", "EngineModule_CryLobby", false))
		{
			CryWarning(VALIDATOR_MODULE_GAME, VALIDATOR_ERROR, "Error creating Lobby System!");
		}

		ICryLobby* pLobby = gEnv->pNetwork->GetLobby();
		if (pLobby)
		{
			//pLobby->SetUserPacketEnd(eGUPD_End);

		//	{
		//		error = pLobby->Initialise(eCLS_LAN, features, CGameBrowser::ConfigurationCallback, CGameBrowser::InitialiseCallback, this);
		//		CRY_ASSERT_MESSAGE(error == eCLE_Success, "Failed to initialize LAN lobby service");
		//	}


		//
		//		error = pLobby->Initialise(eCLS_Online, features, CGameBrowser::ConfigurationCallback, CGameBrowser::InitialiseCallback, this);
		//	

		//	//CRY_ASSERT_MESSAGE( error == eCLE_Success, "Failed to initialize online lobby service" );
		//	CryWarning(VALIDATOR_MODULE_GAME, VALIDATOR_COMMENT, "Online Lobby not supported in the default SDK.");
		//	m_pSquadManager = new CSquadManager();    // MUST be done before game browser is constructed

		//	m_pGameBrowser = new CGameBrowser();

		//	//Set the matchmaking version based on the build version if g_matchmakingversion is a default value
		//	if (!gEnv->IsEditor() && g_pGameCVars->g_MatchmakingVersion <= 1)
		//	{
		//		const int internalBuildVersion = gEnv->pSystem->GetBuildVersion().v[0];
		//		CryLog("BuildVersion %d", internalBuildVersion);
		//		if (internalBuildVersion != 1)
		//		{
		//			g_pGameCVars->g_MatchmakingVersion = internalBuildVersion;
		//		}
		//	}

		//	CGameBrowser::InitLobbyServiceType();

		//	m_pGameLobbyManager = new CGameLobbyManager();
		}
		
	}


	return pGameObject->BindToNetwork();
}

void CPlayer::PostInit(IGameObject *pGameObject)
{
	m_pMovement = static_cast<CPlayerMovement *>(GetGameObject()->AcquireExtension("PlayerMovement"));
	m_pInput = static_cast<CPlayerInput *>(GetGameObject()->AcquireExtension("PlayerInput"));

	m_pView = static_cast<CPlayerView *>(GetGameObject()->AcquireExtension("PlayerView"));

	// Register with the actor system
	gEnv->pGameFramework->GetIActorSystem()->AddActor(GetEntityId(), this);


	{
		//GetGameObject()->:GetProxy(ENTITY_PROXY_USER)
		IEntity* pEntity = GetEntity();
		IGameObject* pGameObj = GetGameObject();

		INetwork* pNetwork = gEnv->pNetwork;
		IEntityComponent* pObj2 = pEntity->GetProxy(ENTITY_PROXY_USER);

		if (pGameObj == pObj2)
		{
			gEnv->pLog->Log("0");
			
		//	pGameObj->DoInvokeRMI()
		}
		else
		{
			gEnv->pLog->Log("-1");

		}
	   
	}
	// Make sure that this extension is updated regularly via the Update function below
	pGameObject->EnableUpdateSlot(this, 0);

}

void CPlayer::ProcessEvent(SEntityEvent& event)
{
	switch (event.event)
	{
		case ENTITY_EVENT_RESET:
		{
			if (event.nParam[0] == 1)
			{
				// Make sure to revive player when respawning in Editor
				SetHealth(GetMaxHealth());
				GetGameObject()->InvokeRMI(CPlayer::DoRequestDoSomething(), CPlayer::RMIParam(Vec3(1.0f,20.0f,30.0f)), eRMI_ToClientChannel, GetChannelId());

			}
		}
		break;
	}
}

void CPlayer::SetHealth(float health)
{
	if (health > 0)
	{
		m_pInput->OnPlayerRespawn();
	}
}

void CPlayer::Update(SEntityUpdateContext& ctx, int updateSlot)
{

	gEnv->pRenderer->GetIRenderAuxGeom()->DrawSphere(GetEntity()->GetPos() + GetEntity()->GetForwardDir()*1.52, 0.3, ColorB(0x00, 0xff, 0x00, 0xff), false);


}