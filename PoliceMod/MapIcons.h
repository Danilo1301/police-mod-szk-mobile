#pragma once

#include "pch.h"
#include "menuSZK/IMenuSZK.h"

static ISprite* spriteBigCircle = nullptr;
static ISprite* spriteCircle = nullptr;

extern bool IsGamePaused();
extern void (*CSprite2d_DrawSprite)(CSprite2d*, CRect const&, CRGBA const&);
extern void (*TransformRealWorldPointToRadarSpace)(CVector2D& out, CVector2D const& in);
extern void (*TransformRadarPointToScreenSpace)(CVector2D& out, CVector2D const& in);
extern void (*LimitRadarPoint)(CVector2D& in);
extern float flMenuMapScaling;

class MapIcons {
public:
    static void OnDrawRadar();
};

// Converte um tamanho em unidades do mundo para pixels no radar / mapa
static float WorldUnitsToPixels(float worldUnits)
{
    CVector2D base(0.0f, 0.0f);
    CVector2D offset(worldUnits, 0.0f);
    bool paused = IsGamePaused();

    // transforma base
    TransformRealWorldPointToRadarSpace(base, base);
    TransformRadarPointToScreenSpace(base, base);
    if (paused) base *= flMenuMapScaling;

    // transforma offset
    TransformRealWorldPointToRadarSpace(offset, offset);
    TransformRadarPointToScreenSpace(offset, offset);
    if (paused) offset *= flMenuMapScaling;

    // distância em pixels
    return sqrt((offset.x - base.x)*(offset.x - base.x) + (offset.y - base.y)*(offset.y - base.y));
}

// Desenha um sprite na tela baseado em posição do mundo e tamanho fixo em pixels
static void DrawSpriteInRadar(ISprite* sprite, CVector worldPos, CRGBA color, float worldSize)
{
    bool paused = IsGamePaused();

    // transforma a posição do mundo para radar
    CVector2D screenPos(worldPos.x, worldPos.y);
    TransformRealWorldPointToRadarSpace(screenPos, screenPos);

    // garante que fique dentro do radar
    LimitRadarPoint(screenPos);

    // transforma para coordenadas de tela
    TransformRadarPointToScreenSpace(screenPos, screenPos);
    if (paused) screenPos *= flMenuMapScaling;

    // calcula tamanho em pixels baseado em unidades do mundo
    float pixelSize = WorldUnitsToPixels(worldSize);

    // desenha o sprite
    CRect rect(
        screenPos.x - pixelSize * 0.5f,
        screenPos.y - pixelSize * 0.5f,
        screenPos.x + pixelSize * 0.5f,
        screenPos.y + pixelSize * 0.5f
    );

    CSprite2d_DrawSprite(
        (CSprite2d*)sprite->GetCSprite2d(),
        rect,
        color
    );
}