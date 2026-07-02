#pragma once

#include <cstdint>
#include <algorithm>
#include <type_traits>
#include <utility>

#if __has_include("hax.h")
#include "hax.h"
#elif __has_include("../hax.h")
#include "../hax.h"
#else
#error "hax.h not found"
#endif

#ifndef HAX_API
#define HAX_API
#endif

#define HAX_LINE static_cast<size_t>(__LINE__)

inline float operator "" _px(long double val);
inline float operator "" _px(unsigned long long val);

namespace Hax::Gui
{
    using TextureHandle = Handle;
    enum class FontHandle : Handle {};
    enum class MsdfFontHandle : Handle {};

    struct Context;

    extern Allocator g_StateAlloc;

    constexpr float kScalingBaseHeight = 1080.f; // Scale factor = screen_height / kScalingBaseHeight

}

//=============================================================================
// [SECTION] COLORS
//=============================================================================

namespace Hax
{
    namespace Gui
    {
        struct Color;

        struct LinearColor
        {
            constexpr                   LinearColor() = default;
            constexpr                   LinearColor(float r, float g, float b, float a = 1.f) : R(r), G(g), B(b), A(a) {}
            constexpr                   LinearColor(const Color& color);
            constexpr                   LinearColor(uint32 hex);

            constexpr LinearColor&      operator*=(float rhs) { R *= rhs; G *= rhs; B *= rhs; return *this; }

            constexpr Color             ToColor() const;

            constexpr Vector4            ToHSV() const;
            static constexpr LinearColor FromHSV(float h, float s, float v, float a = 1.f);

            LinearColor                 Brighten(float factor) const;
            LinearColor                 Darken(float factor) const;

            static const LinearColor    White;
            static const LinearColor    Gray;
            static const LinearColor    Black;
            static const LinearColor    Transparent;
            static const LinearColor    Red;
            static const LinearColor    Green;
            static const LinearColor    Blue;
            static const LinearColor    Yellow;

            union
            {
                struct                  { float R, G, B, A; };
                float                   RGBA[4]{1.f, 1.f, 1.f, 1.f};
            };
        };

        inline constexpr LinearColor LinearColor::White(1.f,1.f,1.f);
        inline constexpr LinearColor LinearColor::Gray(0.5f,0.5f,0.5f);
        inline constexpr LinearColor LinearColor::Black(0,0,0);
        inline constexpr LinearColor LinearColor::Transparent(0,0,0,0);
        inline constexpr LinearColor LinearColor::Red(1.f,0,0);
        inline constexpr LinearColor LinearColor::Green(0,1.f,0);
        inline constexpr LinearColor LinearColor::Blue(0,0,1.f);
        inline constexpr LinearColor LinearColor::Yellow(1.f,1.f,0);

        struct Color
        {
            constexpr                   Color() = default;
            constexpr                   Color(const Color& c) = default;
            constexpr                   Color(uint8 r, uint8 g, uint8 b, uint8 a = 255) : R(r), G(g), B(b), A(a) {}

            constexpr                   Color(uint32 col) : A(col & 0xFF),
                                                            B((col >> 8) & 0xFF),
                                                            G((col >> 16) & 0xFF),
                                                            R((col >> 24) & 0xFF) {}

            constexpr bool              IsTransparent() const { return A == 0; }

            static const Color          White;
            static const Color          Black;
            static const Color          Transparent;
            static const Color          Red;
            static const Color          Green;
            static const Color          Blue;
            static const Color          Yellow;
            static const Color          Cyan;
            static const Color          Magenta;
            static const Color          Orange;
            static const Color          Purple;
            static const Color          Turquoise;
            static const Color          Silver;
            static const Color          Emerald;

            union
            {
                struct                  { uint8 R, G, B, A; };
                uint32                  Bits = 0xFFFFFFFF;
            };
        };

        inline constexpr Color Color::White(255,255,255);
        inline constexpr Color Color::Black(0,0,0);
        inline constexpr Color Color::Transparent(0, 0, 0, 0);
        inline constexpr Color Color::Red(255,0,0);
        inline constexpr Color Color::Green(0,255,0);
        inline constexpr Color Color::Blue(0,0,255);
        inline constexpr Color Color::Yellow(255,255,0);
        inline constexpr Color Color::Cyan(0,255,255);
        inline constexpr Color Color::Magenta(255,0,255);
        inline constexpr Color Color::Orange(243, 156, 18);
        inline constexpr Color Color::Purple(169, 7, 228);
        inline constexpr Color Color::Turquoise(26, 188, 156);
        inline constexpr Color Color::Silver(189, 195, 199);
        inline constexpr Color Color::Emerald(46, 204, 113);
    }

