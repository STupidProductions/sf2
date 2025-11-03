//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================

#ifndef SF_WEAPON_FAKE_WRENCH_H
#define SF_WEAPON_FAKE_WRENCH_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_weaponbase_melee.h"

#ifdef CLIENT_DLL
#define CSFFakeWrench C_SFFakeWrench
#endif

//=============================================================================
//
// FakeWrench class.
//
class CSFFakeWrench : public CTFWeaponBaseMelee
{
public:

	DECLARE_CLASS( CSFFakeWrench, CTFWeaponBaseMelee );
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

	CSFFakeWrench();

	virtual void		Spawn();
	virtual int			GetWeaponId( void ) const			{ return SF_WEAPON_FAKE_WRENCH; }
	virtual void		Smack( void );
	float				GetRepairAmount( void );
#ifdef GAME_DLL
	void				OnFriendlyBuildingHit( CBaseObject* pObject, CTFPlayer* pPlayer, Vector hitLoc );
#endif

private:

	CSFFakeWrench( const CSFFakeWrench & ) {}
};
#endif // SF_WEAPON_FAKE_WRENCH_H