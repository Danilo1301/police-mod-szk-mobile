#pragma once

#include <mod/amlmod.h>

#include "simpleGta.h"

// Ponteiros de função globais
inline RpClump* (*RpClumpForAllAtomics)(RpClump* clump, RpAtomicCallBack callback, void* pData) = nullptr;
inline RpGeometry* (*RpGeometryForAllMaterials)(RpGeometry* geometry, RpMaterialCallBack fpCallBack, void* pData) = nullptr;
inline char* (*GetFrameNodeName)(RwFrame* frame) = nullptr;
inline void (*CSprite2d_DrawSprite)(CSprite2d*, CRect const&, CRGBA const&) = nullptr;
inline void (*TransformRealWorldPointToRadarSpace)(CVector2D& out, CVector2D const& in) = nullptr;
inline void (*TransformRadarPointToScreenSpace)(CVector2D& out, CVector2D const& in) = nullptr;
inline void (*LimitRadarPoint)(CVector2D& in) = nullptr;

// Variáveis globais
inline bool* m_UserPause = nullptr;
inline bool* m_CodePause = nullptr;
inline float flMenuMapScaling = 0.0f;
inline RsGlobalType* RsGlobal = nullptr;

void DoHooks();