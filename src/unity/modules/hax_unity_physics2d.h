#pragma once

#include "hax_unity_core.h"

namespace Unity
{
    class Collider2D : public Behaviour
    {
    public:
        using Behaviour::Behaviour;

        static System::Type             typeof();

        Bounds                          GetBounds();
        void                            SetIsTrigger(bool value);
    };

    class BoxCollider2D : public Collider2D
    {
    public:
        using Collider2D::Collider2D;

        static System::Type             typeof();
    };

    class PolygonCollider2D : public Collider2D
    {
    public:
        using Collider2D::Collider2D;

        static System::Type             typeof();

        System::Array<Vector2>          GetPoints();
        void                            SetPoints(System::Array<Vector2> value);
    };

    class Rigidbody2D : public Component
    {
    public:
        using Component::Component;

        static System::Type             typeof();
    };
}