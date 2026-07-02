#pragma once

#include "hax_unity_core.h"

namespace Unity
{
    struct NavMeshHit
    {
        static System::Type             typeof();

        Vector3                         m_Position;
        Vector3                         m_Normal;
        float                           m_Distance;
        int                             m_Mask;
        int                             m_Hit;
    };

    struct NavMesh
    {
        static System::Type             typeof();

        static bool                     SamplePosition(const Vector3& pos, NavMeshHit* hit, float maxDist, int areaMask);
    };
}