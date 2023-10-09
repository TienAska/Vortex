#include "pch.h"

#include "Renderer.h"
#include "DeviceManager.h"
#include "Shader.h"


//extern "C" { __declspec(dllexport) extern const UINT D3D12SDKVersion = 610; }
//
//extern "C" { __declspec(dllexport) extern const char* D3D12SDKPath = u8".\\D3D12\\"; }

Vortex::Renderer::Renderer(HWND hwnd, UINT width, UINT height) : m_width(width), m_height(height)
{
	m_viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<float>(m_width), static_cast<float>(m_height));
	m_scissorRect = CD3DX12_RECT(0, 0, static_cast<LONG>(m_width), static_cast<LONG>(m_height));

	m_textureFilename = L"Assets/Textures/Fabric_DishCloth_D.tif";

	m_device = DeviceManager::GetDevice();

	// Describe and create the command queue.
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	winrt::check_hresult(m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue)));

	// Describe and create the swap chain.
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.Width = m_width;
	swapChainDesc.Height = m_height;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = FrameCount;
	swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;

	winrt::com_ptr<IDXGISwapChain1> swapChain;
	//winrt::check_hresult(factory->CreateSwapChainForComposition(
	//	m_commandQueue.get(),        // Swap chain needs the queue so that it can force a flush on it.
	//	&swapChainDesc,
	//	nullptr,
	//	swapChain.put()
	//));


	//winrt::check_hresult(DCompositionCreateDevice(nullptr, IID_PPV_ARGS(&dcompDevice)));
	//winrt::check_hresult(dcompDevice->CreateTargetForHwnd(hwnd, true, dcompTarget.put()));
	//winrt::check_hresult(dcompDevice->CreateVisual(dcompVisual.put()));
	//winrt::check_hresult(dcompVisual->SetContent(swapChain.get()));
	//winrt::check_hresult(dcompTarget->SetRoot(dcompVisual.get()));
	//winrt::check_hresult(dcompDevice->Commit());

	winrt::com_ptr<IDXGIFactory6> factory = DeviceManager::GetFactory();
	winrt::check_hresult(factory->CreateSwapChainForHwnd(
		m_commandQueue.get(),        // Swap chain needs the queue so that it can force a flush on it.
		hwnd,
		&swapChainDesc,
		nullptr,
		nullptr,
		swapChain.put()
	));


	// This sample does not support fullscreen transitions.
	//winrt::check_hresult(factory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER));

	m_swapChain = swapChain.as<IDXGISwapChain3>();
	m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

	// Create descriptor heaps.
	{
		// Describe and create a render target view (RTV) descriptor heap.
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
		rtvHeapDesc.NumDescriptors = FrameCount;
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		winrt::check_hresult(m_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap)));

		m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	// Create frame resources.
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());

		// Create a RTV for each frame.
		for (UINT n = 0; n < FrameCount; n++)
		{
			winrt::check_hresult(m_swapChain->GetBuffer(n, IID_PPV_ARGS(&m_renderTargets[n])));
			m_device->CreateRenderTargetView(m_renderTargets[n].get(), nullptr, rtvHandle);
			rtvHandle.Offset(1, m_rtvDescriptorSize);
		}
	}

	winrt::check_hresult(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator)));


	// Create root signature.
	CD3DX12_DESCRIPTOR_RANGE1 descRange[1];
	descRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);

	CD3DX12_ROOT_PARAMETER1 rootParameter[1];
	//rootParameter[0].InitAsDescriptorTable(1, descRange);
	rootParameter[0].InitAsConstantBufferView(0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC);


	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC versionedRootSignatureDesc;
	versionedRootSignatureDesc.Init_1_1(1, rootParameter, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	winrt::com_ptr<ID3DBlob> signature;
	winrt::com_ptr<ID3DBlob> error;

	winrt::check_hresult(D3D12SerializeVersionedRootSignature(&versionedRootSignatureDesc, signature.put(), error.put()));
	winrt::check_hresult(m_device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature)));

	// Compile shaders.
	Shader vertexShader(L"TriangleVS");
	Shader pixelShader(L"TrianglePS");


	// Define the vertex input layout.
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};


	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
	psoDesc.pRootSignature = m_rootSignature.get();
	psoDesc.VS = vertexShader.GetBytecode();
	psoDesc.PS = pixelShader.GetBytecode();
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = m_renderTargets[0]->GetDesc().Format;
	//psoDesc.DSVFormat = m_depthStencil->GetDesc().Format;
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);    // CW front; cull back
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);         // Opaque
	//psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT); // Less-equal depth test w/ writes; no stencil
	psoDesc.DepthStencilState.DepthEnable = FALSE;
	psoDesc.DepthStencilState.StencilEnable = FALSE;
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.SampleDesc = DefaultSampleDesc();
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	winrt::check_hresult(m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState)));

	// Create the command list.
	winrt::check_hresult(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.get(), m_pipelineState.get(), IID_PPV_ARGS(&m_commandList)));

	// Command lists are created in the recording state, but there is nothing
	// to record yet. The main loop expects it to be closed, so close it now.
	winrt::check_hresult(m_commandList->Close());

	// Create the vertex buffer.
	{
		// Define the geometry for a triangle.
		Vertex triangleVertices[] =
		{
			{ { 0.0f, 0.25f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
			{ { 0.25f, -0.25f, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
			{ { -0.25f, -0.25f, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } }
		};

		const UINT vertexBufferSize = sizeof(triangleVertices);

		auto heap_pros = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		auto buffer = CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize);
		// Note: using upload heaps to transfer static data like vert buffers is not 
		// recommended. Every time the GPU needs it, the upload heap will be marshalled 
		// over. Please read up on Default Heap usage. An upload heap is used here for 
		// code simplicity and because there are very few verts to actually transfer.
		winrt::check_hresult(m_device->CreateCommittedResource(
			&heap_pros,
			D3D12_HEAP_FLAG_NONE,
			&buffer,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&m_vertexBuffer)));

		// Copy the triangle data to the vertex buffer.
		UINT8* pVertexDataBegin;
		CD3DX12_RANGE readRange(0, 0);        // We do not intend to read from this resource on the CPU.
		winrt::check_hresult(m_vertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin)));
		memcpy(pVertexDataBegin, triangleVertices, sizeof(triangleVertices));
		m_vertexBuffer->Unmap(0, nullptr);

		// Initialize the vertex buffer view.
		m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
		m_vertexBufferView.StrideInBytes = sizeof(Vertex);
		m_vertexBufferView.SizeInBytes = vertexBufferSize;
	}

	// Create resource heap.
	{
		m_cbvResourceSize = (sizeof(DirectX::SimpleMath::Matrix) + 255) & ~255;
		m_resourceHeap = CreateResourceHeap();
	}


	// Create synchronization objects.
	{
		winrt::check_hresult(m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
		m_fenceValue = 1;

		// Create an event handle to use for frame synchronization.
		m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		if (m_fenceEvent == nullptr)
		{
			winrt::check_hresult(HRESULT_FROM_WIN32(GetLastError()));
		}
	}


	winrt::check_hresult(GameInputCreate(m_gameInput.put()));
}

