#pragma once

#include <stdint.h>

#include "rw/rpworld.h"
#include "rw/rwcore.h"
#include "rw/rwplcore.h"

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

    CRGBA() {}

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

    CVector() {}

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

    CVector2D() : x(0.0f), y(0.0f) {}
    CVector2D(float x, float y) : x(x), y(y) {}

    // soma
    CVector2D operator+(const CVector2D& vec) const {
        return CVector2D(x + vec.x, y + vec.y);
    }

    // subtração
    CVector2D operator-(const CVector2D& vec) const {
        return CVector2D(x - vec.x, y - vec.y);
    }

    // multiplicação por escalar
    CVector2D operator*(float scalar) const {
        return CVector2D(x * scalar, y * scalar);
    }

    // multiplicação-assignment por escalar
    CVector2D& operator*=(float scalar) {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    // adição-assignment
    CVector2D& operator+=(const CVector2D& vec) {
        x += vec.x;
        y += vec.y;
        return *this;
    }

    // subtração-assignment
    CVector2D& operator-=(const CVector2D& vec) {
        x -= vec.x;
        y -= vec.y;
        return *this;
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

class CMatrix
{
public:
    union
    {
        float f[4][4];
        struct
        {
            float rx, ry, rz, rw;
            float fx, fy, fz, fw;
            float ux, uy, uz, uw;
            float px, py, pz, pw;
        };
        struct
        {
            CVector      right;
            unsigned int flags;
            CVector      up;
            unsigned int pad1;
            CVector      at;
            unsigned int pad2;
            CVector      pos;
            unsigned int pad3;
        };
        struct // RwV3d style
        {
            CVector      m_right;
            unsigned int m_flags;
            CVector      m_forward;
            unsigned int m_pad1;
            CVector      m_up;
            unsigned int m_pad2;
            CVector      m_pos;
            unsigned int m_pad3;
        };
    };

    void* m_pAttachedMatrix;
    bool         m_bOwnsAttachedMatrix;
    char         matrixpad[3];
};


// Simple entities
class CSimpleTransform
{
public:
    CVector pos;
    float   heading;
};
class CPlaceable
{
protected:
    CPlaceable();
public:
    char padding[4];
    CSimpleTransform m_placement;
    CMatrix *m_matrix;

    CSimpleTransform* GetPlacement()
    {
        return (CSimpleTransform*)((uintptr_t)this + 4);
    }
    CMatrix* GetMatrix()
    {
        return *(CMatrix**)((uintptr_t)this + 20);
    }

    CVector* GetPosSA()
    {
        auto mat = *(CMatrix**)((uintptr_t)this + 20);
        if (mat)
        {
            return &mat->pos;
        }
        return &((CSimpleTransform*)((uintptr_t)this + 4))->pos;
    }
    CVector* GetPosVC()
    {
        return (CVector*)((uintptr_t)this + 52);
    }
    CMatrix* GetMatSA()
    {
        return *(CMatrix**)((uintptr_t)this + 20);
    }
    CMatrix* GetMatVC()
    {
        return (CMatrix*)((uintptr_t)this + 4);
    }
    CMatrix* GetCamMatVC()
    {
        return (CMatrix*)this;
    }
};

class CEntity : public CPlaceable {
protected:
    CEntity();  
public:
    union {
        struct RwObject *m_pRwObject;
        struct RpClump *m_pRwClump;
        struct RpAtomic *m_pRwAtomic;
    };
};

class CCamera : public CPlaceable {};
class CPed : public CEntity {};
class CVehicle : public CEntity {};
class CObject : public CPlaceable {};

typedef struct RsGlobalType RsGlobalType;
struct RsGlobalType
{
    const RwChar *appName;
    RwInt32 maximumWidth;
    RwInt32 maximumHeight;
    RwInt32 maxFPS;
    RwBool  quit;
};
