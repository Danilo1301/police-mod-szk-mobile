#include "SpriteUtils.h"

#include "menuSZK/IMenuSZK.h"
extern IMenuSZK* menuSZK;

#include "CleoFunctions.h"

ISprite* SpriteUtils::spriteBigCircle = nullptr;
ISprite* SpriteUtils::spriteCircle = nullptr;
ISprite* SpriteUtils::spriteBlip = nullptr;
ISprite* SpriteUtils::spritePoliceDepartment = nullptr;

void SpriteUtils::OnRender()
{
    
}

void SpriteUtils::DrawSpriteInWorld(ISprite* sprite, CVector worldPos, float baseSize, CRGBA color)
{
    if(IsGamePaused())
        return;

    auto playerPosition = GetPlayerPosition();
    
    // Distância até o blip
    float distance = distanceBetweenPoints(playerPosition, worldPos);

    // Se estiver longe demais, não desenha
    const float maxDistance = 50.0f;
    if (distance > maxDistance)
        return;

    // Escala diminui com a distância (0 perto, 40 longe)
    float t = distance / maxDistance;          // 0 → perto, 1 → longe
    float scale = 1.0f - t;                    // 1.0 → perto, 0 → longe
    scale = std::clamp(scale, 0.2f, 1.0f);     // limita o tamanho mínimo

    float size = baseSize * scale;

    auto screenPos = menuSZK->ConvertWorldPositionToScreenPosition(worldPos);
    screenPos.x -= size / 2;
    screenPos.y -= size / 2;

    menuSZK->DrawSprite(sprite, screenPos, CVector2D(size, size), color);
}

void SpriteUtils::DrawSpriteInRadarWorld(ISprite* sprite, CVector worldPos, CRGBA color, float worldSize)
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

void SpriteUtils::DrawSpriteInRadar(ISprite* sprite, CVector worldPos, CRGBA color, float size)
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
    float pixelSize = size;

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

void SpriteUtils::DrawBlip(CVector worldPos, CRGBA color)
{
    DrawSpriteInWorld(spriteBlip, worldPos, 100.0f, color);
}

// Converte um tamanho em unidades do mundo para pixels no radar / mapa
float SpriteUtils::WorldUnitsToPixels(float worldUnits)
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