    constexpr Gui::LinearColor Lerp(const Gui::LinearColor& a, const Gui::LinearColor& b, float t)
    {
        return Gui::LinearColor
        (
            Hax::Lerp(a.R, b.R, t),
            Hax::Lerp(a.G, b.G, t),
            Hax::Lerp(a.B, b.B, t),
            Hax::Lerp(a.A, b.A, t)
        );
    }

    constexpr Gui::Color Lerp(const Gui::Color& a, const Gui::Color& b, float t)
    {
        return Lerp(Gui::LinearColor(a), Gui::LinearColor(b), t).ToColor();
    }
}

namespace Hax::Gui
{
    enum MouseIcon
    {
        MouseIcon_Default = -2,
        MouseIcon_None = -1,
        MouseIcon_Arrow = 0,
        MouseIcon_TextInput,
        MouseIcon_ResizeAll,
        MouseIcon_ResizeNS,
        MouseIcon_ResizeEW,
        MouseIcon_ResizeNESW,
        MouseIcon_ResizeNWSE,
        MouseIcon_Hand,
        MouseIcon_Wait,
        MouseIcon_Progress,
        MouseIcon_NotAllowed,
        MouseIcon_COUNT
    };

    enum class FontWeight : uint32
    {
        Thin        = 100,
        ExtraLight  = 200,
        UltraLight  = 200,
        Light       = 300,
        SemiLight   = 350,
        Normal      = 400,
        Regular     = 400,
        Medium      = 500,
        SemiBold    = 600,
        DemiBold    = 600,
        Bold        = 700,
        ExtraBold   = 800,
        UltraBold   = 800,
        Black       = 900,
        Heavy       = 900,
        ExtraBlack  = 950,
        UltraBlack  = 950
    };

    enum class TextureFormat
    {
        R8_UNorm,
        R8G8B8A8_UNorm
    };

    struct LinearAnim
    {
        void            Elapse(float time, float speed) { if (speed == 0.f) return; Progress = Clamp(Progress + time / speed, 0.f, 1.f); }
        float           Progress;
    };

    struct HotData
    {
        float           ScaleFactor;

        size_t          HoveredItemId;
        size_t          ActiveItemId;
        size_t          FocusedItemId;
        size_t          ClickedItemId;

        double          Time;
        float           DeltaTime;
        float           Framerate;

        Vector2         MousePos;
        Vector2         MouseDeltaPos;

        Vector2         ViewportSize;
    };
    extern HotData G;

    struct  RectParams      
    { 
        Color           BorderColor     = Color::Transparent;
        float           BorderTh        = 0.f;
        Color           FillColor;
        Vector4         Rounding;
    };

    struct  EllipseParams   
    { 
        Color           BorderColor; 
        float           BorderTh; 
        Color           FillColor; 
    };

    struct  TriangleParams  
    { 
        Color           BorderColor; 
        float           BorderTh; 
        Color           FillColor; 
    };

    struct  LineParams      
    { 
        Color           FillColor       = Color::Black; 
        float           Th              = 1_px; 
    };

    struct  ImageParams     
    { 
        Color           BgColor         = Color::White; 
        float           R               = 0.f; 
        Vector2         UVmin           = {}; 
        Vector2         UVmax           = { 1.f, 1.f }; 
    };

    struct  MsdfGlyphParams 
    { 
        Color           Col             = Color::Black;
        bool            Outline         = false;
        Color           OutCol          = Color::Black; 
        float           Spacing         = 1.f; 
        float           Weight          = 0.f; 
    };

    struct  GlyphParams     
    { 
        Color           Color           = Color::Black; 
        float           Spacing         = 1.f; 
    };

    using CircleParams    = EllipseParams;
    using MsdfTextParams  = MsdfGlyphParams;
    using TextParams      = GlyphParams;

    struct StringEditParams 
    { 
        Color           CaretColor      = Color::Black; 
        WStringView     Hint            = {}; 
        Color           HintColor       = 0x969fafff; 
        Color           TextColor       = Color::Black; 
    };

