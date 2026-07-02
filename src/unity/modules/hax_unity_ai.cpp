#include "hax_unity_ai.h"

namespace Unity
{
    System::Type NavMeshHit::typeof()
    {
        static System::Type s_Type = System::Type::GetType("UnityEngine.AIModule", "UnityEngine.AI", "NavMeshHit", true);
        return s_Type;
    }

    System::Type NavMesh::typeof()
    {
        static System::Type s_Type = System::Type::GetType("UnityEngine.AIModule", "UnityEngine.AI", "NavMesh", true);
        return s_Type;
    }

    bool NavMesh::SamplePosition(const Vector3& pos, NavMeshHit* hit, float maxDist, int areaMask)
    {
        static auto s_Method = NavMesh::typeof().GetMethod("SamplePosition_Injected", nullptr, true).Wrap();
        return s_Method.InternalCall<bool>(&pos, hit, maxDist, areaMask);
    }
}