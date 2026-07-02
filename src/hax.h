#pragma once

#include <crtdbg.h>
#include <type_traits>
#include <cmath>
#include <cstdarg>
#include <atomic>
#include <bit>
#include <initializer_list>
#include <stdio.h>

#define HAX_ASSERT(expr) _ASSERTE(expr)
#define HAX_PANIC(expr, logFile, fmt, ...) do { if (!(expr)) Hax::Panic(logFile, fmt, ##__VA_ARGS__); } while (0)

namespace Hax
{
    using int64 = int64_t;
    using uint64 = uint64_t;
    using uint32 = uint32_t;
    using uint16 = uint16_t;
    using uint8 = uint8_t;
    using char16 = wchar_t;
    using uintptr = uintptr_t;
    using Handle = size_t;

    constexpr size_t kSizeMax = (size_t)-1;

    template <typename T> concept TriCo = std::is_trivially_copyable_v<T>;

    template <TriCo T> constexpr void Swap(T& a, T& b) noexcept { T temp = a; a = b; b = temp; }
    template <TriCo T> constexpr T Max(T a, T b) noexcept { return (a < b) ? b : a; }
    template <TriCo T> constexpr T Min(T a, T b) noexcept { return (b < a) ? b : a; }

    //=============================================================================
    // [SECTION] MATHEMATICS
    //=============================================================================

    constexpr float kPi = 3.14159265358979323846f;

    template <TriCo T> constexpr T Clamp(T v, T lo, T hi) noexcept { return (v < lo) ? lo : (hi < v) ? hi : v; }
    template <TriCo T> constexpr T Lerp(T a, T b, float t) noexcept { return static_cast<T>(a + t * (b - a)); }
    template <TriCo T> constexpr T Abs(T v) noexcept { return v < 0 ? -v : v; }

    inline float Sqrt(float v) noexcept { HAX_ASSERT(v >= 0.f); return ::sqrtf(v); }
    inline float Sin(float radians) noexcept { return ::sinf(radians); }
    inline float Cos(float radians) noexcept { return ::cosf(radians); }

    inline float Floor(float v) noexcept { return ::floorf(v); }
    inline float Ceil(float v) noexcept { return ::ceilf(v); }
    inline float Round(float v) noexcept { return ::roundf(v); }

    constexpr float Trunc(float v) noexcept { return v < 0.f ? -static_cast<float>(static_cast<int>(-v)) : static_cast<float>(static_cast<int>( v)); }
    constexpr float Mod(float x, float y) noexcept { if (y == 0.f) return 0.f; return x - Trunc(x / y) * y; }
    
    constexpr float RadToDeg(float angleRad) { return angleRad * 180.f / kPi; }
    constexpr float DegToRad(float angleDeg) { return angleDeg * kPi / 180.f; }

    struct Vector2
    {
        constexpr bool          operator==(const Vector2& o) const  = default;
        constexpr bool          operator!=(const Vector2& o) const  = default;
        constexpr explicit      operator bool() const               { return X != 0 && Y != 0; }

        constexpr Vector2       operator*(const float v) const      { return Vector2(X * v, Y * v); }
        constexpr Vector2       operator*(const Vector2& o) const   { return Vector2(X * o.X, Y * o.Y); }
        constexpr Vector2       operator/(const float v) const      { return Vector2(X / v, Y / v); }
        constexpr Vector2       operator/(const Vector2& o) const   { return Vector2(X / o.X, Y / o.Y); }
        constexpr Vector2       operator+(const Vector2& o) const   { return Vector2(X + o.X, Y + o.Y); }
        constexpr Vector2       operator-(const Vector2& o) const   { return Vector2(X - o.X, Y - o.Y); }

        constexpr Vector2&      operator*=(const float rhs)         { X *= rhs; Y *= rhs; return *this; }
        constexpr Vector2&      operator/=(const float rhs)         { X /= rhs; Y /= rhs; return *this; }
        constexpr Vector2&      operator+=(const Vector2& rhs)      { X += rhs.X; Y += rhs.Y; return *this; }
        constexpr Vector2&      operator-=(const Vector2& rhs)      { X -= rhs.X; Y -= rhs.Y; return *this; }
        constexpr Vector2&      operator*=(const Vector2& rhs)      { X *= rhs.X; Y *= rhs.Y; return *this; }
        constexpr Vector2&      operator/=(const Vector2& rhs)      { X /= rhs.X; Y /= rhs.Y; return *this; }

        float                   Length() const                      { return Sqrt(X * X + Y * Y); }

        float                   X, Y;
    };
    constexpr Vector2 Min(const Vector2& v1, const Vector2& v2) { return Vector2(Min(v1.X, v2.X), Min(v1.Y, v2.Y)); }
    constexpr Vector2 Max(const Vector2& v1, const Vector2& v2) { return Vector2(Max(v1.X, v2.X), Max(v1.Y, v2.Y)); }
    inline Vector2 Round(const Vector2& v) { return Vector2(Round(v.X), Round(v.Y)); }
    inline Vector2 Floor(const Vector2& v) { return Vector2(Floor(v.X), Floor(v.Y)); }
    inline Vector2 Trunc(const Vector2& v) { return Vector2(Trunc(v.X), Trunc(v.Y)); }

    struct Vector3
    {
        constexpr Vector3       operator+(float rhs) const          { return Vector3(X + rhs, Y + rhs, Z + rhs); }
        constexpr Vector3       operator-(float rhs) const          { return Vector3(X - rhs, Y - rhs, Z - rhs); }

        float                   X, Y, Z;
    };

    struct Vector4
    {
        constexpr Vector4() = default;
        constexpr Vector4(float x, float y, float z, float w) : X(x), Y(y), Z(z), W(w) {}
        constexpr Vector4(float v) : X(v), Y(v), Z(v), W(v) {}

        constexpr Vector4       operator+(float rhs) const          { return Vector4(X + rhs, Y + rhs, Z + rhs, W + rhs); }
        constexpr Vector4       operator-(float rhs) const          { return Vector4(X - rhs, Y - rhs, Z - rhs, W - rhs); }

