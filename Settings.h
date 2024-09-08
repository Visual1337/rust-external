#pragma once

#define m_localplayer_c 61454664
#define BaseGameMode 0x39DC0F8 //"Signature": "System_Collections_Generic_List_BaseGameMode__c*"         DONE
#define TodSkyy 0x39859A8 // "Signature": "TOD_Sky_c*"         DONE
#define ConVar_Client_c 0x39F2040 // ConVar_Client_c*         DONE
#define ConVar_Admin_c 0x39CF1A0 // ConVar_Admin_c*         DONE
#define oFovConVarGraphics 0x3982F08 // "Signature": "ConVar_Graphics_c*"         DONE
#define OclusionCulling 0x39D47B8 // "Signature": "OcclusionCulling_c*"         DONE
#define oConVar_Culling 0x39FC620 // "Signature": "ConVar_Culling_c*"         DONE
#define oPlayerMovement 0x6B8 // 0x4E8 // public BaseMovement movement;         DONE
#define oActiveUID 0x7F0 // private uint clActiveItem;         DONE
#define oPlayerInventory 0x900 // public PlayerInventory inventory;         DONE
#define oContainerBelt 0x28 // public ItemContainer containerBelt         DONE
#define oBeltContentList 0x40 // public List<Item> contents   or   public List<int> availableSlots;       DONE
#define oHeld 0xB0 // private EntityRef heldEntity             DONE
#define oRecoilProperties 0x2F0 // public RecoilProperties recoil;             DONE
#define oRecoilYawMin 0x18  // public float recoilYawMin;        DONE
#define oRecoilYawMax 0x1C  // public float recoilYawMax;         DONE
#define oRecoilPitchMin 0x20 // public float recoilPitchMin;        DONE
#define oRecoilPitchMax 0x24 // public float recoilPitchMax;             DONE
#define oAimconeCurveScale 0x60 // public float aimconeCurveScale;       DONE
#define oStancePenalty 0x344 //private float stancePenalty;
#define oAimCone 0x300 //public float aimCone;
#define oHipAimCone 0x304 //public float hipAimCone;
#define oAimconePenaltyPerShot 0x308 //public float aimconePenaltyPerShot;
#define oAutomatic 0x2A0 // public bool automatic;
#define oFastCompoundBow 0x3B8    // public float stringHoldDurationMax; // 0x378
#define capsuleHeight 0x68 // public float capsuleHeight
#define capsuleCenter 0x6C // public float capsuleCenter
#define groundAngle 0xC4 // groundAngle
#define groundAngleNew 0xC8 // groundAngleNew
#define oGravityMultiplier 0x84 // public float gravityMultiplier;
#define oAttackRadius 0x2A4 // public float attackRadius;
#define oisAutomatic 0x2A8// public bool isAutomatic;
#define omaxDistance 0x2A0 // public float maxDistance;
#define oBlockSprintOnAttack 0x2A9 // public bool blockSprintOnAttack;
#define oonlyThrowAsProjectile 0x2E3 // public bool onlyThrowAsProjectile;
#define othrowReady 0x2E0 // private bool throwReady;
#define oPlayerModel 0x690 // public PlayerModel playerModel;