    struct ScrollStyle
    {
        float           TrackWidth      = 20_px;
        float           ThumbPadding    = 5_px;
        Color           TrackCol        = 0x404040FF;
        Color           ThumbCol        = 0xECECECFF;
        Color           ThumbHovCol     = 0x696969FF;
        Color           ThumbActiveCol  = 0x828282FF;
    };

    struct ContainerParams
    {
        float           W = 0.f;
        float           H = 0.f;

        bool            Clip            : 1 = false;
        bool            ScrollX         : 1 = false;
        bool            ScrollY         : 1 = false;
        bool            ScrollVisible   : 1 = false;
        bool            FitX            : 1 = false;
        bool            FitY            : 1 = false;

        ScrollStyle     Style = {};
    };

    struct InterRes
    {
        bool            Active;
        bool            Clicked;
        bool            Focused;
        bool            Hovered;
        bool            Pressed;
    };

    struct FontMetrics 
    { 
        float           Ascent;
        float           CapHeight;
        float           Descent;
        float           LineHeight;
    };

    struct IStatePool
    {
        virtual         ~IStatePool() = default;
    };

    template <TriCo T>
    struct StatePool : IStatePool
    {
        Map<size_t, T>  m_Map{g_StateAlloc};
    };

    //=============================================================================
    // [SECTION] API
    //=============================================================================

    void                Shutdown();
    bool                Initialized();
    void                SetLogFile(LogFile* logFile);

    void                BeginFrame();
    void                EndFrame();

    inline double       GetTime() { return G.Time; }
    inline float        GetDeltaTime() { return G.DeltaTime; }
    inline float        GetFramerate() { return G.Framerate; }

    inline Vector2      GetMousePos() { return G.MousePos; }
    inline Vector2      GetMouseDeltaPos() { return G.MouseDeltaPos; }
    void                SetMouseIcon(MouseIcon icon);
    Handle              GetMouseTexture();
    bool                IsLmbReleased();
    bool                IsLmbJustReleased();
    bool                IsLmbPressed();
    bool                IsLmbJustPressed();
    void                SetMouseTexture(MouseIcon icon, Handle tex);
    void                SetMouseTextureFromRes(MouseIcon icon, Handle tex, int res);

    long                HandleWndMsg(void* hwnd, uint32 msg, uintptr wParam, uint64 lParam);

    void                CreateLayer(WStringView name, int zOrder);
    void                SwitchLayer(WStringView name);
    void                RestoreLayer();

    void                DrawRect(const Vector2& a, const Vector2& b, const RectParams& params = {});
    void                DrawEllipse(const Vector2& c, const Vector2& r, const EllipseParams& params = {});
    void                DrawCircle(const Vector2& c, float r, const CircleParams& params = {});
    void                DrawTriangle(const Vector2& a, const Vector2& b, const Vector2& c, const TriangleParams& params = {});
    void                DrawLine(Vector2 a, Vector2 b, const LineParams& params = {});
    void                DrawImage(TextureHandle hImage, const Vector2& a, const Vector2& b, const ImageParams& params = {});
    void                DrawString(MsdfFontHandle font, WStringView text, const Vector2& pos, float size, const MsdfTextParams& params = {});
    void                DrawString(FontHandle font, WStringView text, const Vector2& pos, float size, const TextParams& params = {});

    bool                StringEdit(size_t id, FontHandle font, wchar_t* buf, size_t bufSize, float textSize, const Rect& bounds, const StringEditParams& params = {});

    void                PushSkipDrawing();
    void                PopSkipDrawing(uint32 count = 1);
    bool                IsDrawingSkipped();

    void                BeginRotation(float angleRad);
    void                EndRotation();

    void                PushClipRect(const Rect& clipRect);
    void                PopClipRect();

    void                BeginVertical(float spacing = 0.f);
    void                EndVertical();
    void                BeginHorizontal(float spacing = 0.f);
    void                EndHorizontal();

    void                PlaceItem(const Vector2& size);
    void                Space(float pixels);
    inline void         Dummy(const Vector2& size) { PlaceItem(size); }

    Vector2             GetCursorPos();
    void                SetCursorPos(const Vector2& pos);
    void                VerticalLine(float th, const Color& color, float size = 0.f);
    void                HorizontalLine(float th, const Color& color, float size = 0.f);
    Rect                GetLayoutBounds();
    float               GetLayoutSpacing();
    void                ResetCursor();

