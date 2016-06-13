///////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) Microsoft Corporation.  All Rights Reserved.
//
//  File:       d3dx9core.h
//  Content:    D3DX core types and functions
//
///////////////////////////////////////////////////////////////////////////

#include "d3dx9.h"

#ifndef __D3DX9CORE_H__
#define __D3DX9CORE_H__


///////////////////////////////////////////////////////////////////////////
// D3DX_SDK_VERSION:
// -----------------
// This identifier is passed to D3DXCheckVersion in order to ensure that an
// application was built against the correct header files and lib files. 
// This number is incremented whenever a header (or other) change would 
// require applications to be rebuilt. If the version doesn't match, 
// D3DXCreateVersion will return FALSE. (The number itself has no meaning.)
///////////////////////////////////////////////////////////////////////////

#define D3DX_VERSION 0x0901
#define D3DX_SDK_VERSION 21

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

BOOL WINAPI
    D3DXCheckVersion(UINT D3DSdkVersion, UINT D3DXSdkVersion);

#ifdef __cplusplus
}
#endif //__cplusplus



///////////////////////////////////////////////////////////////////////////
// D3DXGetDriverLevel:
//    Returns driver version information:
//
//    700 - DX7 level driver
//    800 - DX8 level driver
//    900 - DX9 level driver
///////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

UINT WINAPI
    D3DXGetDriverLevel(LPDIRECT3DDEVICE9 pDevice);

#ifdef __cplusplus
}
#endif //__cplusplus


///////////////////////////////////////////////////////////////////////////
// ID3DXBuffer:
// ------------
// The buffer object is used by D3DX to return arbitrary size data.
//
// GetBufferPointer -
//    Returns a pointer to the beginning of the buffer.
//
// GetBufferSize -
//    Returns the size of the buffer, in bytes.
///////////////////////////////////////////////////////////////////////////

typedef interface ID3DXBuffer ID3DXBuffer;
typedef interface ID3DXBuffer *LPD3DXBUFFER;

// {932E6A7E-C68E-45dd-A7BF-53D19C86DB1F}
DEFINE_GUID(IID_ID3DXBuffer, 
0x932e6a7e, 0xc68e, 0x45dd, 0xa7, 0xbf, 0x53, 0xd1, 0x9c, 0x86, 0xdb, 0x1f);

#undef INTERFACE
#define INTERFACE ID3DXBuffer

DECLARE_INTERFACE_(ID3DXBuffer, IUnknown)
{
    // IUnknown
    STDMETHOD(QueryInterface)(THIS_ REFIID iid, LPVOID *ppv) PURE;
    STDMETHOD_(ULONG, AddRef)(THIS) PURE;
    STDMETHOD_(ULONG, Release)(THIS) PURE;

    // ID3DXBuffer
    STDMETHOD_(LPVOID, GetBufferPointer)(THIS) PURE;
    STDMETHOD_(DWORD, GetBufferSize)(THIS) PURE;
};



//////////////////////////////////////////////////////////////////////////////
// D3DXSPRITE flags:
// -----------------
// D3DXSPRITE_DONOTSAVESTATE
//   Specifies device state is not to be saved and restored in Begin/End.
// D3DXSPRITE_DONOTMODIFY_RENDERSTATE
//   Specifies device render state is not to be changed in Begin.  The device
//   is assumed to be in a valid state to draw vertices containing POSITION0, 
//   TEXCOORD0, and COLOR0 data.
// D3DXSPRITE_OBJECTSPACE
//   The WORLD, VIEW, and PROJECTION transforms are NOT modified.  The 
//   transforms currently set to the device are used to transform the sprites 
//   when the batch is drawn (at Flush or End).  If this is not specified, 
//   WORLD, VIEW, and PROJECTION transforms are modified so that sprites are 
//   drawn in screenspace coordinates.
// D3DXSPRITE_BILLBOARD
//   Rotates each sprite about its center so that it is facing the viewer.
// D3DXSPRITE_ALPHABLEND
//   Enables ALPHABLEND(SRCALPHA, INVSRCALPHA) and ALPHATEST(alpha > 0).
//   ID3DXFont expects this to be set when drawing text.
// D3DXSPRITE_SORT_TEXTURE
//   Sprites are sorted by texture prior to drawing.  This is recommended when
//   drawing non-overlapping sprites of uniform depth.  For example, drawing
//   screen-aligned text with ID3DXFont.
// D3DXSPRITE_SORT_DEPTH_FRONTTOBACK
//   Sprites are sorted by depth front-to-back prior to drawing.  This is 
//   recommended when drawing opaque sprites of varying depths.
// D3DXSPRITE_SORT_DEPTH_BACKTOFRONT
//   Sprites are sorted by depth back-to-front prior to drawing.  This is 
//   recommended when drawing transparent sprites of varying depths.
//////////////////////////////////////////////////////////////////////////////