Vortex::Renderer::~Renderer()
{

	// Ensure that the GPU is no longer referencing resources that are about to be
	// cleaned up by the destructor.
	WaitForPreviousFrame();

	CloseHandle(m_fenceEvent);

	m_gameInput.detach();
}

void Vortex::Renderer::Update()
{
	// Get input.
	IGameInputReading* reading;
	if (SUCCEEDED(m_gameInput->GetCurrentReading(GameInputKindKeyboard, m_gameKeyboard.get(), &reading)))
	{
		if (!m_gameKeyboard) reading->GetDevice(m_gameKeyboard.put());

		auto state = std::vector<GameInputKeyState>(reading->GetKeyCount());
		reading->GetKeyState(state.size(), state.data());
		reading->Release();

		if (!state.empty() && state.front().virtualKey != '\0') 
		{
			// Update matrix to gpu.
			static int size = 1;
			UINT8* data;
			DirectX::SimpleMath::Matrix world = DirectX::SimpleMath::Matrix::CreateScale(size * 0.01f);
			world = m_camera.GetViewMatrix().Transpose();
			CD3DX12_RANGE readRange(0, 0);
			winrt::check_hresult(m_cbvResource->Map(0, &readRange, reinterpret_cast<void**>(&data)));
			memcpy(data, &world, sizeof(DirectX::SimpleMath::Matrix));
			m_cbvResource->Unmap(0, nullptr);
			size++;
		}
	}

	// If an error is returned from GetCurrentReading(), it means the
	// gamepad we were reading from has disconnected. Reset the
	// device pointer, and go back to looking for an active gamepad.
	else if (m_gameKeyboard)
	{
		m_gameKeyboard->Release();
		m_gameKeyboard.detach();
	}
}