    bool                IsItemVisible(const Rect& bounds);

    void                BeginContainer(size_t id = 0, const ContainerParams& params = {});
    void                EndContainer();
    Rect                GetContainerBounds();

    Vector2             GetContentRegionAvail();
    void                ScrollYTo(float posY);

    InterRes            Interact(size_t id, const Rect& bounds);
    inline bool         IsItemHovered(size_t id)    { return G.HoveredItemId == id; }
    inline bool         IsItemFocused(size_t id)    { return G.FocusedItemId == id; }
    inline bool         IsItemActive(size_t id)     { return G.ActiveItemId == id; }
    inline bool         IsItemClicked(size_t id)    { return G.ClickedItemId == id; }
    inline bool         IsItemPressed(size_t id)    { return G.ActiveItemId == id && G.HoveredItemId == id; }
    bool                IsItemPressedRepeat(size_t id, float delay = 0.5f, float interval = 0.033f);

    inline float        Scale(float val) { return val * G.ScaleFactor; }
    inline Vector2      Scale(const Vector2& val) { return Vector2(val.X * G.ScaleFactor, val.Y * G.ScaleFactor); }

    Span<const uint8>   GetResourceData(Handle resModule, int resId, const char16* resType);

    Vector<MsdfFontHandle> LoadMsdfFonts(Span<const Span<const uint8>> items);
    Vector2             CalcTextSize(FontHandle hFont, WStringView text, float size, float spacing = 1.0);
    Vector2             CalcTextSize(MsdfFontHandle hFont, WStringView text, float size, float spacing = 1.0);
    float               GetFontLineHeight(FontHandle hFont, float fontH);
    float               GetFontLineHeight(MsdfFontHandle hFont, float fontH);
    float               GetFontAscent(FontHandle hFont, float fontH);
    float               GetFontCapHeight(FontHandle hFont, float fontH);
    Vector2             GetGlyphSize(FontHandle hFont, char16 sym, float h);

    FontMetrics         GetFontMetrics(FontHandle hFont, float fontH);

    FontHandle          LoadFont(Span<const uint8>);
    FontHandle          LoadSystemFont(const char16* fontFamilyName, FontWeight weight = FontWeight::Regular, bool italic = false);
    void                UnloadFont(FontHandle hFont);
    void                BakeGlyphsInRange(FontHandle hFont, char16 from, char16 to, uint32 size);
    //TextureHandle       GetFontAtlas();

    TextureHandle       LoadImageFromMemory(Span<const uint8> data);
    Vector2             GetImageSize(TextureHandle hImage);
    void                UnloadImage(TextureHandle hImage);

    void                ShowDemoWindow();

    inline Vector2      GetViewportSize() { return G.ViewportSize; }
    inline Vector2      GetViewportCenter() { return G.ViewportSize / 2.f; }
    inline Rect         GetViewportBounds() { return Rect(Vector2(), G.ViewportSize); }

    bool                IsKeyDown(uint8 vk);
    bool                IsKeyJustDown(uint8 vk);
    bool                IsKeyDownRepeat(uint8 vk, float delay = 0.5f, float interval = 0.033f);
    bool                IsKeyUp(uint8 vk);
    bool                IsKeyJustUp(uint8 vk);
    WStringView          GetKeyName(uint8 vk);
    const Vector<uint8>& GetJustPressedKeys();

    IStatePool**        GetStatePool(size_t typeId);
    void                AddStatePool(size_t typeId, IStatePool* pool);
    template <typename T> inline T& GetState(size_t id);

    Allocator*          IterAllocators(void*& iter);

    //=============================================================================
    // [SECTION] IMPLEMENTATION
    //=============================================================================

    constexpr Vector4 LinearColor::ToHSV() const
    {
        float r = R;
        float g = G;
        float b = B;
        float a = A;

        float K = 0.f;
        if (g < b)
        {
            Swap(g, b);
            K = -1.f;
        }
        if (r < g)
        {
            Swap(r, g);
            K = -2.f / 6.f - K;
        }

        const float chroma = r - (g < b ? g : b);
        float h = Abs(K + (g - b) / (6.f * chroma + 1e-20f));
        float s = chroma / (r + 1e-20f);
        float v = r;

        return {h, s, v, a};
    }