#define D3DXSPRITE_DONOTSAVESTATE               (1 << 0)
#define D3DXSPRITE_DONOTMODIFY_RENDERSTATE      (1 << 1)
#define D3DXSPRITE_OBJECTSPACE                  (1 << 2)
#define D3DXSPRITE_BILLBOARD                    (1 << 3)
#define D3DXSPRITE_ALPHABLEND                   (1 << 4)
#define D3DXSPRITE_SORT_TEXTURE                 (1 << 5)
#define D3DXSPRITE_SORT_DEPTH_FRONTTOBACK       (1 << 6)
#define D3DXSPRITE_SORT_DEPTH_BACKTOFRONT       (1 << 7)

#define D3DXSPRITE_TEXT (


//////////////////////////////////////////////////////////////////////////////
// ID3DXSprite:
// ------------
// This object intends to provide an easy way to drawing sprites using D3D.
//
// Begin - 
//    Prepares device for drawing sprites.
//
// Draw -
//    Draws a sprite.  Before transformation, the sprite is the size of 
//    SrcRect, with its top-left corner specified by Position.  The color 
//    and alpha channels are modulated by Color.
//
// Flush -
//    Forces all batched sprites to submitted to the device.
//
// End - 
//    Restores device state to how it was when Begin was called.
//
// OnLostDevice, OnResetDevice -
//    Call OnLostDevice() on this object before calling Reset() on the
//    device, so that this object can release any stateblocks and video
//    memory resources.  After Reset(), the call OnResetDevice().
//////////////////////////////////////////////////////////////////////////////

typedef interface ID3DXSprite ID3DXSprite;
typedef interface ID3DXSprite *LPD3DXSPRITE;


// {D4715B38-6C44-472a-9024-6E2B0321CAC6}
DEFINE_GUID( IID_ID3DXSprite, 
0xd4715b38, 0x6c44, 0x472a, 0x90, 0x24, 0x6e, 0x2b, 0x3, 0x21, 0xca, 0xc6);


#undef INTERFACE
#define INTERFACE ID3DXSprite

DECLARE_INTERFACE_(ID3DXSprite, IUnknown)
{
    // IUnknown
    STDMETHOD(QueryInterface)(THIS_ REFIID iid, LPVOID *ppv) PURE;
    STDMETHOD_(ULONG, AddRef)(THIS) PURE;
    STDMETHOD_(ULONG, Release)(THIS) PURE;

    // ID3DXSprite
    STDMETHOD(GetDevice)(THIS_ LPDIRECT3DDEVICE9* ppDevice) PURE;

    STDMETHOD(GetTransform)(THIS_ D3DXMATRIX *pTransform) PURE;
    STDMETHOD(SetTransform)(THIS_ CONST D3DXMATRIX *pTransform) PURE;

    STDMETHOD(SetWorldViewRH)(THIS_ CONST D3DXMATRIX *pWorld, CONST D3DXMATRIX *pView) PURE;
    STDMETHOD(SetWorldViewLH)(THIS_ CONST D3DXMATRIX *pWorld, CONST D3DXMATRIX *pView) PURE;

    STDMETHOD(Begin)(THIS_ DWORD Flags) PURE;
    STDMETHOD(Draw)(THIS_ LPDIRECT3DTEXTURE9 pTexture, CONST RECT *pSrcRect, CONST D3DXVECTOR3 *pCenter, CONST D3DXVECTOR3 *pPosition, D3DCOLOR Color) PURE;
    STDMETHOD(Flush)(THIS) PURE;
    STDMETHOD(End)(THIS) PURE;

    STDMETHOD(OnLostDevice)(THIS) PURE;
    STDMETHOD(OnResetDevice)(THIS) PURE;
};


