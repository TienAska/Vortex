#include "pch.h"
#include "Device.h"

winrt::com_ptr<IDXGIFactory6> Vortex::Device::s_dxgiFactory;
std::vector<Vortex::Device> Vortex::Device::s_deviceList;

void Vortex::Device::Initialize()
{
    // The device only can be initialized once!
    WINRT_ASSERT(s_dxgiFactory == nullptr);

    uint32_t dxgiFactoryFlags = 0;
//#if defined(_DEBUG)
//    {
//        winrt::com_ptr<ID3D12Debug1> debugController;
//        winrt::check_hresult(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
//        debugController->EnableDebugLayer();
//        //debugController->SetEnableGPUBasedValidation(true);
//        dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
//    }
//#endif
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

winrt::com_ptr<ID3D12Fence1> Vortex::Device::CreateFence(uint64_t value) const
{
    winrt::com_ptr<ID3D12Fence1> fence;
    winrt::check_hresult(m_d3d12Device->CreateFence(value, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));
    return fence;
}

winrt::com_ptr<ID3D12CommandQueue> Vortex::Device::CreateGraphicsCommandQueue() const
{
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    queueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.NodeMask = 0;

    winrt::com_ptr<ID3D12CommandQueue> commandQueue;
    winrt::check_hresult(m_d3d12Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue)));
    return commandQueue;
}

winrt::com_ptr<ID3D12CommandQueue> Vortex::Device::CreateComputeCommandQueue() const
{
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
    queueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.NodeMask = 0;

    winrt::com_ptr<ID3D12CommandQueue> commandQueue;
    winrt::check_hresult(m_d3d12Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue)));
    return commandQueue;
}

winrt::com_ptr<ID3D12CommandQueue> Vortex::Device::CreateCopyCommandQueue() const
{
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
    queueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.NodeMask = 0;

    winrt::com_ptr<ID3D12CommandQueue> commandQueue;
    winrt::check_hresult(m_d3d12Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue)));
    return commandQueue;
}

winrt::com_ptr<ID3D12CommandAllocator> Vortex::Device::CreateGraphicsCommandAllocator() const
{
    winrt::com_ptr<ID3D12CommandAllocator> commandAllocator;
    winrt::check_hresult(m_d3d12Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator)));
    return commandAllocator;
}

winrt::com_ptr<ID3D12CommandAllocator> Vortex::Device::CreateComputeCommandAllocator() const
{
    winrt::com_ptr<ID3D12CommandAllocator> commandAllocator;
    winrt::check_hresult(m_d3d12Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COMPUTE, IID_PPV_ARGS(&commandAllocator)));
    return commandAllocator;
}

winrt::com_ptr<ID3D12CommandAllocator> Vortex::Device::CreateBundleCommandAllocator() const
{
    winrt::com_ptr<ID3D12CommandAllocator> commandAllocator;
    winrt::check_hresult(m_d3d12Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_BUNDLE, IID_PPV_ARGS(&commandAllocator)));
    return commandAllocator;
}

winrt::com_ptr<ID3D12CommandAllocator> Vortex::Device::CreateCopyCommandAllocator() const
{
    winrt::com_ptr<ID3D12CommandAllocator> commandAllocator;
    winrt::check_hresult(m_d3d12Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COPY, IID_PPV_ARGS(&commandAllocator)));
    return commandAllocator;
}

winrt::com_ptr<ID3D12GraphicsCommandList6> Vortex::Device::CreateGraphicsCommandList() const
{
    winrt::com_ptr<ID3D12GraphicsCommandList6> commandList;
    winrt::check_hresult(m_d3d12Device->CreateCommandList1(0, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(&commandList)));
    return commandList;
}

winrt::com_ptr<ID3D12GraphicsCommandList6> Vortex::Device::CreateComputeCommandList() const
{
    winrt::com_ptr<ID3D12GraphicsCommandList6> commandList;
    winrt::check_hresult(m_d3d12Device->CreateCommandList1(0, D3D12_COMMAND_LIST_TYPE_COMPUTE, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(&commandList)));
    return commandList;
}

winrt::com_ptr<ID3D12GraphicsCommandList6> Vortex::Device::CreateBundleCommandList() const
{
    winrt::com_ptr<ID3D12GraphicsCommandList6> commandList;
    winrt::check_hresult(m_d3d12Device->CreateCommandList1(0, D3D12_COMMAND_LIST_TYPE_BUNDLE, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(&commandList)));
    return commandList;
}