    constexpr LinearColor LinearColor::FromHSV(float h, float s, float v, float a)
    {
        float r, g, b;

        if (s == 0.0f)
        {
            r = g = b = v;
            return LinearColor(r, g, b);
        }

        h = Mod(h, 1.0f) / (60.0f / 360.0f);
        int   i = (int)h;
        float f = h - (float)i;
        float p = v * (1.0f - s);
        float q = v * (1.0f - s * f);
        float t = v * (1.0f - s * (1.0f - f));

        switch (i)
        {
            case 0: r = v; g = t; b = p; break;
            case 1: r = q; g = v; b = p; break;
            case 2: r = p; g = v; b = t; break;
            case 3: r = p; g = q; b = v; break;
            case 4: r = t; g = p; b = v; break;
            case 5: 
            default: r = v; g = p; b = q; break;
        }

        return LinearColor(r, g, b);
    }

    constexpr float k1To255 = 1.f / 255.f;
    constexpr LinearColor::LinearColor(const Color& color) : R(static_cast<float>(color.R) * k1To255), 
                                                             G(static_cast<float>(color.G) * k1To255), 
                                                             B(static_cast<float>(color.B) * k1To255), 
                                                             A(static_cast<float>(color.A) * k1To255) {}

    constexpr LinearColor::LinearColor(uint32 hex) : LinearColor(Color(hex)) {}

    constexpr Color LinearColor::ToColor() const
    {
        return Color
        (
            (uint8)(0.5f + Clamp(R, 0.f, 1.f) * 255.f),
            (uint8)(0.5f + Clamp(G, 0.f, 1.f) * 255.f),
            (uint8)(0.5f + Clamp(B, 0.f, 1.f) * 255.f),
            (uint8)(0.5f + Clamp(A, 0.f, 1.f) * 255.f)
        );
    }

    template <typename T>
    inline T& GetState(size_t id) 
    {
        constexpr size_t typeId = GetTypeId<T>();

        StatePool<T>* pool = nullptr;

        if (IStatePool** ppPool = GetStatePool(typeId))
            pool = (StatePool<T>*)*ppPool;
        else
        {
            pool = New<StatePool<T>>(g_StateAlloc);
            AddStatePool(typeId, pool);
        }

        return pool->m_Map.FindOrAdd(id);
    }
}

inline float operator "" _px(long double val) { return (float)val * Hax::Gui::G.ScaleFactor; }
inline float operator "" _px(unsigned long long val) { return (float)val *  Hax::Gui::G.ScaleFactor; }

#ifdef HAXGUI_INCLUDE_INTERNAL
#include <atomic>

struct IDWriteFactory5;
struct IDWriteInMemoryFontFileLoader;
struct IDWriteBitmapRenderTarget;

namespace Hax::Gui
{
    constexpr size_t kInvalidId = 0;

    struct WndMsg
    {
        void*       Hwnd;
        uint32      Msg;
        uintptr     WParam;
        uint64      LParam;
    };

    struct Texture2D
    {
        void* Tex;
        void* View;
        uint32 Width, Height, Depth;
    };

    struct WndMsgBuffer
    {
        static constexpr size_t kCapacity = 64;
        static constexpr size_t kMask = kCapacity - 1;

        void Write(const WndMsg& msg)
        {
            size_t index = m_Write.load(std::memory_order_relaxed);
            m_Buffer[index] = msg;

            size_t next = (index + 1) & kMask;
            m_Write.store(next, std::memory_order_release);
        }

        bool Read(WndMsg& out)
        {
            size_t read = m_Read.load(std::memory_order_relaxed);
            size_t write = m_Write.load(std::memory_order_acquire);

            if (read == write)
                return false;

            out = m_Buffer[read];
            m_Read.store((read + 1) & kMask, std::memory_order_release);
            return true;
        }

        Array<WndMsg, kCapacity>            m_Buffer;
        alignas(64) std::atomic<size_t>     m_Read = 0;
        alignas(64) std::atomic<size_t>     m_Write = 0;
    };
}

namespace Hax::Gui
{
    // Predefine

    struct MsdfFont;

    // enums

    enum class RenderItemType 
    { 
        Nop = 0,
        Ellipse = 1,
        Rect = 2,
        Triangle = 3,
        Line = 4,
        Image = 5,
        MsdfGlyph = 6,
        Glyph = 7
    };