#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

HRESULT WINAPI 
    D3DXCreateSprite( 
        LPDIRECT3DDEVICE9   pDevice, 
        LPD3DXSPRITE*       ppSprite);

#ifdef __cplusplus
}
#endif //__cplusplus



//////////////////////////////////////////////////////////////////////////////
// ID3DXFont:
// ----------
// Font objects contain the textures and resources needed to render a specific 
// font on a specific device.
//
// GetGlyphData -
//    Returns glyph cache data, for a given glyph.
//
// PreloadCharacters/PreloadGlyphs/PreloadText -
//    Preloads glyphs into the glyph cache textures.
//
// DrawText -
//    Draws formatted text on a D3D device.  Some parameters are 
//    surprisingly similar to those of GDI's DrawText function.  See GDI 
//    documentation for a detailed description of these parameters.
//    If pSprite is NULL, an internal sprite object will be used.
//
// OnLostDevice, OnResetDevice -
//    Call OnLostDevice() on this object before calling Reset() on the
//    device, so that this object can release any stateblocks and video
//    memory resources.  After Reset(), the call OnResetDevice().
//////////////////////////////////////////////////////////////////////////////

typedef struct _D3DXFONT_DESCA
{
    UINT Height;
    UINT Width;
    UINT Weight;
    UINT MipLevels;
    BOOL Italic;
    BYTE CharSet;
    BYTE OutputPrecision;
    BYTE Quality;
    BYTE PitchAndFamily;
    CHAR FaceName[LF_FACESIZE];

} D3DXFONT_DESCA, *LPD3DXFONT_DESCA;

typedef struct _D3DXFONT_DESCW
{
    UINT Height;
    UINT Width;
    UINT Weight;
    UINT MipLevels;
    BOOL Italic;
    BYTE CharSet;
    BYTE OutputPrecision;
    BYTE Quality;
    BYTE PitchAndFamily;
    WCHAR FaceName[LF_FACESIZE];

} D3DXFONT_DESCW, *LPD3DXFONT_DESCW;

#ifdef UNICODE
typedef D3DXFONT_DESCW D3DXFONT_DESC;
typedef LPD3DXFONT_DESCW LPD3DXFONT_DESC;
#else
typedef D3DXFONT_DESCA D3DXFONT_DESC;
typedef LPD3DXFONT_DESCA LPD3DXFONT_DESC;
#endif


typedef interface ID3DXFont ID3DXFont;
typedef interface ID3DXFont *LPD3DXFONT;


// {0B8D1536-9EEC-49b0-A5AD-93CF63AFB7C6}
DEFINE_GUID( IID_ID3DXFont, 
0xb8d1536, 0x9eec, 0x49b0, 0xa5, 0xad, 0x93, 0xcf, 0x63, 0xaf, 0xb7, 0xc6);


#undef INTERFACE
#define INTERFACE ID3DXFont