        float                   X, Y, Z, W;
    };

    struct Rect
    {
        static Rect             FromFloats(float xMin, float yMin, float xMax, float yMax)  { return {{xMin, yMin}, {xMax, yMax}}; }
        static Rect             FromMinMax(const Vector2& min, const Vector2& max)          { return {min, max}; }
        static Rect             FromPosSize(const Vector2& pos, const Vector2& size)        { return {pos, pos + size}; }

        constexpr bool          operator==(const Rect& o) const     { return Min == o.Min && Max == o.Max; }

        constexpr Vector2       GetCenter() const                   { return Vector2((Min.X + Max.X) * 0.5f, (Min.Y + Max.Y) * 0.5f); }
        constexpr Vector2       GetSize() const                     { return Max - Min; }
        constexpr Vector2       GetTL() const                       { return Min; }
        constexpr Vector2       GetTR() const                       { return Vector2(Max.X, Min.Y); }
        constexpr Vector2       GetBL() const                       { return Vector2(Min.X, Max.Y); }
        constexpr Vector2       GetBR() const                       { return Max; }
        constexpr float         GetHeight() const                   { return Max.Y - Min.Y; }

        constexpr bool          Contains(const Vector2& p) const    { return p.X >= Min.X && p.Y >= Min.Y && p.X < Max.X && p.Y < Max.Y; }
        constexpr bool          Contains(const Rect& r) const       { return Contains(r.Min) && Contains(r.Max); }
        constexpr bool          Intersects(const Rect& r) const     { return r.Min.X < Max.X && r.Min.Y < Max.Y && r.Max.X > Min.X && r.Max.Y > Min.Y; }
        constexpr void          ClipWith(const Rect& o)             { Min = Hax::Max(o.Min, Min); Max = Hax::Min(o.Max, Max); }
        constexpr Rect          Intersect(const Rect& o) const      { return Rect(Hax::Max(o.Min, Min), Hax::Min(o.Max, Max)); }
        constexpr bool          IsInverted() const                  { return Min.X > Max.X || Min.Y > Max.Y; }

        constexpr void          Add(const Vector2& p)               { if (Min.X > p.X) Min.X = p.X; if (Min.Y > p.Y) Min.Y = p.Y; if (Max.X < p.X) Max.X = p.X; if (Max.Y < p.Y) Max.Y = p.Y; }
        constexpr void          Add(const Rect& r)                  { if (Min.X > r.Min.X) Min.X = r.Min.X; if (Min.Y > r.Min.Y) Min.Y = r.Min.Y; if (Max.X < r.Max.X) Max.X = r.Max.X; if (Max.Y < r.Max.Y) Max.Y = r.Max.Y; }

        Vector2                 Min, Max;
    };

    struct RectU
    {
        bool                    IsInverted() const                  { return MinX > MaxX || MinY > MaxY; }

        uint32                  MinX, MinY, MaxX, MaxY;
    };

    //=============================================================================
    // [SECTION] VOCABULARY TYPES
    //=============================================================================

    template <TriCo T>
    struct Optional
    {
        constexpr Optional() : m_Empty(0), m_HasValue(false) {}
        constexpr Optional(const T& val) : m_Value(val), m_HasValue(true) {}
        constexpr Optional(const Optional&) = default;

        constexpr Optional&     operator=(const Optional&) = default;
        constexpr Optional&     operator=(const T& val)             { m_Value = val; m_HasValue = true; return *this; }

        constexpr void          Reset()                             { m_HasValue = false; }

        constexpr bool          HasValue() const                    { return m_HasValue; }
        constexpr T&            Value()                             { HAX_ASSERT(m_HasValue); return m_Value; }
        constexpr const T&      Value() const                       { HAX_ASSERT(m_HasValue); return m_Value; }

        constexpr T             ValueOr(const T& defaultValue) const{ return m_HasValue ? m_Value : defaultValue; }

        constexpr T*            operator->()                        { return &Value(); }
        constexpr T&            operator*()                         { return Value(); }

        constexpr bool          operator==(const T& val) const      { return m_HasValue && m_Value == val; }
        constexpr bool          operator==(const Optional& other) const { if (m_HasValue != other.m_HasValue) return false; return !m_HasValue || m_Value == other.m_Value; }

        constexpr explicit      operator bool() const { return m_HasValue; }

    private:
        union 
        {
            char m_Empty;
            T    m_Value;
        };
        bool     m_HasValue;
    };
    static_assert(std::is_trivially_copyable_v<Optional<int>>);

    template <TriCo T1, TriCo T2>
    struct Pair
    {
        bool                    operator==(const Pair<T1, T2>& o) const { return First == o.First && Second == o.Second; }

        T1                      First;
        T2                      Second;
    };
    static_assert(std::is_trivially_copyable_v<Pair<int, int>>);

    //=============================================================================
    // [SECTION] VIEWS
    //=============================================================================

    template <typename T> concept CharT = std::is_same_v<T, char> || std::is_same_v<T, wchar_t>;
    template <CharT T> constexpr size_t StrLen(const T* s) noexcept { if (!s) return 0; size_t len = 0; while (s[len] != 0) len++; return len; }

    template<TriCo T>
    struct Span
    {
                                Span() = default;
                                Span(T* data, size_t size) : m_Data(data), m_Size(size) {}
        template<size_t N>      Span(T (&arr)[N]) : m_Data(arr), m_Size(N) {}
        template<typename Cont> Span(Cont& c) : m_Data(c.Data()), m_Size(c.Size()) {}

        const T&                operator[](size_t idx) const        { HAX_ASSERT(idx < m_Size); return m_Data[idx]; }
        T&                      operator[](size_t idx)              { HAX_ASSERT(idx < m_Size); return m_Data[idx]; }

        const T*                begin() const                       { return m_Data; }
        T*                      begin()                             { return m_Data; }
        const T*                end()   const                       { return m_Data + m_Size; }
        T*                      end()                               { return m_Data + m_Size; }