    enum RenderBatchAction : uint32
    {
        RenderBatchAction_None = 0,
        RenderBatchAction_SetClipRect = 1 << 0,
        RenderBatchAction_SetTexture = 1 << 1,
        RenderBatchAction_SetFont = 1 << 2,
    };

    enum class LayoutType 
    { 
        Root, 
        Container, 
        Horizontal, 
        Vertical 
    };

    enum ContainerFlag
    {
        ContainerFlag_None = 0,
        ContainerFlag_Clip = 1 << 0,
        ContainerFlag_ScrollX = 1 << 1,
        ContainerFlag_ScrollY = 1 << 2,
        ContainerFlag_ScrollVisible = 1 << 3,
        ContainerFlag_FitX = 1 << 4,
        ContainerFlag_FitY = 1 << 5,
    };

    // classes

    struct Timer
    {
        int64   TicksPerSecond;
        int64   TotalTicksLastFrame;
        double  Time;
        float   DeltaTime;

        float   FramerateSecPerFrameAccum;
        float   FramerateSecPerFrame[60];
        int     FramerateSecPerFrameIdx;
        int     FramerateSecPerFrameCount = 1;
        float   Framerate;
    };

    struct Mouse
    {
        bool        IsLmbJustPressed() const    { return LmbDown && !PrevLmbDown; }
        bool        IsLmbJustReleased() const   { return PrevLmbDown && !LmbDown; }

        Vector2     Pos;
        Vector2     DeltaPos;
        Vector2     DeltaWheel;

        float       DownDuration = -1.f;
        float       PrevDownDuration = -1.f;
        bool        LmbDown;
        bool        PrevLmbDown;

        MouseIcon   PrevIcon = MouseIcon_Default;
        MouseIcon   Icon = MouseIcon_Default;

        Handle      Textures[MouseIcon_COUNT];
    };

    struct Viewport
    {
        Vector2     Size;
        Handle      Hwnd;
        float       ScaleFactor;
    };

    struct Layout
    {
        bool        IsRoot() const          { return Type == LayoutType::Root; }
        bool        IsHorizontal() const    { return Type == LayoutType::Horizontal; }
        bool        IsVertical() const      { return Type == LayoutType::Vertical || Type == LayoutType::Container; }

        Vector2     CursorPos;
        Rect        Bounds;
        float       Spacing;
        LayoutType  Type;
    };

    struct Container
    {
        bool        ScrollX() const         { return Flags & ContainerFlag_ScrollX; }
        bool        ScrollY() const         { return Flags & ContainerFlag_ScrollY; }
        bool        ScrollVisible() const   { return Flags & ContainerFlag_ScrollVisible; }

        bool        FitsX() const           { return Flags & ContainerFlag_FitX; }
        bool        FitsY() const           { return Flags & ContainerFlag_FitY; }

        bool        Clipping() const        { return Flags & ContainerFlag_Clip; }
        bool        RequiresState() const   { return Flags > ContainerFlag_Clip; }

        size_t      Id;
        Rect        Bounds;
        uint32      Flags;
    };

    __declspec(align(16)) struct RenderItem
    {

        bool operator==(const RenderItem& other) const { return memcmp(this, &other, sizeof(RenderItem)) == 0; }

        union
        {
            struct
            {
                float Params14[4];
                float Params58[4];
                float Params912[4];
                float Param13;
                float Param14;
            };

            struct { Vector2 Center; Vector2 R; float BorderTh; } Ellipse;
            struct { Vector2 Min; Vector2 Max; Vector4 R; float BorderTh; } Rect;
            struct { Vector2 A; Vector2 B; float Th; } Line;
            struct { Vector2 A; Vector2 B; Vector2 C; float BorderTh; } Triangle;
            struct { Vector2 A; Vector2 B; Vector2 UVmin; Vector2 UVmax; float R; } Image;
            struct { Vector2 A; Vector2 B; Vector2 UVmin; Vector2 UVmax; float Index; float PxRange; Vector2 AtlasSize; float Weight; float Outline; } MsdfGlyph;
            struct { Vector2 A; Vector2 B; Vector2 UVmin; Vector2 UVmax; } Glyph;
        };

        RenderItemType  Type;
        Color           Color1, Color2;
        float           Sin, Cos;
    };

    struct RenderItemRes
    { 
        const MsdfFont* Font;
        void* Image;
    };

