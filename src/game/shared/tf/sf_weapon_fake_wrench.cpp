//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: We need to pretend to be wrench to do stuffs
//
//=============================================================================

#include "cbase.h"
#include "sf_weapon_fake_wrench.h"
#include "cdll_int.h"

// Client specific.
#ifdef CLIENT_DLL
#include "c_tf_player.h"
// NVNT haptics system interface
#include "haptics/ihaptics.h"
// Server specific.
#else
#include "tf_player.h"
#endif

//=============================================================================
//
// Weapon FakeWrench tables.
//
IMPLEMENT_NETWORKCLASS_ALIASED( SFFakeWrench, DT_SFWeaponFakeWrench )

BEGIN_NETWORK_TABLE( CSFFakeWrench, DT_SFWeaponFakeWrench )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CSFFakeWrench )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( sf_weapon_fake_wrench, CSFFakeWrench );
PRECACHE_WEAPON_REGISTER( sf_weapon_fake_wrench );

//=============================================================================
//
// Weapon FakeWrench functions.
//

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CSFFakeWrench::CSFFakeWrench()
{}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CSFFakeWrench::Spawn()
{
	BaseClass::Spawn();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CSFFakeWrench::Smack( void )
{
	// see if we can hit an object with a higher range

	// Get the current player.
	CTFPlayer *pPlayer = GetTFPlayerOwner();
	if ( !pPlayer )
		return;

	if ( !CanAttack() )
		return;

	// Setup a volume for the melee weapon to be swung - approx size, so all melee behave the same.
	static Vector vecSwingMins( -18, -18, -18 );
	static Vector vecSwingMaxs( 18, 18, 18 );

	// Setup the swing range.
	Vector vecForward;
	AngleVectors( pPlayer->EyeAngles(), &vecForward );
	Vector vecSwingStart = pPlayer->Weapon_ShootPosition();
	Vector vecSwingEnd = vecSwingStart + vecForward * 70;

	// only trace against objects

	// See if we hit anything.
	trace_t trace;

	CTraceFilterIgnorePlayers traceFilter( NULL, COLLISION_GROUP_NONE );
	UTIL_TraceLine( vecSwingStart, vecSwingEnd, MASK_SOLID, &traceFilter, &trace );
	if ( trace.fraction >= 1.0 )
	{
		UTIL_TraceHull( vecSwingStart, vecSwingEnd, vecSwingMins, vecSwingMaxs, MASK_SOLID, &traceFilter, &trace );
	}

	// We hit, setup the smack.
	if ( trace.fraction < 1.0f &&
		 trace.m_pEnt &&
		 trace.m_pEnt->IsBaseObject() &&
		 trace.m_pEnt->GetTeamNumber() == pPlayer->GetTeamNumber() )
	{
#ifdef GAME_DLL
		OnFriendlyBuildingHit( dynamic_cast<CBaseObject*>(trace.m_pEnt), pPlayer, trace.endpos );
#else
		// NVNT if the local player is the owner of this wrench 
		//   Notify the haptics system we just repaired something.
		if (pPlayer == C_TFPlayer::GetLocalTFPlayer() && haptics)
			haptics->ProcessHapticEvent(2, "Weapons", "tf_weapon_wrench_fix");
#endif
	}
	else
	{
		// if we cannot, Smack as usual for player hits
		BaseClass::Smack();
	}
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
#ifdef GAME_DLL
void CSFFakeWrench::OnFriendlyBuildingHit( CBaseObject *pObject, CTFPlayer *pPlayer, Vector hitLoc )
{
	bool bUsefulHit = false;

	if ( !pObject->HasSapper() )
		bUsefulHit = ( pObject->Command_Repair( pPlayer, GetRepairAmount(), 1.f) > 0 );

	CDisablePredictionFiltering disabler;

	if ( bUsefulHit )
	{
		pObject->DoWrenchHitEffect( hitLoc, true, false );
		// play success sound
		WeaponSound( SPECIAL1 );
	}
	else
	{
		if ( pObject->IsDisposableBuilding() )
		{
			CSingleUserRecipientFilter singleFilter( pPlayer );
			EmitSound( singleFilter, pObject->entindex(), "Player.UseDeny" );
		}
		else
		{
			// play failure sound
			WeaponSound( SPECIAL2 );
		}
	}
}
#endif // GAME_DLL

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
float CSFFakeWrench::GetRepairAmount( void )
{
	float flRepairAmount = 0.f;
	CALL_ATTRIB_HOOK_FLOAT( flRepairAmount, sf2_set_can_repair_buildings );

	return flRepairAmount;
}