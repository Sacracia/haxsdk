#pragma once

#include "hax_unity_core.h"

namespace Unity::Photon
{
    enum RpcTarget
    {
        All,
        Others,
        MasterClient,
        AllBuffered,
        OthersBuffered,
        AllViaServer,
        AllBufferedViaServer
    };

    struct Player : System::Object
    {
        using System::Object::Object;
    };

    struct PhotonView : Unity::MonoBehaviour
    {
        using Unity::MonoBehaviour::MonoBehaviour;

        static System::Type             typeof();

        bool&                           IsMine();
        Player                          GetOwner();

        void                            RPC(System::String methodName, RpcTarget target, System::Array<System::Object> parameters);
    };

    struct PhotonNetwork
    {
        static System::Type             typeof();

        static Player                   GetMasterClient();
        static Unity::GameObject        InstantiateRoomObject(System::String name,
                                                              const Unity::Vector3& position,
                                                              const Unity::Quaternion& rotation,
                                                              char8_t group = 0,
                                                              void* data = nullptr);
    };
}