    struct RenderBatch
    {
        Optional<Rect>  ClipRect;
        const MsdfFont*     Font;
        void*           Texture;
        uint32          InstancesNum;
        uint32          ActionMask = RenderBatchAction_None;
    };

    struct Rotation
    {
        float Angle;
        float Sin;
        float Cos;
    };

    struct Layer
    {
        size_t              Id;
        int                 ZOrder;
        int                 SkipCounter;

        Vector<RenderItem>  RenderItems;
        Vector<RenderBatch> RenderBatches;

        Vector<Rotation>    RotationStack;
        Rotation            CurrentRotation;

        Vector<Rect>        ClipRectStack;
        Rect                CurrentClipRect;

        Vector<Layout>      LayoutStack;
        Layout              CurrentLayout;

        Vector<Container>   ContainerStack;
        Container           CurrentContainer;
        size_t              CurrentScrollId;
    };

    struct IBackend
    {
        virtual ~IBackend() = default;

        virtual void Render() = 0;

        //virtual Texture2D CreateAtlasArray(int width, int height, int depth) = 0;
        //virtual void SetSubarray(Texture2D array, uint32 depth, uint32 w, uint32 h, const uint8* data) = 0;
        virtual Texture2D CreateTexture(TextureFormat format, const uint8* pixels, uint32 width, uint32 height, uint32 depth = 1) = 0;
        //virtual Texture2D CreateTextureR8(const uint8* pixels, int width, int height) = 0;
        virtual void UpdateTextureRegion(Texture2D tex, const RectU& region, const uint8* src, uint32 srcPitch, uint32 arraySlice = 0) = 0;
        virtual void DestroyTexture(Texture2D handle) = 0;
    };

    __declspec(align(16)) struct FontConstData
    {
        float PxRange;
        float TexWidth;
        float TexHeight;
    };
    constexpr uint32 kBitmapSize = 1024;
    struct Interaction
    {
        size_t FocusedItemId;
        size_t ClickedItemId;
        struct
        {
            struct { size_t Id = kInvalidId; int ZOrder = INT_MIN; } HoveredItem, HoveredContainer;
            size_t ActiveItemId;
        } ThisFrame, NextFrame;
    };

    struct KeyData
    {
        bool        Down = false;
        float       DownDuration = -1.f;
        float       DownDurationPrev = -1.f;
    };

    struct Font;
    struct MsdfFont;

    struct Context
    {
        LogFile*            Logger;
        Allocator           GeneralAlloc{"General"};
        Allocator           FontAlloc{"Font"};

        Timer               Timer;
        Mouse               Mouse;
        Viewport            Viewport;
        IBackend*           Backend;
        Interaction         Interaction;

        Vector<Layer*>      Layers{GeneralAlloc};
        Vector<Layer*>      LayerStack{GeneralAlloc};
        Layer*              CurrentLayer;
        Layer*              DefaultLayer;

        WndMsgBuffer        WndMsgQueue;

        Vector<MsdfFont*>   MsdfFonts{GeneralAlloc};
        Vector<Font*>       Fonts{GeneralAlloc};
        Vector<Texture2D>   Textures{GeneralAlloc};

        HashMap<size_t, IStatePool*>  StatePools{GeneralAlloc};

        KeyData             KeysData[256]{};

        Texture2D           AtlasArray;

        struct
        {
            uint8*          Pixels;
            Texture2D       Texture;
            RectU           DirtyRect;
            uint32          CurX = 2, CurY = 2, LineH;
        } Bitmap;

        IDWriteFactory5* DWriteFactory;
        IDWriteInMemoryFontFileLoader* DWriteFontFileLoader;

        Vector<uint8> KeysPressedThisFrame{GeneralAlloc};
        Vector<char16> CharsPressedThisFrame{GeneralAlloc};

        bool ShouldCoUninitialize;

        float Gamma, Contrast;
        uint8 GammaLUT[256];
    };

    extern Context* g_Context;

    inline int GetChannelCount(TextureFormat format)
    {
        switch (format)
        {
            case TextureFormat::R8_UNorm:
                return 1;
            case TextureFormat::R8G8B8A8_UNorm:
                return 4;
        }

        return 0;
    }

    inline int GetBytesPerChannel(TextureFormat format)
    {
        switch (format)
        {
            case TextureFormat::R8_UNorm:
            case TextureFormat::R8G8B8A8_UNorm:
                return 1;
        }

        return 0;
    }
}
#endif