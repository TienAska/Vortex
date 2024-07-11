#include "pch.h"
#include "Device.h"

winrt::com_ptr<IDXGIFactory6> Vortex::Device::s_dxgiFactory;
std::vector<Vortex::Device> Vortex::Device::s_deviceList;

void Vortex::Device::Initialize()
{
    // The device only can be initialized once!
    WINRT_ASSERT(s_dxgiFactory == nullptr);

    uint32_t dxgiFactoryFlags = 0;
#if defined(_DEBUG)
    {
        winrt::com_ptr<ID3D12Debug1> debugController;
        winrt::check_hresult(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
        debugController->EnableDebugLayer();
        //debugController->SetEnableGPUBasedValidation(true);
        dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
    }
#endif
    winrt::check_hresult(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&s_dxgiFactory)));

	winrt::com_ptr<IDXGIAdapter3> hardwareAdapter;
    for (uint32_t i = 0; SUCCEEDED(s_dxgiFactory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&hardwareAdapter))); ++i)
    {
        s_deviceList.push_back(hardwareAdapter);
    }
}

Vortex::Device::Device(const winrt::com_ptr<IDXGIAdapter3>& adaptor)
{
    winrt::check_hresult(adaptor->GetDesc2(&m_adapterDesc));
    D3D_FEATURE_LEVEL level = D3D_FEATURE_LEVEL_12_2;
    if (m_adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE) // Windows Advanced Rasterization Platform (WARP)
        level = D3D_FEATURE_LEVEL_12_1;
    winrt::check_hresult(D3D12CreateDevice(adaptor.get(), level, IID_PPV_ARGS(&m_d3d12Device)));
}

winrt::com_ptr<ID3D12CommandQueue> Vortex::Device::CreateCommandQueue() const
{
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

    winrt::com_ptr<ID3D12CommandQueue> commandQueue;
    winrt::check_hresult(m_d3d12Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue)));
    return commandQueue;
}

winrt::com_ptr<IDXGISwapChain3> Vortex::Device::CreateSwapChain(const winrt::com_ptr<ID3D12CommandQueue>& commandQueue, HWND hWnd, uint32_t width, uint32_t height, winrt::com_ptr<ID3D12Resource> renderTargets[VX_DOUBLE_BUFFER])
{
    // This sample does not support full screen transitions.
    winrt::check_hresult(s_dxgiFactory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER));

    // Create a swap chain.
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.Width = width;
    swapChainDesc.Height = height;
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.Stereo = FALSE;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = VX_DOUBLE_BUFFER;
    swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
    winrt::com_ptr<IDXGISwapChain1> swapChain;
    winrt::check_hresult(s_dxgiFactory->CreateSwapChainForHwnd(commandQueue.get(), hWnd, &swapChainDesc, nullptr, nullptr, swapChain.put()));

    // Create a RTV heap.
    D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
    heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    heapDesc.NumDescriptors = VX_DOUBLE_BUFFER;
    heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    heapDesc.NodeMask = 0;
    winrt::check_hresult(m_d3d12Device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_rtvHeap)));

    // Get render targets from swap chain and create the RTVs.
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());
    uint32_t rtvDescriptorSize = m_d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    for (uint32_t i = 0; i < VX_DOUBLE_BUFFER; ++i)
    {
        winrt::check_hresult(swapChain->GetBuffer(i, IID_PPV_ARGS(&renderTargets[i])));
        m_d3d12Device->CreateRenderTargetView(renderTargets[i].get(), nullptr, rtvHandle);
        rtvHandle.Offset(1, rtvDescriptorSize);
    }

    return swapChain.as<IDXGISwapChain3>();
}
