#pragma once

#include "hax_unity_core.h"

namespace Unity
{
    class Rigidbody : public Component
    {
    public:
        using Component::Component;

        static System::Type             typeof();

        void                            AddForce(const Vector3& force, ForceMode mode = ForceMode::Force);
        Vector3                         GetVelocity();
        void                            SetVelocity(const Vector3& value);
    };

    struct RaycastHit
    {
        static System::Type             typeof();

        Transform                       GetTransform();

        Vector3                         m_Point;
        Vector3                         m_Normal;
        uint32_t                        m_FaceID;
        float                           m_Distance;
        Vector2                         m_UV;
        int                             m_Collider;
    };

    class Physics
    {
    public:
        static System::Type              typeof();
        static System::Array<RaycastHit> SphereCastAll(const Vector3& origin, float radius, const Vector3& direction, float maxDistance, int layerMask);
    };

    class Collider : public Component
    {
    public:
        using Component::Component;

        static System::Type             typeof();

        bool                            GetEnabled();
        void                            SetEnabled(bool value);
        Bounds                          GetBounds();
    };

    class BoxCollider : public Collider
    {
    public:
        using Collider::Collider;

        static System::Type             typeof();

        Vector3                         GetCenter();
        Vector3                         GetSize();
    };
}