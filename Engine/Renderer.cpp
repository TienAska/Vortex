#include "pch.h"
#include "Renderer.h"

Vortex::SwapChain::SwapChain(const winrt::com_ptr<ID3D12CommandQueue>& commandQueue, HWND hwnd, uint32_t width, uint32_t height) :
    m_viewport(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height)),
    m_scissorRect(0, 0, static_cast<LONG>(width), static_cast<LONG>(height))
{
    m_swapChain = VX_DEVICE0->CreateSwapChain(hwnd, width, height, commandQueue, m_renderTargets, m_rtvDescriptorHeap, m_rtvDescriptorSize);
    m_transitionToPrenset[0] = CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[0].get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
    m_transitionToPrenset[1] = CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[1].get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
    m_transitionToRenderTarget[0] = CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[0].get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
    m_transitionToRenderTarget[1] = CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[1].get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
}

Vortex::Renderer::Renderer(HWND hWnd, uint32_t width, uint32_t height) :
    m_fenceEvent(::CreateEvent(nullptr, FALSE, FALSE, nullptr)), m_fenceValue(0),
    m_timeSinceStart(std::chrono::steady_clock::now()),
    m_camera(std::make_shared<Camera>()),
    m_globalParams(std::make_shared<GlobalParameters>())
{
    winrt::check_bool(bool{ m_fenceEvent });

    if (!Device::IsInitialized())
        Device::Initialize();

    m_fence = VX_DEVICE0->CreateFence(m_fenceValue);
    
    m_commandQueue = VX_DEVICE0->CreateGraphicsCommandQueue();
    m_commandAllocator = VX_DEVICE0->CreateGraphicsCommandAllocator();
    m_commandListBegin = VX_DEVICE0->CreateGraphicsCommandList();
    m_commandListEnd = VX_DEVICE0->CreateGraphicsCommandList();

    m_swapChain = std::make_shared<SwapChain>(m_commandQueue, hWnd, width, height);

    winrt::check_hresult(GameInputCreate(m_gameInput.put()));
    //winrt::check_hresult(RegisterReadingCallback(m_gameMouse, GameInputKindMouse, 0, ));
    
    Vortex::Device::CreateResourceHeap(VX_0, 4);
    m_constantResource = VX_DEVICE0->CreateConstantResource((sizeof(GlobalParameters) + 255) & ~255);
    m_cbvGpuHandle = VX_DEVICE0->CreateCBV(0, m_constantResource, (sizeof(GlobalParameters) + 255) & ~255);
}

void Vortex::Renderer::Execute()
{
    WaitForPreviousFrame();

    Update();

    winrt::check_hresult(m_commandAllocator->Reset());

    std::vector<ID3D12CommandList*> commandLists;
    // Begin frame.
    {
        winrt::check_hresult(m_commandListBegin->Reset(m_commandAllocator.get(), nullptr));
        m_commandListBegin->ResourceBarrier(1, m_swapChain->GetTransitionToRenderTarget());
        static const float clearColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
        m_commandListBegin->ClearRenderTargetView(m_swapChain->GetBackBufferRTVHandle(), clearColor, 0, nullptr);
        winrt::check_hresult(m_commandListBegin->Close());
    }
    commandLists.push_back(m_commandListBegin.get());
    
    // Passes frames.
    for (const std::unique_ptr<IRenderPass>& pass : m_passes)
    {
        commandLists.push_back(pass->GetCommandList(m_swapChain, *this));
    }

    // End frame.
    {
        winrt::check_hresult(m_commandListEnd->Reset(m_commandAllocator.get(), nullptr));
        m_commandListEnd->ResourceBarrier(1, m_swapChain->GetTransitionToPresent());
        winrt::check_hresult(m_commandListEnd->Close());
    }
    commandLists.push_back(m_commandListEnd.get());

    // Execute
    m_commandQueue->ExecuteCommandLists(static_cast<uint32_t>(commandLists.size()), commandLists.data());

    m_swapChain->Flip();
}

Vortex::Renderer::~Renderer()
{
    // Ensure that the GPU is no longer referencing resources that are about to be
    // cleaned up by the destructor.
    WaitForPreviousFrame();

    m_fenceEvent.close();
    //m_gameInput->Release();
    //m_gameInput.detach();
}

