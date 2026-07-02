
#include <d3d11.h>
#include <dxgi.h>
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

#include <atomic>
#include <thread>

#include <gui/backends/hax_gui_dx11.h>

static std::atomic<bool>        g_Running = true;
static ID3D11Device*            g_Device;
static ID3D11DeviceContext*     g_DeviceContext;
static IDXGISwapChain*          g_SwapChain;
static bool                     g_SwapChainOccluded;
static UINT                     g_ResizeWidth, g_ResizeHeight;
static ID3D11RenderTargetView*  g_RenderTargetView;

static void                     RenderThread(HWND hwnd);
static LRESULT WINAPI           WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
static bool                     CreateDeviceD3D(HWND hWnd);
static void                     CleanupDeviceD3D();
static void                     CreateRenderTarget();
static void                     CleanupRenderTarget();

int main()
{
    ::SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    WNDCLASSEXW wc =
    {
        .cbSize = sizeof(wc),
        .style = CS_CLASSDC,
        .lpfnWndProc = WndProc,
        .cbClsExtra = 0,
        .cbWndExtra = 0,
        .hInstance = ::GetModuleHandle(nullptr), 
        .hIcon = nullptr, 
        .hCursor = ::LoadCursor(nullptr, IDC_ARROW), 
        .hbrBackground = nullptr, 
        .lpszMenuName = nullptr, 
        .lpszClassName = L"ImGui Example",
        .hIconSm = nullptr
    };

    ::RegisterClassExW(&wc);
    HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"Example", WS_OVERLAPPEDWINDOW | WS_MAXIMIZE | WS_VISIBLE, 100, 100, 1600, 900, nullptr, nullptr, wc.hInstance, nullptr);

    ::ShowWindow(hwnd, SW_MAXIMIZE);
    ::UpdateWindow(hwnd);

    std::thread renderThread(RenderThread, hwnd);

    while (g_Running)
    {
        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                g_Running = false;
        }
    }

    renderThread.join();

    ::DestroyWindow(hwnd);
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);

    return 0;
}

static void RenderThread(HWND hwnd)
{
    CreateDeviceD3D(hwnd);

    Hax::Gui::Initialize((Hax::Handle)hwnd, g_Device);

    while (g_Running)
    {
        if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
        {
            CleanupRenderTarget();
            g_SwapChain->ResizeBuffers(0, g_ResizeWidth, g_ResizeHeight, DXGI_FORMAT_UNKNOWN, 0);
            g_ResizeWidth = g_ResizeHeight = 0;
            CreateRenderTarget();
        }

        Hax::Gui::LinearColor bg = 0xEBEBEBFF;
        g_DeviceContext->OMSetRenderTargets(1, &g_RenderTargetView, nullptr);
        g_DeviceContext->ClearRenderTargetView(g_RenderTargetView, &bg.R);

        Hax::Gui::BeginFrame();
        Hax::Gui::ShowDemoWindow();
        Hax::Gui::EndFrame();

        g_SwapChain->Present(1, 0);
    }

    Hax::Gui::Shutdown();
    CleanupDeviceD3D();
}

static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (Hax::Gui::HandleWndMsg((void*)hWnd, msg, wParam, lParam))
        return 1;

    switch (msg)
    {
        case WM_SIZE:
            if (wParam == SIZE_MINIMIZED)
                return 0;
            g_ResizeWidth = (UINT)LOWORD(lParam);
            g_ResizeHeight = (UINT)HIWORD(lParam);
            return 0;
        case WM_SYSCOMMAND:
            if ((wParam & 0xfff0) == SC_KEYMENU)
                return 0;
            break;
        case WM_DESTROY:
            ::PostQuitMessage(0);
            return 0;
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}

static bool CreateDeviceD3D(HWND hWnd)
{
    DXGI_SWAP_CHAIN_DESC sd =
    {
        .BufferDesc = 
        { 
            .Width = 0,
            .Height = 0,
            .RefreshRate = {.Numerator = 60, .Denominator = 1},
            .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
        },
        .SampleDesc = {.Count = 1, .Quality = 0},
        .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
        .BufferCount = 2,
        .OutputWindow = hWnd,
        .Windowed = TRUE,
        .SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
        .Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
    };

    UINT createDeviceFlags = 0;
    #ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    #endif

    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = {D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0,};
    HRESULT res = ::D3D11CreateDeviceAndSwapChain
    (
        nullptr, 
        D3D_DRIVER_TYPE_HARDWARE, 
        nullptr, 
        createDeviceFlags, 
        featureLevelArray, 
        2, 
        D3D11_SDK_VERSION, 
        &sd, 
        &g_SwapChain, 
        &g_Device, 
        &featureLevel, 
        &g_DeviceContext
    );
    if (res == DXGI_ERROR_UNSUPPORTED)
    {
        res = ::D3D11CreateDeviceAndSwapChain
        (
            nullptr,
            D3D_DRIVER_TYPE_WARP,
            nullptr,
            createDeviceFlags,
            featureLevelArray,
            2,
            D3D11_SDK_VERSION,
            &sd,
            &g_SwapChain,
            &g_Device,
            &featureLevel,
            &g_DeviceContext
        );
    }
    if (res != S_OK)
        return false;

    CreateRenderTarget();
    return true;
}

static void CleanupDeviceD3D()
{
    CleanupRenderTarget();

    ID3D11RenderTargetView* nullViews [] = {nullptr}; 
    g_DeviceContext->OMSetRenderTargets(_countof(nullViews), nullViews, nullptr); 

    if (g_SwapChain) 
    { 
        g_SwapChain->Release(); 
        g_SwapChain = nullptr; 
    }

    if (g_DeviceContext)
    {
        g_DeviceContext->ClearState();
        g_DeviceContext->Flush();
        g_DeviceContext->Release(); 
        g_DeviceContext = nullptr; 
    }

    if (g_Device) 
    {
        #ifdef _DEBUG
        ID3D11Debug* d3dDebug = nullptr;
        if (SUCCEEDED(g_Device->QueryInterface(__uuidof(ID3D11Debug), (void**)&d3dDebug)))
        {
            g_Device->Release();
            g_Device = nullptr;

            d3dDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
            d3dDebug->Release();
        }
        #endif
    }
}

static void CreateRenderTarget()
{
    ID3D11Texture2D* backBuffer;
    g_SwapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
    g_Device->CreateRenderTargetView(backBuffer, nullptr, &g_RenderTargetView);
    backBuffer->Release();
}

static void CleanupRenderTarget()
{
    if (g_RenderTargetView)
    {
        g_RenderTargetView->Release();
        g_RenderTargetView = nullptr;
    }
}