void Vortex::Renderer::Render()
{
	// Record all the commands we need to render the scene into the command list.
	PopulateCommandList();

	// Execute the command list.
	ID3D12CommandList* ppCommandLists[] = { m_commandList.get() };
	m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	// Present the frame.
	winrt::check_hresult(m_swapChain->Present(1, 0));

	WaitForPreviousFrame();
}

void Vortex::Renderer::PopulateCommandList()
{
	// Command list allocators can only be reset when the associated 
	// command lists have finished execution on the GPU; apps should use 
	// fences to determine GPU execution progress.
	winrt::check_hresult(m_commandAllocator->Reset());

	// However, when ExecuteCommandList() is called on a particular command 
	// list, that command list can then be reset at any time and must be before 
	// re-recording.
	winrt::check_hresult(m_commandList->Reset(m_commandAllocator.get(), m_pipelineState.get()));

	ID3D12DescriptorHeap* heaps[] = { m_resourceHeap.get() };
	m_commandList->SetDescriptorHeaps(1, heaps);
	m_commandList->SetGraphicsRootSignature(m_rootSignature.get());
	m_commandList->SetGraphicsRootConstantBufferView(0, m_cbvResource->GetGPUVirtualAddress());
	//m_commandList->SetGraphicsRootDescriptorTable(0, m_resourceHeap->GetGPUDescriptorHandleForHeapStart());
	m_commandList->RSSetViewports(1, &m_viewport);
	m_commandList->RSSetScissorRects(1, &m_scissorRect);

	// Indicate that the back buffer will be used as a render target.
	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_frameIndex].get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	m_commandList->ResourceBarrier(1, &barrier);

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_frameIndex, m_rtvDescriptorSize);
	m_commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

	// Record commands.
	const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
	m_commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_commandList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
	m_commandList->DrawInstanced(3, 1, 0, 0);

	// Indicate that the back buffer will now be used to present.
	barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_frameIndex].get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	m_commandList->ResourceBarrier(1, &barrier);

	winrt::check_hresult(m_commandList->Close());
}

void Vortex::Renderer::WaitForPreviousFrame()
{
	// WAITING FOR THE FRAME TO COMPLETE BEFORE CONTINUING IS NOT BEST PRACTICE.
	// This is code implemented as such for simplicity. The D3D12HelloFrameBuffering
	// sample illustrates how to use fences for efficient resource usage and to
	// maximize GPU utilization.

	// Signal and increment the fence value.
	const UINT64 fence = m_fenceValue;
	winrt::check_hresult(m_commandQueue->Signal(m_fence.get(), fence));
	m_fenceValue++;

	// Wait until the previous frame is finished.
	if (m_fence->GetCompletedValue() < fence)
	{
		winrt::check_hresult(m_fence->SetEventOnCompletion(fence, m_fenceEvent));
		WaitForSingleObject(m_fenceEvent, INFINITE);
	}

	m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
}