DECLARE_INTERFACE_(ID3DXFont, IUnknown)
{
    // IUnknown
    STDMETHOD(QueryInterface)(THIS_ REFIID iid, LPVOID *ppv) PURE;
    STDMETHOD_(ULONG, AddRef)(THIS) PURE;
    STDMETHOD_(ULONG, Release)(THIS) PURE;

    // ID3DXFont
    STDMETHOD(GetDevice)(THIS_ LPDIRECT3DDEVICE9 *ppDevice) PURE;
    STDMETHOD(GetDescA)(THIS_ D3DXFONT_DESCA *pDesc) PURE;
    STDMETHOD(GetDescW)(THIS_ D3DXFONT_DESCW *pDesc) PURE;

    STDMETHOD_(HDC, GetDC)(THIS) PURE;
    STDMETHOD(GetGlyphData)(THIS_ UINT Glyph, LPDIRECT3DTEXTURE9 *ppTexture, RECT *pBlackBox, POINT *pCellInc) PURE;

    STDMETHOD(PreloadCharacters)(THIS_ UINT First, UINT Last) PURE;
    STDMETHOD(PreloadGlyphs)(THIS_ UINT First, UINT Last) PURE;
    STDMETHOD(PreloadTextA)(THIS_ LPCSTR pString, INT Count) PURE;
    STDMETHOD(PreloadTextW)(THIS_ LPCWSTR pString, INT Count) PURE;

    STDMETHOD_(INT, DrawTextA)(THIS_ LPD3DXSPRITE pSprite, LPCSTR pString, INT Count, LPRECT pRect, DWORD Format, D3DCOLOR Color) PURE;
    STDMETHOD_(INT, DrawTextW)(THIS_ LPD3DXSPRITE pSprite, LPCWSTR pString, INT Count, LPRECT pRect, DWORD Format, D3DCOLOR Color) PURE;

    STDMETHOD(OnLostDevice)(THIS) PURE;
    STDMETHOD(OnResetDevice)(THIS) PURE;

#ifdef __cplusplus
#ifdef UNICODE
    HRESULT GetDesc(D3DXFONT_DESCW *pDesc) { return GetDescW(pDesc); }
    HRESULT PreloadText(LPCWSTR pString, INT Count) { return PreloadTextW(pString, Count); }
#else
    HRESULT GetDesc(D3DXFONT_DESCA *pDesc) { return GetDescA(pDesc); }
    HRESULT PreloadText(LPCSTR pString, INT Count) { return PreloadTextA(pString, Count); }
#endif
#endif //__cplusplus
};

#ifndef DrawText
#ifdef UNICODE
#define DrawText DrawTextW
#else
#define DrawText DrawTextA
#endif
#endif


#ifdef __cplusplus
extern "C" {
#endif //__cplusplus


HRESULT WINAPI 
    D3DXCreateFontA(
        LPDIRECT3DDEVICE9       pDevice,  
        UINT                    Height,
        UINT                    Width,
        UINT                    Weight,
        UINT                    MipLevels,
        BOOL                    Italic,
        DWORD                   CharSet,
        DWORD                   OutputPrecision,
        DWORD                   Quality,
        DWORD                   PitchAndFamily,
        LPCSTR                  pFaceName,
        LPD3DXFONT*             ppFont);

HRESULT WINAPI 
    D3DXCreateFontW(
        LPDIRECT3DDEVICE9       pDevice,  
        UINT                    Height,
        UINT                    Width,
        UINT                    Weight,
        UINT                    MipLevels,
        BOOL                    Italic,
        DWORD                   CharSet,
        DWORD                   OutputPrecision,
        DWORD                   Quality,
        DWORD                   PitchAndFamily,
        LPCWSTR                 pFaceName,
        LPD3DXFONT*             ppFont);

#ifdef UNICODE
#define D3DXCreateFont D3DXCreateFontW
#else
#define D3DXCreateFont D3DXCreateFontA
#endif


HRESULT WINAPI 
    D3DXCreateFontIndirectA( 
        LPDIRECT3DDEVICE9       pDevice, 
        CONST D3DXFONT_DESCA*   pDesc, 
        LPD3DXFONT*             ppFont);

HRESULT WINAPI 
    D3DXCreateFontIndirectW( 
        LPDIRECT3DDEVICE9       pDevice, 
        CONST D3DXFONT_DESCW*   pDesc, 
        LPD3DXFONT*             ppFont);

#ifdef UNICODE
#define D3DXCreateFontIndirect D3DXCreateFontIndirectW
#else
#define D3DXCreateFontIndirect D3DXCreateFontIndirectA
#endif


#ifdef __cplusplus
}
#endif //__cplusplus



