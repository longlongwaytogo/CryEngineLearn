#pragma once

#include "Player/ISimpleActor.h"

#include <CryMath/Cry_Camera.h>
#include <IGameObject.h>

class CPlayerInput;
class CPlayerMovement;
class CPlayerView;

class CPlayer;

class CSpawnPoint;

struct ISimpleWeapon;

//////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////
// Represents a player participating in gameplay
////////////////////////////////////////////////////////
class CPlayer 
	: public CGameObjectExtensionHelper<CPlayer, ISimpleActor>
{
public:
	CPlayer();
	virtual ~CPlayer();

	// RMI test param
	struct RMIParam
	{
		RMIParam()
		{}
		RMIParam(Vec3& v)
		{
			dir.x = v.x;
			dir.y = v.y;
			dir.z = v.z;
			strength = 4;

		}

		void SerializeWith(TSerialize ser)
		{
			ser.Value("dir", dir, 'dir1');
			ser.Value("strength", strength, 'iii');
		}

		Vec3 dir;
		float strength;
	};


	void RequestDoSomething()
	{
		gEnv->pLog->Log("do something ...");
	}
	DECLARE_SERVER_RMI_NOATTACH(DoRequestDoSomething, RMIParam, eNRT_ReliableOrdered);
	

	// ISimpleActor
	virtual bool Init(IGameObject* pGameObject) override;
	virtual void PostInit(IGameObject* pGameObject) override;
	virtual void ProcessEvent(SEntityEvent& event) override;
	virtual void Update(SEntityUpdateContext& ctx, int updateSlot) override;

	virtual void SetHealth(float health) override;
	// ~ISimpleActor

	CPlayerInput *GetInput() const { return m_pInput; }
	CPlayerMovement *GetMovement() const { return m_pMovement; }

protected:
	CPlayerInput *m_pInput;
	CPlayerMovement *m_pMovement;
	CPlayerView *m_pView;
	ITexture	*m_pTexture;
};