        size_t                  Size() const                        { return m_Size; }
        bool                    Empty() const                       { return m_Size == 0; }

        const T*                Data() const                        { return m_Data; }

    private:
        T*                      m_Data = nullptr;
        size_t                  m_Size = 0;
    };

    template <CharT T>
    struct StringViewImpl
    {
        constexpr                   StringViewImpl() = default;
        constexpr                   StringViewImpl(const StringViewImpl& other) = default;
        constexpr                   StringViewImpl(const T* str, size_t len) : m_Str(str), m_Len(len) {}
        constexpr                   StringViewImpl(const T* s) : m_Str(s), m_Len(StrLen(s)) {}
                                    StringViewImpl(std::nullptr_t) = delete;
                                    StringViewImpl(std::nullptr_t, size_t len) = delete;

        constexpr StringViewImpl&   operator=(const StringViewImpl& other) = default;

        constexpr const T*          begin()  const                                  { return m_Str; }
        constexpr const T*          end()    const                                  { return m_Str + m_Len; }

        constexpr size_t            Length() const                                  { return m_Len; }
        constexpr size_t            Size() const                                    { return m_Len; }
        constexpr bool              Empty() const                                   { return m_Len == 0; }

        constexpr const T&          operator[](size_t i) const                      { HAX_ASSERT(i < m_Len); return m_Str[i]; }

        constexpr const T&          First() const                                   { HAX_ASSERT(m_Len > 0); return m_Str[0]; }
        constexpr const T&          Last() const                                    { HAX_ASSERT(m_Len > 0); return m_Str[m_Len - 1]; }

        constexpr const T*          Data() const                                    { return m_Str; }

        constexpr StringViewImpl    Substr(size_t pos, size_t n = kSizeMax) const   { HAX_ASSERT(pos <= m_Len); return StringViewImpl(m_Str + pos, Min(n, m_Len - pos)); }
        constexpr bool              Equals(const StringViewImpl& other) const       { return Compare(other) == 0; }
        constexpr bool              EndsWith(const StringViewImpl& suf) const       { if (m_Len < suf.m_Len) return false; for (size_t i = 0; i < suf.m_Len; ++i) if (m_Str[m_Len - suf.m_Len + i] != suf.m_Str[i]) return false; return true; }
        constexpr bool              StartsWith(const StringViewImpl& suf) const     { if (m_Len < suf.m_Len) return false; for (size_t i = 0; i < suf.m_Len; ++i) if (m_Str[i] != suf.m_Str[i]) return false; return true; }
        constexpr int               Compare(const StringViewImpl& other) const 
        {
            size_t minLen = (m_Len < other.m_Len) ? m_Len : other.m_Len;

            for (size_t i = 0; i < minLen; ++i)
                if (m_Str[i] != other.m_Str[i])
                    return (int)m_Str[i] - (int)other.m_Str[i];

            if (m_Len < other.m_Len) return -1;
            if (m_Len > other.m_Len) return 1;
            return 0;
        }

        constexpr void              RemovePrefix(size_t n) { n = Min(n, m_Len); m_Str += n; m_Len -= n; }
        constexpr void              RemoveSuffix(size_t n) { n = Min(n, m_Len); m_Len -= n; }

        constexpr bool              operator<(const StringViewImpl& other) const { return Compare(other) < 0; }
        constexpr bool              operator==(const StringViewImpl& other) const   { return Equals(other); }

    private:
        const T*                    m_Str = nullptr;
        size_t                      m_Len = 0;
    };

    using WStringView  = StringViewImpl<wchar_t>;
    using StringView = StringViewImpl<char>;

    //=============================================================================
    // [SECTION] ALLOCATORS
    //=============================================================================

    struct Allocator
    {
        Allocator() = default;
        Allocator(const char* name) : Name(name) {}

        const char*                                 Name = "Unnamed";
        size_t                                      TotalAllocated = 0;
        size_t                                      MaxAllocated = 0;
    };

    extern Allocator g_GlobalAlloc;

    [[nodiscard]] void* Alloc(Allocator& alloc, size_t size);
    [[nodiscard]] inline void* Alloc(size_t size) { return Alloc(g_GlobalAlloc, size); }

    void Free(Allocator& alloc, void* ptr);
    inline void Free(void* ptr) { Free(g_GlobalAlloc, ptr); }

    template<typename T, typename... Args>
    [[nodiscard]] inline T* New(Allocator& alloc, Args&&... args) { void* ptr = Alloc(alloc, sizeof(T)); if (!ptr) return nullptr; return new(ptr) T(std::forward<Args>(args)...); }

    template<typename T, typename... Args>
    [[nodiscard]] inline T* New(Args&&... args) { return New<T>(g_GlobalAlloc, std::forward<Args>(args)...); }

    template<typename T>
    inline void Delete(Allocator& alloc, T* ptr) { if (ptr) { ptr->~T(); Free(alloc, ptr); } }

    template<typename T>
    inline void Delete(T* ptr) { Delete(g_GlobalAlloc, ptr); }

    //=============================================================================
    // [SECTION] HASH
    //=============================================================================

#ifdef _WIN64
    static constexpr size_t kFnvOffset = 0xcbf29ce484222325ULL;
    static constexpr size_t kFnvPrime  = 0x100000001b3ULL;
#else
    static constexpr size_t kFnvOffset = 0x811c9dc5U;
    static constexpr size_t kFnvPrime  = 0x01000193U;
#endif

    inline size_t Hash(const void* data, size_t sizeBytes, size_t seed = kFnvOffset) 
    {
        const uint8* ptr = static_cast<const uint8*>(data);
        for (size_t i = 0; i < sizeBytes; ++i) 
        {
            seed ^= ptr[i];
            seed *= kFnvPrime;
        }
        return seed;
    }

    template <CharT T>
    inline constexpr size_t Hash(StringViewImpl<T> sv) 
    {
        size_t hash = kFnvOffset;
        for (size_t i = 0; i < sv.Length(); ++i) 
        {
            hash ^= static_cast<size_t>(sv[i]);
            hash *= kFnvPrime;
        }
        return hash;
    }