///////////////////////////////////////////////////////////////////////////
// ID3DXRenderToSurface:
// ---------------------
// This object abstracts rendering to surfaces.  These surfaces do not 
// necessarily need to be render targets.  If they are not, a compatible
// render target is used, and the result copied into surface at end scene.
//
// BeginScene, EndScene -
//    Call BeginScene() and EndScene() at the beginning and ending of your
//    scene.  These calls will setup and restore render targets, viewports, 
//    etc.. 
//
// OnLostDevice, OnResetDevice -
//    Call OnLostDevice() on this object before calling Reset() on the
//    device, so that this object can release any stateblocks and video
//    memory resources.  After Reset(), the call OnResetDevice().
///////////////////////////////////////////////////////////////////////////

typedef struct _D3DXRTS_DESC
{
    UINT                Width;
    UINT                Height;
    D3DFORMAT           Format;
    BOOL                DepthStencil;
    D3DFORMAT           DepthStencilFormat;

} D3DXRTS_DESC, *LPD3DXRTS_DESC;


typedef interface ID3DXRenderToSurface ID3DXRenderToSurface;
typedef interface ID3DXRenderToSurface *LPD3DXRENDERTOSURFACE;


// {0D014791-8863-4c2c-A1C0-02F3E0C0B653}
DEFINE_GUID( IID_ID3DXRenderToSurface, 
0xd014791, 0x8863, 0x4c2c, 0xa1, 0xc0, 0x2, 0xf3, 0xe0, 0xc0, 0xb6, 0x53);


#undef INTERFACE
#define INTERFACE ID3DXRenderToSurface

DECLARE_INTERFACE_(ID3DXRenderToSurface, IUnknown)
{
    // IUnknown
    STDMETHOD(QueryInterface)(THIS_ REFIID iid, LPVOID *ppv) PURE;
    STDMETHOD_(ULONG, AddRef)(THIS) PURE;
    STDMETHOD_(ULONG, Release)(THIS) PURE;

    // ID3DXRenderToSurface
    STDMETHOD(GetDevice)(THIS_ LPDIRECT3DDEVICE9* ppDevice) PURE;
    STDMETHOD(GetDesc)(THIS_ D3DXRTS_DESC* pDesc) PURE;

    STDMETHOD(BeginScene)(THIS_ LPDIRECT3DSURFACE9 pSurface, CONST D3DVIEWPORT9* pViewport) PURE;
    STDMETHOD(EndScene)(THIS_ DWORD MipFilter) PURE;

    STDMETHOD(OnLostDevice)(THIS) PURE;
    STDMETHOD(OnResetDevice)(THIS) PURE;
};


#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

HRESULT WINAPI
    D3DXCreateRenderToSurface(
        LPDIRECT3DDEVICE9       pDevice,
        UINT                    Width,
        UINT                    Height,
        D3DFORMAT               Format,
        BOOL                    DepthStencil,
        D3DFORMAT               DepthStencilFormat,
        LPD3DXRENDERTOSURFACE*  ppRenderToSurface);

#ifdef __cplusplus
}
#endif //__cplusplus



///////////////////////////////////////////////////////////////////////////
// ID3DXRenderToEnvMap:
// --------------------
// This object abstracts rendering to environment maps.  These surfaces 
// do not necessarily need to be render targets.  If they are not, a 
// compatible render target is used, and the result copied into the
// environment map at end scene.
//
// BeginCube, BeginSphere, BeginHemisphere, BeginParabolic -
//    This function initiates the rendering of the environment map.  As
//    parameters, you pass the textures in which will get filled in with
//    the resulting environment map.
//
// Face -
//    Call this function to initiate the drawing of each face.  For each 
//    environment map, you will call this six times.. once for each face 
//    in D3DCUBEMAP_FACES.
//
// End -
//    This will restore all render targets, and if needed compose all the
//    rendered faces into the environment map surfaces.
//
// OnLostDevice, OnResetDevice -
//    Call OnLostDevice() on this object before calling Reset() on the
//    device, so that this object can release any stateblocks and video
//    memory resources.  After Reset(), the call OnResetDevice().
///////////////////////////////////////////////////////////////////////////

