#pragma once

#if __has_include("hax_gui.h")
#include "hax_gui.h"
#elif __has_include("../hax_gui.h")
#include "../hax_gui.h"
#else
#error "hax_gui.h not found"
#endif

struct ID3D11Device;

namespace Hax::Gui
{
    void Initialize(Handle hwnd, ID3D11Device* device);
}