    struct String;
    inline size_t Hash(const String& s);

    template <CharT T>
    inline constexpr size_t Hash(const T* str)
    {
        return Hash(StringViewImpl(str));
    }

    template <TriCo T>
    inline size_t Hash(const T& v)
    {
        T tmp = v;
        __builtin_zero_non_value_bits(&tmp);

        return Hash(&tmp, sizeof(T));
    }

    template <typename T>
    inline size_t Hash(T* v)
    {
        return (size_t)((uintptr_t)v >> 3);
    }

    inline constexpr size_t Hash(int v)             { return static_cast<size_t>(v); }
    inline constexpr size_t Hash(int64 v)           { return static_cast<size_t>(v); }
    inline constexpr size_t Hash(unsigned int v)    { return static_cast<size_t>(v); }

    template <typename T>
    constexpr size_t GetTypeId() 
    { 
        return Hash(__FUNCSIG__);
    }

    //=============================================================================
    // [SECTION] CONTAINERS
    //=============================================================================

    template <TriCo T, size_t N>
    struct Array
    {
        constexpr               Array() = default;
        constexpr               Array(const T& val) { for (size_t i = 0; i < N; ++i) m_Data[i] = val; }
        constexpr               Array(std::initializer_list<T> list) { HAX_ASSERT(list.size() <= N); size_t i = 0; for (const auto& item : list) m_Data[i++] = item; for (; i < N; ++i) m_Data[i] = T{}; }
                        
        constexpr size_t        Size() const { return N; }

        constexpr T&            operator[](size_t index)                { HAX_ASSERT(index < N); return m_Data[index]; }
        constexpr const T&      operator[](size_t index) const          { HAX_ASSERT(index < N); return m_Data[index]; }

        constexpr bool          operator==(const Array<T, N>& o) const  { for (size_t i = 0; i < N; ++i) if (m_Data[i] != o.m_Data[i]) return false; return true; }
        Array&                  operator=(const Array& o) = default;

        constexpr T*            begin()                                 { return m_Data; }
        constexpr const T*      begin() const                           { return m_Data; }
        constexpr T*            end()                                   { return m_Data + N; }
        constexpr const T*      end() const                             { return m_Data + N; }

    private:
        T                       m_Data[N];
    };

    template <TriCo T>
    struct Vector
    {
        Vector() = default;
        Vector(Allocator& alloc) : m_Alloc(&alloc)                  {}
        Vector(const Vector<T>& src)                                { m_Alloc = src.m_Alloc; operator=(src); }
        Vector(Vector<T>&& src)                                     { m_Alloc = src.m_Alloc; operator=(std::move(src)); }
        ~Vector()                                                   { ClearFree(); }

        Vector<T>&          operator=(const Vector<T>& src)         { if (&src == this) return *this; ClearFree(); Resize(src.m_Size); if (src.m_Data) memcpy(m_Data, src.m_Data, m_Size * sizeof(T)); return *this; }
        Vector<T>&          operator=(Vector<T>&& src)              { if (&src == this) return *this; ClearFree(); m_Alloc = src.m_Alloc; m_Data = src.m_Data; m_Size = src.m_Size; m_Capacity = src.m_Capacity; src.m_Data = nullptr; src.m_Capacity = src.m_Size = 0; return *this; }

        void                Clear()                                 { m_Size = 0; }
        void                ClearFree()                             { if (m_Data) { m_Size = m_Capacity = 0; Free(*m_Alloc, m_Data); m_Data = nullptr; } }

        T*                  Data()                                  { return m_Data; }
        const T*            Data() const                            { return m_Data; }
        bool                Empty() const                           { return m_Size == 0; }
        size_t              Size() const                            { return m_Size; }
        size_t              Capacity() const                        { return m_Capacity; }

        T&                  operator[](size_t i)                    { HAX_ASSERT(i >= 0 && i < m_Size); return m_Data[i]; }
        const T&            operator[](size_t i) const              { HAX_ASSERT(i >= 0 && i < m_Size); return m_Data[i]; }

        T*                  begin()                                 { return m_Data; }
        const T*            begin() const                           { return m_Data; }
        T*                  end()                                   { return m_Data + m_Size; }
        const T*            end() const                             { return m_Data + m_Size; }

        T&                  First()                                 { HAX_ASSERT(m_Size > 0); return m_Data[0]; }
        const T&            First() const                           { HAX_ASSERT(m_Size > 0); return m_Data[0]; }
        T&                  Last()                                  { HAX_ASSERT(m_Size > 0); return m_Data[m_Size - 1]; }
        const T&            Last() const                            { HAX_ASSERT(m_Size > 0); return m_Data[m_Size - 1]; }

        void                EnsureCapacity(size_t desired);
        void                Resize(size_t newSize)                  { EnsureCapacity(newSize); m_Size = newSize; }
        void                Resize(size_t newSize, const T& v)      { EnsureCapacity(newSize); if (newSize > m_Size) for (size_t i = m_Size; i < newSize; ++i) memcpy(&m_Data[i], &v, sizeof(v)); m_Size = newSize; }
        void                Reserve(size_t newCapacity);

        T*                  Insert(const T* it, const T& v);
        T*                  Insert(size_t off, const T& v);
        T*                  Erase(const T* it);
        T*                  Erase(size_t off);
        T*                  Erase(const T* it, const T* itLast);    

        void                PushBack(const T& v)                    { EnsureCapacity(m_Size + 1); m_Data[m_Size++] = v; }
        void                PopBack()                               { HAX_ASSERT(m_Size > 0); m_Size--; }
        void                PushFront(const T& v)                   { if (m_Size == 0) PushBack(v); else Insert(m_Data, v); }

