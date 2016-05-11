/***********************************************************************
History
01 Aug 2008 -- Zhang Dong @1
- Create.
************************************************************************************/
#pragma once

#pragma pack(push, 1)
typedef struct 
{
    double dx;
    double dy;
} TBL_POS;

typedef struct
{
	short       nWireNum;
    TBL_POS     stTchPos[2]; //Teach position
    short       nRefNum[2];
    short       nLoopMode;
    char        cReserve[100];
} WIRE_RECORD;

typedef struct
{
	short       nRefNum;
    short       nRefType;
    bool        bCtactLvlValid;
    long        lCtactLvl;
    TBL_POS     stManuAlignPos[2];     //Manual alignment position
    TBL_POS     stVisionAlignPos[2];   //Vision Alignment position
    char        cReserve[100];
} REF_RECORD;

typedef struct
{
	short       nMatrixNum;
    TBL_POS     stVisionAlignPos[2];   //Vision Alignment position
    TBL_POS     stOriginalCenterPos;
    TBL_POS     stUpdatedCenterPos;
    double      dSinAln;
    double      dCosAln;
    char        cReserve[100];
} MATRIX_RECORD;

typedef struct
{
	short       nPgmVersion;           //Program version for load/save bond recipe
    bool        bPgmValid;
    short       nTotalWireNum;
    short       nTotalRefNum;
    short       nTailLengthUm;
    char        cReserve[100];
} PGM_RECORD;

typedef struct
{
	short       nMcDataVersion;        //Machine data version for load/save bond recipe
    long        lResetLevel;
    TBL_POS     stBondTipOs;
    char        cReserve[100];
} MACHINE_DATA;

typedef struct
{
    short nStandbyPower[2];
    short nCtactTime[2];
    short nCtactPower[2];
    short nCtactForce[2];
    short nBaseTime[2];
    short nBasePower[2];
    short nBaseForce[2];
    short nReleaseTime[2];
    short nReleasePower[2];
    short nReleaseForce[2];
    short nPowerFactor;
    short nForceFactor;
    short nSrchHeight1;
    short nSrchSpeed1;
    char  cReserve[100];
} BOND_PARA_RCD;

typedef struct
{
    double dLoopHeight;
    double dRevHeight1;
    double dRevDistance1;
    double dRevAngle1;
    double dRevDist1Height;
    double dRevDist1Length;
    double dKink1Height;
    bool   bKink2Enable;
    double dRevHeight2;
    double dRevDistance2;
    double dRevAngle2;
    double dRevDist2Height;
    double dRevDist2Length;
    double dKink2Height;
    double dSpanLength;
    double dSlopeLength;
    double dTrajRadius;
    bool   bAbove1stBondEnable;
    double dAbove1stBondHeight;
    double dAbove1stBondLength;
    short  nTrajPoint;
    short  nSrchHeight2;
    short  nSrchSpeed2;

} LOOP_SHAPE_PARA;

typedef struct
{
    short nKinkNum;
    short nLoopHeight;
    short nRevDist;
    short nRevAngle;
    short nLoopHtCorrect;
    short nSpanLength;
    short nSpanLengthUnit; //0: %; 1: um
    short nKink2HtFactor;
    short nPullDist;
    short nAboveLoopTop;
    short nSrchHeight2;
    short nSrchSpeed2;
} LOOP_MODE_BASE_PARA;

typedef struct
{
    LOOP_MODE_BASE_PARA stLoopBasePara;
} LOOP_MODE_PARA;

typedef struct
{
    short nSrchSpeed1;
    short nSrchSpeed2;
    short nSrchTol1;
    short nSrchTol2;
    short nStandbyPower[2];
    short nCtactTime[2];
    short nCtactPower[2];
    short nCtactForce[2];
    short nBaseTime[2];
    short nBasePower[2];
    short nBaseForce[2];
    short nReleaseTime[2];
    short nReleasePower[2];
    short nReleaseForce[2];
    bool  bKink2Enable;
    bool  bAbove1stBondEnable;
    long  lSrchHt1Lvl;
    long  lRevHeight1;
    long  lRevDist1Height;
    long  lRevHeight2;
    long  lRevDist2Height;
    long  lSlopeLength;
    long  lAboveLoopTopHt;
    long  lTrajHeight;
    long  lTailHeight;
    long  lResetLvl;
    TBL_POS stBondPos[2];
    TBL_POS stKink1Pos;
    TBL_POS stKink2Pos;
    TBL_POS stLoopTopPos;
    TBL_POS stAboveLoopTopPos;
    TBL_POS stTrajEndPos;
    TBL_POS stPullPos;
    short   nTrajPoint;
    double  adTrajPos[300];

} BOND_INFO_RCD;

#pragma pack(pop)