typedef struct _D3DXRTE_DESC
{
    UINT        Size;
    UINT        MipLevels;
    D3DFORMAT   Format;
    BOOL        DepthStencil;
    D3DFORMAT   DepthStencilFormat;

} D3DXRTE_DESC, *LPD3DXRTE_DESC;


typedef interface ID3DXRenderToEnvMap ID3DXRenderToEnvMap;
typedef interface ID3DXRenderToEnvMap *LPD3DXRenderToEnvMap;


// {1561135E-BC78-495b-8586-94EA537BD557}
DEFINE_GUID( IID_ID3DXRenderToEnvMap, 
0x1561135e, 0xbc78, 0x495b, 0x85, 0x86, 0x94, 0xea, 0x53, 0x7b, 0xd5, 0x57);


#undef INTERFACE
#define INTERFACE ID3DXRenderToEnvMap

DECLARE_INTERFACE_(ID3DXRenderToEnvMap, IUnknown)
{
    // IUnknown
    STDMETHOD(QueryInterface)(THIS_ REFIID iid, LPVOID *ppv) PURE;
    STDMETHOD_(ULONG, AddRef)(THIS) PURE;
    STDMETHOD_(ULONG, Release)(THIS) PURE;

    // ID3DXRenderToEnvMap
    STDMETHOD(GetDevice)(THIS_ LPDIRECT3DDEVICE9* ppDevice) PURE;
    STDMETHOD(GetDesc)(THIS_ D3DXRTE_DESC* pDesc) PURE;

    STDMETHOD(BeginCube)(THIS_ 
        LPDIRECT3DCUBETEXTURE9 pCubeTex) PURE;

    STDMETHOD(BeginSphere)(THIS_
        LPDIRECT3DTEXTURE9 pTex) PURE;

    STDMETHOD(BeginHemisphere)(THIS_ 
        LPDIRECT3DTEXTURE9 pTexZPos,
        LPDIRECT3DTEXTURE9 pTexZNeg) PURE;

    STDMETHOD(BeginParabolic)(THIS_ 
        LPDIRECT3DTEXTURE9 pTexZPos,
        LPDIRECT3DTEXTURE9 pTexZNeg) PURE;

    STDMETHOD(Face)(THIS_ D3DCUBEMAP_FACES Face, DWORD MipFilter) PURE;
    STDMETHOD(End)(THIS_ DWORD MipFilter) PURE;

    STDMETHOD(OnLostDevice)(THIS) PURE;
    STDMETHOD(OnResetDevice)(THIS) PURE;
};


#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

HRESULT WINAPI
    D3DXCreateRenderToEnvMap(
        LPDIRECT3DDEVICE9       pDevice,
        UINT                    Size,
        UINT                    MipLevels,
        D3DFORMAT               Format,
        BOOL                    DepthStencil,
        D3DFORMAT               DepthStencilFormat,
        LPD3DXRenderToEnvMap*   ppRenderToEnvMap);

#ifdef __cplusplus
}
#endif //__cplusplus