winrt::com_ptr<ID3D12GraphicsCommandList6> Vortex::Device::CreateCopyCommandList() const
{
    winrt::com_ptr<ID3D12GraphicsCommandList6> commandList;
    winrt::check_hresult(m_d3d12Device->CreateCommandList1(0, D3D12_COMMAND_LIST_TYPE_COPY, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(&commandList)));
    return commandList;
}

winrt::com_ptr<IDXGISwapChain3> Vortex::Device::CreateSwapChain(
    HWND hWnd, uint32_t width, uint32_t height,
    const winrt::com_ptr<ID3D12CommandQueue>& commandQueue,
    winrt::com_ptr<ID3D12Resource> renderTargets[VX_DOUBLE_BUFFER],
    winrt::com_ptr<ID3D12DescriptorHeap>& rtvHeap, uint32_t& descriptorSize) const
{
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
    swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
    winrt::com_ptr<IDXGISwapChain1> swapChain;
    winrt::check_hresult(s_dxgiFactory->CreateSwapChainForHwnd(commandQueue.get(), hWnd, &swapChainDesc, nullptr, nullptr, swapChain.put()));

    // This sample does not support full screen transitions.
    winrt::check_hresult(s_dxgiFactory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER));

    // Create a RTV heap.
    D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
    heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    heapDesc.NumDescriptors = VX_DOUBLE_BUFFER;
    heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    heapDesc.NodeMask = 0;
    winrt::check_hresult(m_d3d12Device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&rtvHeap)));

    // Get render targets from swap chain and create the RTVs.
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart());
    descriptorSize = m_d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    for (uint32_t i = 0; i < VX_DOUBLE_BUFFER; ++i)
    {
        winrt::check_hresult(swapChain->GetBuffer(i, IID_PPV_ARGS(&renderTargets[i])));
        m_d3d12Device->CreateRenderTargetView(renderTargets[i].get(), nullptr, rtvHandle);
        rtvHandle.Offset(1, descriptorSize);
    }

    return swapChain.as<IDXGISwapChain3>();
}

winrt::com_ptr<ID3D12RootSignature> Vortex::Device::CreateRootSignature(CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC versionedRootSignatureDesc) const
{
    winrt::com_ptr<ID3DBlob> signature;
    winrt::com_ptr<ID3DBlob> error;
    winrt::check_hresult(D3D12SerializeVersionedRootSignature(&versionedRootSignatureDesc, signature.put(), error.put()));

    winrt::com_ptr<ID3D12RootSignature> rootSignature;
    winrt::check_hresult(m_d3d12Device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&rootSignature)));

    return rootSignature;
}

winrt::com_ptr<ID3D12PipelineState> Vortex::Device::CreateMeshPSO(
    const winrt::com_ptr<ID3D12RootSignature>& rootSignature,
    D3D12_SHADER_BYTECODE mesh, D3D12_SHADER_BYTECODE pixel,
    D3D12_SHADER_BYTECODE amplification/* = { NULL, 0 }*/) const
{
    D3DX12_MESH_SHADER_PIPELINE_STATE_DESC psoDesc = {};
    psoDesc.pRootSignature = rootSignature.get();
    psoDesc.AS = amplification;
    psoDesc.MS = mesh;
    psoDesc.PS = pixel;
    psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);         // Opaque
    psoDesc.SampleMask = DefaultSampleMask();
    psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);    // CW front; cull back
    //psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT); // Less-equal depth test w/ writes; no stencil
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    //psoDesc.DSVFormat;
    psoDesc.SampleDesc = DefaultSampleDesc();
    psoDesc.NodeMask = 0;
    psoDesc.CachedPSO = { NULL, 0 };
    psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

    CD3DX12_PIPELINE_MESH_STATE_STREAM psoStream(psoDesc);

    D3D12_PIPELINE_STATE_STREAM_DESC streamDesc = {};
    streamDesc.pPipelineStateSubobjectStream = &psoStream;
    streamDesc.SizeInBytes = sizeof(psoStream);

    winrt::com_ptr<ID3D12PipelineState> pipelineState;
    winrt::check_hresult(m_d3d12Device->CreatePipelineState(&streamDesc, IID_PPV_ARGS(&pipelineState)));

    return pipelineState;
}