void Vortex::Renderer::WaitForPreviousFrame()
{
    // WAITING FOR THE FRAME TO COMPLETE BEFORE CONTINUING IS NOT BEST PRACTICE.
    // This is code implemented as such for simplicity. The D3D12HelloFrameBuffering
    // sample illustrates how to use fences for efficient resource usage and to
    // maximize GPU utilization.

    // Signal and increment the fence value.
    const uint64_t fence = m_fenceValue++;
    winrt::check_hresult(m_commandQueue->Signal(m_fence.get(), fence));

    // Wait until the previous frame is finished.
    if (m_fence->GetCompletedValue() < fence)
    {
        winrt::check_hresult(m_fence->SetEventOnCompletion(fence, m_fenceEvent.get()));
        ::WaitForSingleObject(m_fenceEvent.get(), INFINITE);
    }

    //m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
}
//
//void Vortex::Renderer::UploadTexture()
//{
//    D3D12_SUBRESOURCE_DATA subresourceData;
//    std::unique_ptr<uint8_t[]> decodedData;
//    winrt::check_hresult(DirectX::LoadWICTextureFromFile(m_device.get(), m_textureFilename.c_str(), m_srvResource.put(), decodedData, subresourceData));
//
//    winrt::com_ptr<ID3D12Resource> uploadResource;
//    const UINT64 uploadBufferSize = GetRequiredIntermediateSize(m_srvResource.get(), 0, 1);
//    D3D12_HEAP_PROPERTIES uploadHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
//    D3D12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);
//    winrt::check_hresult(m_device->CreateCommittedResource(
//        &uploadHeapProperties,
//        D3D12_HEAP_FLAG_NONE,
//        &bufferDesc,
//        D3D12_RESOURCE_STATE_GENERIC_READ,
//        nullptr,
//        IID_PPV_ARGS(&uploadResource)
//    ));
//
//    UpdateSubresources(m_commandList.get(), m_srvResource.get(), uploadResource.get(), 0, 0, 1, &subresourceData);
//}
//

//
//winrt::com_ptr<ID3D12DescriptorHeap> Vortex::Renderer::CreateSamplerHeap()
//{
//	winrt::com_ptr<ID3D12DescriptorHeap> samplerHeap;
//	D3D12_DESCRIPTOR_HEAP_DESC samplerHeapDesc = {};
//	samplerHeapDesc.NumDescriptors = 1;
//	samplerHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
//	samplerHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
//	winrt::check_hresult(m_device->CreateDescriptorHeap(&samplerHeapDesc, IID_PPV_ARGS(&samplerHeap)));
//
//	D3D12_SAMPLER_DESC samplerDesc = {};
//	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
//	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
//	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
//	m_device->CreateSampler(&samplerDesc, samplerHeap->GetCPUDescriptorHandleForHeapStart());
//
//	return samplerHeap;
//}


void Vortex::Renderer::Update()
{
    // Get input.
    winrt::com_ptr<IGameInputReading> reading;
    if (SUCCEEDED(m_gameInput->GetCurrentReading(GameInputKindKeyboard | GameInputKindMouse, nullptr, reading.put())))
    {
        if (!m_gameDevice) reading->GetDevice(m_gameDevice.put());

        m_globalParams->model = DirectX::XMMatrixTranspose(DirectX::XMMatrixTranslationFromVector(DirectX::XMVectorSet(0.0f, 0.0f, -1.0f, 1.0f)));

        std::vector<GameInputKeyState> keyState(reading->GetKeyCount());
        if (reading->GetKeyState(static_cast<uint32_t>(keyState.size()), keyState.data()))
        {
            //UploadTexture();
            if (keyState.front().virtualKey == ' ')
            {
                m_globalParams->model = DirectX::XMMatrixIdentity();
            }
            else if(keyState.front().virtualKey == 'A')
            {
                m_globalParams->model = DirectX::XMMatrixTranspose(DirectX::XMMatrixTranslationFromVector(DirectX::XMVectorSet(-1.0f, 0.0f, 0.0f, 1.0f)));
            }


        }

        static float sensitivity = 0.1f;
        GameInputMouseState mouseState;
        if (reading->GetMouseState(&mouseState))
        {
            static GameInputMouseState lastState;
            if (mouseState.buttons & GameInputMouseRightButton)
            {
                if (!(lastState.buttons & GameInputMouseRightButton))
                {
                    lastState = mouseState;
                }

                float yaw = (mouseState.positionX - lastState.positionX) * sensitivity;
                float pitch = (mouseState.positionY - lastState.positionY) * sensitivity;
                yaw = DirectX::XMConvertToRadians(yaw);
                pitch = DirectX::XMConvertToRadians(pitch);
                m_camera->Yaw(yaw);
                m_camera->Pitch(pitch);
                //m_camera->Rotate(yaw, pitch);
            }
            lastState = mouseState;
        }

        m_globalParams->view = DirectX::XMMatrixTranspose(m_camera->GetView());
        m_globalParams->projection = DirectX::XMMatrixTranspose(m_camera->GetProjection());
        m_globalParams->time = std::chrono::duration<float>(std::chrono::steady_clock::now() - m_timeSinceStart).count();
        uint8_t* gpuPtr = nullptr;
        CD3DX12_RANGE range(0, 0);
        winrt::check_hresult(m_constantResource->Map(0, &range, reinterpret_cast<void**>(&gpuPtr)));
        memcpy(gpuPtr, m_globalParams.get(), sizeof(GlobalParameters));
        m_constantResource->Unmap(0, nullptr);
    }
    else if (m_gameDevice)
    {
        m_gameDevice->Release();
        m_gameDevice.detach();
    }
}