///////////////////////////////////////////////////////////////////////////
// ID3DXLine:
// ------------
// This object intends to provide an easy way to draw lines using D3D.
//
// Begin - 
//    Prepares device for drawing lines
//
// Draw -
//    Draws a line strip in screen-space.
//    Input is in the form of a array defining points on the line strip. of D3DXVECTOR2 
//
// DrawTransform -
//    Draws a line in screen-space with a specified input transformation matrix.
//
// End - 
//     Restores device state to how it was when Begin was called.
//
// SetPattern - 
//     Applies a stipple pattern to the line.  Input is one 32-bit
//     DWORD which describes the stipple pattern. 1 is opaque, 0 is
//     transparent.
//
// SetPatternScale - 
//     Stretches the stipple pattern in the u direction.  Input is one
//     floating-point value.  0.0f is no scaling, whereas 1.0f doubles
//     the length of the stipple pattern.
//
// SetWidth - 
//     Specifies the thickness of the line in the v direction.  Input is
//     one floating-point value.
//
// SetAntialias - 
//     Toggles line antialiasing.  Input is a BOOL.
//     TRUE  = Antialiasing on.
//     FALSE = Antialiasing off.
//
// SetGLLines - 
//     Toggles non-antialiased OpenGL line emulation.  Input is a BOOL.
//     TRUE  = OpenGL line emulation on.
//     FALSE = OpenGL line emulation off.
//
// OpenGL line:     Regular line:  
//   *\                *\
//   | \              /  \
//   |  \            *\   \
//   *\  \             \   \
//     \  \             \   \
//      \  *             \   *
//       \ |              \ /
//        \|               *
//         *
//
// OnLostDevice, OnResetDevice -
//    Call OnLostDevice() on this object before calling Reset() on the
//    device, so that this object can release any stateblocks and video
//    memory resources.  After Reset(), the call OnResetDevice().
///////////////////////////////////////////////////////////////////////////


typedef interface ID3DXLine ID3DXLine;
typedef interface ID3DXLine *LPD3DXLINE;


// {72CE4D70-CC40-4143-A896-32E50AD2EF35}
DEFINE_GUID( IID_ID3DXLine, 
0x72ce4d70, 0xcc40, 0x4143, 0xa8, 0x96, 0x32, 0xe5, 0xa, 0xd2, 0xef, 0x35);

#undef INTERFACE
#define INTERFACE ID3DXLine

DECLARE_INTERFACE_(ID3DXLine, IUnknown)
{
    // IUnknown
    STDMETHOD(QueryInterface)(THIS_ REFIID iid, LPVOID *ppv) PURE;
    STDMETHOD_(ULONG, AddRef)(THIS) PURE;
    STDMETHOD_(ULONG, Release)(THIS) PURE;

    // ID3DXLine
    STDMETHOD(GetDevice)(THIS_ LPDIRECT3DDEVICE9* ppDevice) PURE;

    STDMETHOD(Begin)(THIS) PURE;

    STDMETHOD(Draw)(THIS_ CONST D3DXVECTOR2 *pVertexList,
        DWORD dwVertexListCount, D3DCOLOR Color) PURE;

    STDMETHOD(DrawTransform)(THIS_ CONST D3DXVECTOR3 *pVertexList,
        DWORD dwVertexListCount, CONST D3DXMATRIX* pTransform, 
        D3DCOLOR Color) PURE;

    STDMETHOD(SetPattern)(THIS_ DWORD dwPattern) PURE;
    STDMETHOD_(DWORD, GetPattern)(THIS) PURE;

    STDMETHOD(SetPatternScale)(THIS_ FLOAT fPatternScale) PURE;
    STDMETHOD_(FLOAT, GetPatternScale)(THIS) PURE;

    STDMETHOD(SetWidth)(THIS_ FLOAT fWidth) PURE;
    STDMETHOD_(FLOAT, GetWidth)(THIS) PURE;

    STDMETHOD(SetAntialias)(THIS_ BOOL bAntialias) PURE;
    STDMETHOD_(BOOL, GetAntialias)(THIS) PURE;

    STDMETHOD(SetGLLines)(THIS_ BOOL bGLLines) PURE;
    STDMETHOD_(BOOL, GetGLLines)(THIS) PURE;

    STDMETHOD(End)(THIS) PURE;

    STDMETHOD(OnLostDevice)(THIS) PURE;
    STDMETHOD(OnResetDevice)(THIS) PURE;
};


#ifdef __cplusplus
extern "C" {
#endif //__cplusplus


HRESULT WINAPI
    D3DXCreateLine(
        LPDIRECT3DDEVICE9   pDevice,
        LPD3DXLINE*         ppLine);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //__D3DX9CORE_H__

