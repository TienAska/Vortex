#include "pch.h"
#include "Renderer.h"

Vortex::SwapChain::SwapChain(const winrt::com_ptr<ID3D12CommandQueue>& commandQueue, HWND hwnd, uint32_t width, uint32_t height)
{
    m_swapChain = VX_DEVICE0->CreateSwapChain(hwnd, width, height, commandQueue, m_renderTargets, m_rtvDescriptorHeap, m_rtvDescriptorSize);
}

winrt::com_ptr<ID3D12Resource> Vortex::SwapChain::GetBackBufferRenderTarget() const
{
	return m_renderTargets[m_swapChain->GetCurrentBackBufferIndex()];
}

CD3DX12_CPU_DESCRIPTOR_HANDLE Vortex::SwapChain::GetBackBufferRTVHandle() const
{
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), m_swapChain->GetCurrentBackBufferIndex(), m_rtvDescriptorSize);
}

void Vortex::SwapChain::Flip()
{
	winrt::check_hresult(m_swapChain->Present(0, DXGI_PRESENT_ALLOW_TEARING));
}

Vortex::Renderer::Renderer(HWND hWnd, uint32_t width, uint32_t height) :
	m_viewport(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height)),
	m_scissorRect(0, 0, static_cast<LONG>(width), static_cast<LONG>(height)),
	m_fenceEvent(::CreateEvent(nullptr, FALSE, FALSE, nullptr)), m_fenceValue(0)
{
	winrt::check_bool(bool{ m_fenceEvent });

	if (!Device::IsInitialized())
		Device::Initialize();

	m_fence = VX_DEVICE0->CreateFence(m_fenceValue);
	
	m_commandQueue = VX_DEVICE0->CreateGraphicsCommandQueue();
	m_commandAllocator = VX_DEVICE0->CreateGraphicsCommandAllocator();
	m_commandList = VX_DEVICE0->CreateGraphicsCommandList();

	m_swapChain = std::make_unique<SwapChain>(m_commandQueue, hWnd, width, height);

	//winrt::check_hresult(GameInputCreate(m_gameInput.put()));
	//winrt::check_hresult(RegisterReadingCallback(m_gameMouse, GameInputKindMouse, 0, ));
}

void Vortex::Renderer::Execute()
{
	WaitForPreviousFrame();

	winrt::check_hresult(m_commandAllocator->Reset());
	winrt::check_hresult(m_commandList->Reset(m_commandAllocator.get(), nullptr));

	m_commandList->RSSetViewports(1, &m_viewport);
	m_commandList->RSSetScissorRects(1, &m_scissorRect);

	auto transitionBarrier = CD3DX12_RESOURCE_BARRIER::Transition(m_swapChain->GetBackBufferRenderTarget().get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	m_commandList->ResourceBarrier(1, &transitionBarrier);

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_swapChain->GetBackBufferRTVHandle();
	m_commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

	const float clearColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	m_commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

	for (const std::unique_ptr<IRenderPass>& pass : m_passes)
	{
        ID3D12DescriptorHeap* heaps[] = { pass->GetDescriptorHeap() };
        m_commandList->SetDescriptorHeaps(_countof(heaps), heaps);
		m_commandList->ExecuteBundle(pass->GetCommandList());
	}

	transitionBarrier = CD3DX12_RESOURCE_BARRIER::Transition(m_swapChain->GetBackBufferRenderTarget().get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	m_commandList->ResourceBarrier(1, &transitionBarrier);

	winrt::check_hresult(m_commandList->Close());

	// Execute the command list.
    ID3D12CommandList* ppCommandLists[] = { m_commandList.get() };
	m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

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

//
//void Vortex::Renderer::Update()
//{
//	// Get input.
//	IGameInputReading* reading;
//	if (SUCCEEDED(m_gameInput->GetCurrentReading(GameInputKindKeyboard | GameInputKindMouse, nullptr, &reading)))
//	{
//		if (!m_gameDevice) reading->GetDevice(m_gameDevice.put());
//
//		std::vector<GameInputKeyState> keyState(reading->GetKeyCount());
//		reading->GetKeyState(static_cast<uint32_t>(keyState.size()), keyState.data());
//
//		GameInputMouseState mouseState;
//		if (reading->GetMouseState(&mouseState))
//		{
//			static GameInputMouseState lastState;
//			static float sensitivity = 0.1f;
//			if (mouseState.buttons & GameInputMouseRightButton)
//			{
//				float yaw = (mouseState.positionX - lastState.positionX) * sensitivity;
//				float pitch = (mouseState.positionY - lastState.positionY) * sensitivity;
//				lastState = mouseState;
//				m_camera.Rotate(0.0f, yaw);
//				m_camera.Rotate(pitch, 0.0f);
//			}
//		}
//
//		if (!keyState.empty() && keyState.front().virtualKey != '\0')
//		{
//			UploadTexture();
//		}
//		reading->Release();
//
//		// Update matrix to gpu.
//		static int size = 1;
//		UINT8* data;
//		auto model = DirectX::SimpleMath::Matrix::Identity;
//		//world *= DirectX::SimpleMath::Matrix::CreateScale(size * 0.01f);
//		//world = m_camera.GetViewProjection().Transpose();
//		GlobalParameters globalParameters = { size, model.Transpose(), model.Transpose(), model.Transpose() };
//		CD3DX12_RANGE readRange(0, 0);
//		winrt::check_hresult(m_cbvResource->Map(0, &readRange, reinterpret_cast<void**>(&data)));
//		memcpy(data, &globalParameters, sizeof(GlobalParameters));
//		m_cbvResource->Unmap(0, nullptr);
//		size++;
//	}
//	else if (m_gameDevice)
//	{
//		m_gameDevice->Release();
//		m_gameDevice.detach();
//	}
//}