        bool                Contains(const T& v) const              { const T* data = m_Data; const T* dataEnd = m_Data + m_Size; while (data < dataEnd) if (*data++ == v) return true; return false; }
        T*                  Find(const T& v)                        { T* data = m_Data; const T* dataEnd = m_Data + m_Size; while (data < dataEnd) if (*data == v) break; else ++data; return data; }
        const T*            Find(const T& v) const                  { const T* data = m_Data; const T* dataEnd = m_Data + m_Size; while (data < dataEnd) if (*data == v) break; else ++data; return data; }
        size_t              FindIndex(const T& v) const             { const T* end = m_Data + m_Size; const T* it = Find(v); if (it == end) return (size_t)-1; return (size_t)(it - m_Data); }
        bool                FindErase(const T& v)                   { const T* it = Find(v); if (it < m_Data + m_Size) { Erase(it); return true; } return false; }

    private:
        Allocator*          m_Alloc = &g_GlobalAlloc;
        size_t              m_Size = 0;
        size_t              m_Capacity = 0;
        T*                  m_Data = nullptr;
    };

    struct String
    {
        constexpr String() = default;
        String(const wchar_t* str, Allocator& alloc) : m_Data(alloc){ operator=(str); }
        String(const wchar_t* str)                                  { operator=(str); }
        String(Allocator& alloc) : m_Data(alloc)                    {}
        String(const String& str) : m_Data(str.m_Data)              {}
        String(String&& str) : m_Data(std::move(str.m_Data))        {}
        String(std::nullptr_t) = delete;
        ~String()                                                   { ClearFree(); }

        String&             operator=(const wchar_t* str);
        String&             operator=(const String& str)            { operator=(str.CStr()); }

        const wchar_t*      CStr()   const                          { return Empty() ? L"" : m_Data.Data(); }
        bool                Empty() const                           { return m_Data.Size() <= 1; }
        size_t              Length() const                          { size_t sz = m_Data.Size(); return sz > 0 ? sz - 1 : 0; }
        size_t              Size() const                            { return Length(); }

        wchar_t&            First()                                 { return m_Data.First(); }
        wchar_t&            Last()                                  { return m_Data.Last(); }

        void                Clear()                                 { m_Data.Clear(); }
        void                ClearFree()                             { m_Data.ClearFree(); }
        void                Reserve(size_t n)                       { m_Data.Reserve(n); }
        wchar_t*            Data()                                  { return m_Data.Data(); }
        void                Erase(size_t i)                         { m_Data.Erase(i); }

        void                Append(const wchar_t* str);

        bool                Equals(const wchar_t* str) const;

        bool                operator==(const wchar_t* str) const    { return Equals(str); }
        bool                operator==(const String& other) const   { return Equals(other.CStr()); }

        wchar_t&            operator[](size_t i)                    { return m_Data[i]; }
        const wchar_t&      operator[](size_t i) const              { return m_Data[i]; }

        Vector<wchar_t>     m_Data{g_GlobalAlloc};
    };
    inline size_t Hash(const String& s) { return Hash(StringViewImpl(s.CStr(), s.Length())); }

    template<CharT T, size_t MaxLength = 128>
    struct BasicStringBuilder
    {
        static_assert(MaxLength <= 512);

        BasicStringBuilder() = default;

        T&                  operator[](size_t idx)                  { HAX_ASSERT(idx < m_Length); return m_Data[idx]; }
        const T&            operator[](size_t idx) const            { HAX_ASSERT(idx < m_Length); return m_Data[idx]; }

        void                Clear()                                 { m_Data[0] = 0; m_Length = 0; }
        bool                Empty() const                           { return m_Length == 0; }
        size_t              Length() const                          { return m_Length; }
        size_t              Size() const                            { return m_Length; }
        size_t              SizeInBytes() const                     { return m_Length * sizeof(T); }

        void                AppendF(const T* fmt, ...);
        void                Append(StringViewImpl<T> sv);

        StringViewImpl<T>   View()                                  { return StringViewImpl<T>(m_Data, m_Length); }
        const T*            CStr() const                            { return m_Data; }
        T*                  CStr()                                  { return m_Data; }

    private:
        int                 VPrintf(char* buf, size_t bufSize, const char* fmt, va_list args)       { return vsnprintf(buf, bufSize, fmt, args); }
        int                 VPrintf(wchar_t* buf, size_t bufSize, const wchar_t* fmt, va_list args) { return vswprintf(buf, bufSize, fmt, args); }

        T                   m_Data[MaxLength + 1]{};
        size_t              m_Length = 0;
    };

    template <size_t MaxLen = 128> using StringBuilder = BasicStringBuilder<char, MaxLen>;
    template <size_t MaxLen = 128> using WStringBuilder = BasicStringBuilder<char16, MaxLen>;

    template <TriCo K, TriCo V>
    struct Map
    {
        struct Entry
        {
            K key;
            V value;
        };

        Map() = default;
        Map(Allocator& alloc) : m_Entries(alloc)                    {}
        ~Map()                                                      { ClearFree(); }

        const V*            Get(const K& key) const;
        V*                  Get(const K& key)                       { return const_cast<V*>(static_cast<const Map*>(this)->Get(key)); }

        V&                  FindOrAdd(const K& key);

        void                Insert(const K& key, const V& value)    { FindOrAdd(key) = value; }
        bool                Erase(const K& key);

        bool                Contains(const K& key) const            { return Get(key) != nullptr; }
        void                Clear()                                 { m_Entries.Clear(); }
        void                ClearFree()                             { m_Entries.ClearFree(); }
        void                Reserve(size_t cap)                     { m_Entries.Reserve(cap); }
        size_t              Size() const                            { return m_Entries.Size(); }
        bool                Empty() const                           { return m_Entries.Size() == 0; }

        Entry*              begin()                                 { return m_Entries.begin(); }
        const Entry*        begin() const                           { return m_Entries.begin(); }
        Entry*              end()                                   { return m_Entries.end(); }
        const Entry*        end() const                             { return m_Entries.end(); }

    private:
        size_t              FindLowerBound(const K& key) const;

        Vector<Entry>       m_Entries;
    };

    template <typename Container, typename EntryType>
    struct HashMapIterator
    {
        HashMapIterator(EntryType* ptr, EntryType* end) : m_Ptr(ptr), m_End(end) { SkipInvalid(); }

