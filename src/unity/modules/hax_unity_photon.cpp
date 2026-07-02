#include "hax_unity_photon.h"

namespace Unity::Photon
{
    System::Type PhotonView::typeof()
    {
        static System::Type s_Type = System::Type::GetType("PhotonUnityNetworking", "Photon.Pun", "PhotonView", true);
        return s_Type;
    }

    Player PhotonView::GetOwner()
    {
        THROW_IF_NULL();
        static auto s_Method = PhotonView::typeof().GetMethod("get_Owner", nullptr, true).Wrap();
        return s_Method.Call<Player>(*this);
    }

    void PhotonView::RPC(System::String methodName, RpcTarget target, System::Array<System::Object> parameters)
    {
        THROW_IF_NULL();
        static auto s_Method = PhotonView::typeof().GetMethod("RPC", "System.Void(System.String,Photon.Pun.RpcTarget,System.Object[])", true).Wrap();
        return s_Method.Call<void>(*this, methodName, target, parameters);
    }

    System::Type PhotonNetwork::typeof()
    {
        static System::Type s_Type = System::Type::GetType("PhotonUnityNetworking", "Photon.Pun", "PhotonNetwork", true);
        return s_Type;
    }

    bool& PhotonView::IsMine()
    {
        THROW_IF_NULL();
        static uint32_t s_Offset = PhotonView::typeof().GetField("<IsMine>k__BackingField", true).GetFieldOffset();
        return *(bool*)((char*)GetPtr() + s_Offset);
    }

    Unity::GameObject PhotonNetwork::InstantiateRoomObject(System::String name, const Unity::Vector3& position, const Unity::Quaternion& rotation, char8_t group, void* data)
    {
        static auto s_Method = typeof().GetMethod("InstantiateRoomObject", nullptr, true).Wrap();

        if constexpr (HAX_UNITY_IS_MONO)
            return s_Method.Call<Unity::GameObject>(name, System::Box(position), System::Box(rotation), group, data);
        else
            return s_Method.Call<Unity::GameObject>(name, position, rotation, group, data);
    }

    Player PhotonNetwork::GetMasterClient()
    {
        static auto s_Method = typeof().GetMethod("get_MasterClient", nullptr, true).Wrap();
        return s_Method.Call<Player>();
    }
}