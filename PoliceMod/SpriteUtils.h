#pragma once

#include "pch.h"
#include "menuSZK/IMenuSZK.h"

extern bool IsGamePaused();
extern void (*CSprite2d_DrawSprite)(CSprite2d*, CRect const&, CRGBA const&);
extern void (*TransformRealWorldPointToRadarSpace)(CVector2D& out, CVector2D const& in);
extern void (*TransformRadarPointToScreenSpace)(CVector2D& out, CVector2D const& in);
extern void (*LimitRadarPoint)(CVector2D& in);
extern float flMenuMapScaling;

class SpriteUtils {
public:
    static ISprite* spriteBigCircle;
    static ISprite* spriteCircle;
    static ISprite* spriteBlip;
    static ISprite* spritePoliceDepartment;
    
    static void OnRender();

    static void DrawSpriteInWorld(ISprite* sprite, CVector worldPos, float size, CRGBA color);
    static void DrawSpriteInRadarWorld(ISprite* sprite, CVector worldPos, CRGBA color, float worldSize);
    static void DrawSpriteInRadar(ISprite* sprite, CVector worldPos, CRGBA color, float size);
    static void DrawBlip(CVector worldPos, CRGBA color);
    static float WorldUnitsToPixels(float worldUnits);
};