        EntryType&          operator*()  const                              { return *m_Ptr; }
        EntryType*          operator->() const                              { return m_Ptr; }

        HashMapIterator&    operator++()                                    { m_Ptr++; SkipInvalid(); return *this; }

        bool                operator==(const HashMapIterator& other) const  { return m_Ptr == other.m_Ptr; }
        bool                operator!=(const HashMapIterator& other) const  { return m_Ptr != other.m_Ptr; }

        void                SkipInvalid();

        EntryType* m_Ptr;
        EntryType* m_End;
    };

    template <TriCo K, TriCo V>
    struct HashMap
    {
        enum class SlotState : uint8 { Empty, Occupied, Deleted };

        struct Entry
        {
            K Key;
            V Value;
            SlotState State = SlotState::Empty;
        };

        using Iterator = HashMapIterator<HashMap, Entry>;
        using ConstIterator = HashMapIterator<const HashMap, const Entry>;

        HashMap() = default;
        HashMap(Allocator& alloc) : m_Entries(alloc) {}
        ~HashMap() { ClearFree(); }

        Iterator            begin()                                         { return Iterator(m_Entries.begin(), m_Entries.end()); }
        Iterator            end()                                           { return Iterator(m_Entries.end(), m_Entries.end()); }

        ConstIterator       begin() const                                   { return ConstIterator(m_Entries.begin(), m_Entries.end()); }
        ConstIterator       end() const                                     { return ConstIterator(m_Entries.end(), m_Entries.end()); }

        const V*            Get(const K& key) const;
        V*                  Get(const K& key)                               { return const_cast<V*>(static_cast<const HashMap*>(this)->Get(key)); }

        V&                  FindOrAdd(const K& key);

        void                Insert(const K& key, const V& value)            { FindOrAdd(key) = value; }
        bool                Erase(const K& key);

        void                Reserve(size_t cap)                             { if (cap > m_Entries.Size()) Grow(cap); }

        void                Clear()                                         { for (auto& e : m_Entries) e.State = SlotState::Empty; m_Count = 0; }
        void                ClearFree()                                     { m_Entries.ClearFree(); m_Count = 0; }
        size_t              Size()      const                               { return m_Count; }

    private:
        const Entry*        GetEntry(const K& key) const;
        Entry*              GetEntry(const K& key)                          { return const_cast<Entry*>(static_cast<const HashMap*>(this)->GetEntry(key)); }

        void                Grow(size_t newCap = 0);

        Vector<Entry>       m_Entries;
        size_t              m_Count = 0;
    };

    template <TriCo T>
    class TripleBuffer 
    {
    public:
        TripleBuffer(size_t capacity);

        Vector<T>&          GetBack() { return m_Buffers[m_BackIndex]; }
        Vector<T>&          GetFront();
        void                RefreshSpare();

    private:

        struct SpareInfo 
        {
            int     Index;
            bool    IsFresh;
        };

        Vector<T>               m_Buffers[3];
        std::atomic<SpareInfo>  m_Spare;
        int                     m_FrontIndex;
        int                     m_BackIndex;
    };

    template <TriCo T>
    TripleBuffer<T>::TripleBuffer(size_t capacity) :  m_Buffers(), m_Spare(SpareInfo(1, false)), m_FrontIndex(0), m_BackIndex(2)
    { 
        m_Buffers[0].Reserve(capacity);
        m_Buffers[1].Reserve(capacity);
        m_Buffers[2].Reserve(capacity);
    }

    template <TriCo T>
    Vector<T>& TripleBuffer<T>::GetFront()
    {
        SpareInfo spare = m_Spare.load(std::memory_order_relaxed);
        if (spare.IsFresh) 
        {
            SpareInfo newInfo{m_FrontIndex, false};
            SpareInfo prevInfo = m_Spare.exchange(newInfo, std::memory_order_acq_rel);
            m_FrontIndex = prevInfo.Index;
        }
        return m_Buffers[m_FrontIndex];
    }

    template <TriCo T>
    void TripleBuffer<T>::RefreshSpare() 
    { 
        SpareInfo newInfo{m_BackIndex, true}; 
        SpareInfo prevInfo = m_Spare.exchange(newInfo, std::memory_order_acq_rel); 
        m_BackIndex = prevInfo.Index;
    }

    //=============================================================================
    // [SECTION] INI FILE
    //=============================================================================

    using IniStringBuilder = StringBuilder<128>;
    using IniFileWrite = void(*)(void*, IniStringBuilder&);
    using IniFileRead = void(*)(void*, const char*);

    inline void IniFileWrite_Int(void* data, IniStringBuilder& sb)     { sb.AppendF("%d", *(int*)data); }
    inline void IniFileWrite_Float(void* data, IniStringBuilder& sb)   { sb.AppendF("%.3f", *(float*)data); }
    inline void IniFileWrite_Bool(void* data, IniStringBuilder& sb)    { sb.AppendF("%d", *(bool*)data); }
    inline void IniFileWrite_Vector2(void* data, IniStringBuilder& sb) { Vector2* vec = (Vector2*)data; sb.AppendF("%.3f,%.3f", vec->X, vec->Y); }

    inline void IniFileRead_Int(void* data, const char* str)        { sscanf_s(str, "%d", (int*)data); }
    inline void IniFileRead_Float(void* data, const char* str)      { sscanf_s(str, "%f", (float*)data); }
    inline void IniFileRead_Bool(void* data, const char* str)       { int v; sscanf_s(str, "%d", &v); *(bool*)data = v; }
    inline void IniFileRead_Vector2(void* data, const char* str)    { Vector2* vec = (Vector2*)data; sscanf_s(str, "%f,%f", &vec->X, &vec->Y); }

    struct IniFileSection;

    struct IniFile
    {
        IniFile(const char16* fileName) : FileName(fileName) {}
        ~IniFile();

        Vector<IniFileSection*> Sections;
        const char16*           FileName;
    };

