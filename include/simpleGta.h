#pragma once

#include <stdint.h>

class CRect {
public:
    float left = 0;          // x1
    float top = 0;        // y1
    float right = 0;         // x2
    float bottom = 0;           // y2

    CRect(float left, float top, float right, float bottom)
    {
        this->left = left;
        this->top = top;
        this->right = right;
        this->bottom = bottom;
    }

    static CRect fromWidthAndHeight(float x, float y, float width, float height)
    {
        CRect rect = CRect(x, y, x + width, y + height);
        return rect;
    }
};

class CRGBA {
public:
    unsigned char r = 0;
    unsigned char g = 0;
    unsigned char b = 0;
    unsigned char a = 0;

    CRGBA(unsigned char r, unsigned char g, unsigned char b)
    {
        this->r = r;
        this->g = g;
        this->b = b;
        this->a = 255;
    }

    CRGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
    {
        this->r = r;
        this->g = g;
        this->b = b;
        this->a = a;
    }
};

class CVector {
public:
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    CVector(float x, float y, float z)
    {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    inline void operator=(const CVector& right){
        x = right.x;
        y = right.y;
        z = right.z;
    }

    inline void operator+=(const CVector& right) {
        x = right.x + x;
        y = right.y + y;
        z = right.z + z;
    }

    inline void operator-=(const CVector& right) {
        x = x - right.x;
        y = y - right.y;
        z = z - right.z;
    }

    inline void operator*=(float multiplier) {
        x = multiplier * x;
        y = multiplier * y;
        z = multiplier * z;
    }

    inline void operator/=(float divisor) {
        float a = 1.0 / divisor;
        x = a * x;
        y = a * y;
        z = a * z;
    }

    CVector operator+(const CVector right)
    {
        CVector result(this->x, this->y, this->z);
        result.x += right.x;
        result.y += right.y;
        result.z += right.z;
        return result;
    }
};

class CVector2D {
public:
    float x = 0.0f;
    float y = 0.0f;

    CVector2D(float x, float y)
    {
        this->x = x;
        this->y = y;
    }

    CVector2D operator+(const CVector2D vec)
    {
        CVector2D result(this->x, this->y);
        result.x += vec.x;
        result.y += vec.y;
        return result;
    }

    CVector2D operator-(const CVector2D vec)
    {
        CVector2D result(this->x, this->y);
        result.x -= vec.x;
        result.y -= vec.y;
        return result;
    }
};

class CSprite2d {
public:
	// class variables
	struct RwTexture *m_pTexture;
	// static variables
	//static unsigned int& nextBufferIndex;
	//static unsigned int& nextBufferVertex;
	//static float& NearScreenZ;
	//static float& RecipNearClip;
	// count: 8
	//static struct RwD3D9Vertex *maVertices;
	// class functions
	//CSprite2d();
	//~CSprite2d();
};

enum eFontAlignment : unsigned char {
    ALIGN_CENTER,
    ALIGN_LEFT,
    ALIGN_RIGHT
};

enum eFontStyle : unsigned char {
    FONT_GOTHIC,
    FONT_SUBTITLES,
    FONT_MENU,
    FONT_PRICEDOWN
};