winrt::com_ptr<ID3D12DescriptorHeap> Vortex::Renderer::CreateResourceHeap()
{
	winrt::com_ptr<ID3D12DescriptorHeap> resourceHeap;
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc = {};
	descriptorHeapDesc.NumDescriptors = 3;
	descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	winrt::check_hresult(m_device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&resourceHeap)));

	INT descriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	CD3DX12_CPU_DESCRIPTOR_HANDLE descriptorHandle(resourceHeap->GetCPUDescriptorHandleForHeapStart());
	// Constant buffer view
	{
		D3D12_HEAP_PROPERTIES uploadHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		D3D12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(m_cbvResourceSize);
		winrt::check_hresult(m_device->CreateCommittedResource(
			&uploadHeapProperties,
			D3D12_HEAP_FLAG_NONE,
			&bufferDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&m_cbvResource)
		));
		m_cbvResource->SetName(L"CBV resource");

		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
		cbvDesc.BufferLocation = m_cbvResource->GetGPUVirtualAddress();
		cbvDesc.SizeInBytes = m_cbvResourceSize;
		m_device->CreateConstantBufferView(&cbvDesc, descriptorHandle);
	}
	descriptorHandle.Offset(1, descriptorSize);

	//// Shader resource view
	//{
	//	D3D12_SUBRESOURCE_DATA subresourceData;
	//	std::unique_ptr<uint8_t[]> decodedData;
	//	winrt::check_hresult(DirectX::LoadWICTextureFromFile(m_device.get(), m_textureFilename.c_str(), m_srvResource.put(), decodedData, subresourceData));

	//	D3D12_RESOURCE_DESC textureDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R8G8B8A8_UNORM, 1024, 1024);
	//	D3D12_HEAP_PROPERTIES defaultHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	//	winrt::check_hresult(m_device->CreateCommittedResource(
	//		&defaultHeapProperties,
	//		D3D12_HEAP_FLAG_NONE,
	//		&textureDesc,
	//		D3D12_RESOURCE_STATE_COPY_DEST,
	//		nullptr,
	//		IID_PPV_ARGS(&m_srvResource)
	//	));


	//	winrt::com_ptr<ID3D12Resource> uploadResource;
	//	D3D12_HEAP_PROPERTIES uploadHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	//	const UINT64 uploadBufferSize = GetRequiredIntermediateSize(m_srvResource.get(), 0, 1);
	//	D3D12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);
	//	winrt::check_hresult(m_device->CreateCommittedResource(
	//		&uploadHeapProperties,
	//		D3D12_HEAP_FLAG_NONE,
	//		&bufferDesc,
	//		D3D12_RESOURCE_STATE_GENERIC_READ,
	//		nullptr,
	//		IID_PPV_ARGS(&uploadResource)
	//	));


	//	UpdateSubresources(m_commandList.get(), m_srvResource.get(), uploadResource.get(), 0, 0, 1, &subresourceData);

	//	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	//	srvDesc.Format = textureDesc.Format;
	//	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	//	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	//	srvDesc.Texture2D.MostDetailedMip = 0;
	//	srvDesc.Texture2D.MipLevels = 1;
	//	m_device->CreateShaderResourceView(m_srvResource.get(), &srvDesc, descriptorHandle);
	//}
	//descriptorHandle.Offset(1, descriptorSize);

	//// Unordered access view
	//{
	//	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	//	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	//	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
	//	uavDesc.Buffer.FirstElement = 0;
	//	uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;
	//	m_device->CreateUnorderedAccessView(m_uavResource.get(), nullptr, &uavDesc, descriptorHandle);
	//}
	//descriptorHandle.Offset(1, descriptorSize);


	return resourceHeap;
}

winrt::com_ptr<ID3D12DescriptorHeap> Vortex::Renderer::CreateSamplerHeap()
{
	winrt::com_ptr<ID3D12DescriptorHeap> m_cbvHeap;
	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc = {};
	cbvHeapDesc.NumDescriptors = 1;
	cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
	winrt::check_hresult(m_device->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&m_cbvHeap)));

	return m_cbvHeap;
}

// Create descriptor heaps.
winrt::com_ptr<ID3D12DescriptorHeap> Vortex::Renderer::CreateRTVHeap()
{
	// Describe and create a render target view (RTV) descriptor heap.
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.NumDescriptors = FrameCount;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	winrt::check_hresult(m_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap)));

	m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	return m_rtvHeap;
}