    void IniSave(IniFile& iniFile);
    void IniLoad(IniFile& iniFile);
    void IniAddEntry(IniFile& iniFile, const char* section, const char* name, void* data, IniFileWrite writeFn, IniFileRead readFn);
    void IniRemoveEntry(IniFile& iniFile, const char* section, const char* name);

    //=============================================================================
    // [SECTION] LOGGING
    //=============================================================================

    struct LogFile
    {
        Handle hFile;
        Handle hMutex;
        Handle hConsole;
    };

    void InitLogFile(LogFile& logFile, const char16* fileName, bool useConsole);
    void CloseLogFile(LogFile& logFile);
    void Log(LogFile& logFile, const char16* fmt, ...);
    void LogDebug(LogFile& logFile, const char16* fmt, ...);
    void LogWarning(LogFile& logFile, const char16* fmt, ...);
    void LogError(LogFile& logFile, const char16* fmt, ...);

    void Panic(LogFile* logFile, const char16* fmt, ...);

    //=============================================================================
    // [SECTION] IMPLEMENTATION
    //=============================================================================

    template <TriCo T>
    void Vector<T>::EnsureCapacity(size_t desired)
    {  
        if (desired <= m_Capacity)
            return;

        size_t newCapacity = m_Capacity ? (m_Capacity + m_Capacity / 2) : 8;
        newCapacity = Max(newCapacity, desired);

        T* newData = (T*)Alloc(*m_Alloc, newCapacity * sizeof(T));
        if (m_Data)
        {
            memcpy(newData, m_Data, sizeof(T) * m_Size);
            Free(*m_Alloc, m_Data);
        }

        m_Data = newData;
        m_Capacity = newCapacity;
    }

    template <TriCo T>
    void Vector<T>::Reserve(size_t newCapacity)
    { 
        if (newCapacity <= m_Capacity) 
            return;

        T* newData = (T*)Alloc(*m_Alloc, newCapacity * sizeof(T)); 
        if (m_Data)
        {
            memcpy(newData, m_Data, m_Size * sizeof(T)); 
            Free(*m_Alloc, m_Data);
        }

        m_Data = newData;
        m_Capacity = newCapacity;
    }

    template <TriCo T>
    T* Vector<T>::Insert(const T* it, const T& v)     
    { 
        const size_t off = it - m_Data;
        HAX_ASSERT(off >= 0 && off <= m_Size);

        EnsureCapacity(m_Size + 1);
        if (off < m_Size)
            memmove(m_Data + off + 1, m_Data + off, (m_Size - off) * sizeof(T));

        memcpy(&m_Data[off], &v, sizeof(v));
        m_Size++;
        return m_Data + off;
    }

    template <TriCo T>
    T* Vector<T>::Insert(size_t off, const T& v) 
    { 
        HAX_ASSERT(off <= m_Size);

        EnsureCapacity(m_Size + 1);
        if (off < m_Size)
            memmove(m_Data + off + 1, m_Data + off, (m_Size - off) * sizeof(T));

        memcpy(&m_Data[off], &v, sizeof(v));
        m_Size++;
        return m_Data + off;
    }

    template <TriCo T>
    T* Vector<T>::Erase(const T* it)                  
    { 
        const size_t off = it - m_Data;
        HAX_ASSERT(off < m_Size); 

        if (off == 0)
            return m_Data;

        memmove(m_Data + off, m_Data + off + 1, (m_Size - off - 1) * sizeof(T));
        m_Size--;
        return m_Data + off;
    }

    template <TriCo T>
    T* Vector<T>::Erase(size_t off)                  
    {
        HAX_ASSERT(off < m_Size); 

        if (off < m_Size - 1)
            memmove(m_Data + off, m_Data + off + 1, (m_Size - off - 1) * sizeof(T));

        m_Size--;

        return m_Data + off;
    }

    template <TriCo T>
    T* Vector<T>::Erase(const T* it, const T* itLast)
    { 
        const size_t off = it - m_Data;
        HAX_ASSERT(off >= 0 && off < m_Size && itLast >= it && itLast <= m_Data + m_Size); 

        const size_t count = itLast - it;
        memmove(m_Data + off, m_Data + off + count, (m_Size - off - count) * sizeof(T));
        m_Size -= count;
        return m_Data + off;
    }

    template<CharT T, size_t MaxLength>
    void BasicStringBuilder<T, MaxLength>::AppendF(const T* fmt, ...)
    {
        if (m_Length < MaxLength)
        {
            va_list args;
            va_start(args, fmt);

            int written = this->VPrintf(m_Data + m_Length, MaxLength - m_Length + 1, fmt, args);
            HAX_ASSERT(written >= 0);

            va_end(args);

            if (written > 0)
                m_Length = Min(m_Length + (size_t)written, MaxLength);
        }
    }

    template<CharT T, size_t MaxLength>
    void BasicStringBuilder<T, MaxLength>::Append(StringViewImpl<T> sv)
    {
        size_t spaceLeft = MaxLength - m_Length;
        if (spaceLeft > 0 && sv.Size() > 0)
        {
            size_t written = Min(sv.Size(), spaceLeft);
            memcpy(m_Data + m_Length, sv.Data(), written * sizeof(T));
            m_Length += written;
            m_Data[m_Length] = 0;
        }
    }

    template <TriCo K, TriCo V>
    const V* Map<K, V>::Get(const K& key) const
    {
        size_t idx = FindLowerBound(key);
        const size_t size = m_Entries.Size();

        if (idx < size)
        {
            const auto* data = m_Entries.Data();
            if (data[idx].key == key)
                return &data[idx].value;
        }
        return nullptr;
    }

    template <TriCo K, TriCo V>
    V& Map<K, V>::FindOrAdd(const K& key)
    {
        size_t idx = FindLowerBound(key);
        Entry* data = m_Entries.Data();

        if (idx < m_Entries.Size() && data[idx].key == key)
            return data[idx].value;

        m_Entries.Insert(idx, {key, V{}});
        return m_Entries.Data()[idx].value;
    }

    template <TriCo K, TriCo V>
    bool Map<K, V>::Erase(const K& key)
    {
        size_t idx = FindLowerBound(key);
        if (idx < m_Entries.Size() && m_Entries.Data()[idx].key == key) 
        {
            m_Entries.Erase(idx);
            return true;
        }
        return false;
    }

    template <TriCo K, TriCo V>
    size_t Map<K, V>::FindLowerBound(const K& key) const
    {
        const Entry* data = m_Entries.Data();
        size_t count = m_Entries.Size();

        if (count < 32)
        {
            size_t idx = 0;
            while (idx < count && data[idx].key < key)
                idx++;
            return idx;
        }

        size_t first = 0;
        while (count > 0) 
        {
            size_t step = count / 2;
            size_t mid = first + step;

            if (data[mid].key < key) 
            {
                first = mid + 1;
                count -= step + 1;
            } 
            else 
            {
                count = step;
            }
        }
        return first;
    }

    template <typename Container, typename EntryType>
    void HashMapIterator<Container, EntryType>::SkipInvalid() 
    { 
        while (m_Ptr < m_End && (uint8)m_Ptr->State != 1)
            m_Ptr++;
    }

    template <TriCo K, TriCo V>
    const V* HashMap<K,V>::Get(const K& key) const
    {
        const Entry* entry = GetEntry(key);
        return entry == nullptr ? nullptr : &entry->Value;
    }

    template <TriCo K, TriCo V>
    V& HashMap<K,V>::FindOrAdd(const K& key) 
    {
        if (m_Count * 10 >= m_Entries.Size() * 7)
        {
            Grow();
        }

        const size_t cap = m_Entries.Size();
        const size_t mask = cap - 1;
        const size_t hash = Hash(key);
        size_t idx = hash & mask;
        size_t firstDeleted = size_t(-1);

        while (m_Entries[idx].State != SlotState::Empty)
        {
            if (m_Entries[idx].State == SlotState::Occupied)
            {
                if (m_Entries[idx].Key == key) 
                { 
                    return m_Entries[idx].Value; 
                }
            }
            else if (firstDeleted == size_t(-1))
            {
                firstDeleted = idx;
            }

            idx = (idx + 1) & mask;
        }

        const size_t targetIdx = (firstDeleted != size_t(-1)) ? firstDeleted : idx;
        auto& entry = m_Entries[targetIdx];

        entry.Key = key;
        entry.State = SlotState::Occupied;
        entry.Value = V{};
        m_Count++;

        return entry.Value;
    }

    template <TriCo K, TriCo V>
    bool HashMap<K, V>::Erase(const K& key)
    {
        V* val = Get(key);
        if (val)
        {
            Entry* entry = (Entry*)((uint8_t*)(val) - offsetof(Entry, Value));
            entry->State = SlotState::Deleted;
            m_Count--;
            return true;
        }
        return false;
    }

    template <TriCo K, TriCo V>
    const typename HashMap<K,V>::Entry* HashMap<K,V>::GetEntry(const K& key) const
    {
        const size_t cap = m_Entries.Size();
        if (cap == 0) return nullptr;

        const size_t mask = cap - 1;
        size_t idx = Hash(key) & mask;

        // В линейном пробировании Empty — это сигнал остановки.
        // Если таблица не забита на 100%, мы всегда упремся в Empty либо найдем ключ.
        while (m_Entries[idx].State != SlotState::Empty)
        {
            if (m_Entries[idx].State == SlotState::Occupied && m_Entries[idx].Key == key)
            {
                return &m_Entries[idx];
            }
            idx = (idx + 1) & mask;
        }

        return nullptr;
    }

    inline size_t NextPowerOfTwo(size_t n) 
    {
        if (n <= 16) return 16;
        return std::bit_ceil(n); // Округляет вверх до ближайшей степени 2
    }

    template <TriCo K, TriCo V>
    void HashMap<K, V>::Grow(size_t newCap) 
    {
        size_t oldCap = m_Entries.Size();

        // Гарантируем степень двойки
        newCap = NextPowerOfTwo(newCap > 0 ? newCap : oldCap * 2);
        size_t mask = newCap - 1; // Например: 16 - 1 = 15 (в битах: 00001111)

        Vector<Entry> oldEntries = std::move(m_Entries); 
        m_Entries.Resize(newCap);
        m_Count = 0;

        // Инициализация новых слотов
        for (size_t i = 0; i < newCap; ++i)
            m_Entries[i].State = SlotState::Empty;

        // Рехеширование
        for (size_t i = 0; i < oldCap; ++i) 
        {
            if (oldEntries[i].State == SlotState::Occupied) 
            {
                size_t hash = Hash(oldEntries[i].Key);
                size_t idx = hash & mask; // Мгновенная замена %

                while (m_Entries[idx].State != SlotState::Empty) 
                    idx = (idx + 1) & mask; // Мгновенная замена %

                m_Entries[idx] = std::move(oldEntries[i]);
                m_Count++;
            }
        }
    }

    inline String& String::operator=(const wchar_t* str)
    {
        if (str == CStr())
            return *this;

        ClearFree();

        if (str && str[0] != '\0')
        {
            size_t len = 0;
            while (str[len]) len++;
            m_Data.Resize(len + 1);
            memcpy(m_Data.Data(), str, (len + 1) * sizeof(wchar_t));
        }

        return *this;
    }

    inline void String::Append(const wchar_t* str) 
    {
        if (str && str[0] != '\0')
        {
            size_t addLen = 0;
            while (str[addLen]) addLen++;

            size_t oldLen = Length();
            m_Data.Resize(oldLen + addLen + 1);
            memcpy(m_Data.Data() + oldLen, str, (addLen + 1) * sizeof(wchar_t));
        }
    }

    inline bool String::Equals(const wchar_t* str) const 
    {
        if (!str || str[0] == '\0') 
            return Empty();

        const wchar_t* p1 = CStr();
        while (*p1 && (*p1 == *str))
        {
            p1++; str++;
        }

        return *p1 == *str;
    }

    //=============================================================================
    // [SECTION] INTERNAL IMPLEMENTATION
    //=============================================================================
    // ...
}