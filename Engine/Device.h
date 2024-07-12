#pragma once

#define VX_DEVICE0 Vortex::Device::GetDevice0()
#define VX_DOUBLE_BUFFER 2

namespace Vortex
{
	class Device
	{
	public:
		// Functions
		static void Initialize();
		static inline bool IsInitialized() { return s_dxgiFactory != nullptr; }
        static inline size_t Count() { return s_deviceList.size(); }
        static inline Device* GetDevice0() { WINRT_ASSERT(s_deviceList.size() > 0); return &s_deviceList[0]; }
	private:
		// Variables
		static winrt::com_ptr<IDXGIFactory6> s_dxgiFactory;
		static std::vector<Device> s_deviceList;

	private:
		// Constructors
		Device() = delete;
		Device(const winrt::com_ptr<IDXGIAdapter3>& adaptor);
	public:
		// Getters
        inline winrt::hstring GetDescription() const { return m_adapterDesc.Description; }
	private:
		// Variables
		DXGI_ADAPTER_DESC2 m_adapterDesc;
		winrt::com_ptr<ID3D12Device4> m_d3d12Device;
		winrt::com_ptr<ID3D12DescriptorHeap> m_shaderVisibleHeap;
	public:
		// Wrapped functions
		winrt::com_ptr<ID3D12Fence1> CreateFence(uint64_t value) const;

		winrt::com_ptr<ID3D12CommandQueue> CreateGraphicsCommandQueue() const;
		winrt::com_ptr<ID3D12CommandQueue> CreateComputeCommandQueue() const;
		winrt::com_ptr<ID3D12CommandQueue> CreateCopyCommandQueue() const;
		winrt::com_ptr<ID3D12CommandAllocator> CreateGraphicsCommandAllocator() const;
		winrt::com_ptr<ID3D12CommandAllocator> CreateComputeCommandAllocator() const;
		winrt::com_ptr<ID3D12CommandAllocator> CreateBundleCommandAllocator() const;
		winrt::com_ptr<ID3D12CommandAllocator> CreateCopyCommandAllocator() const;
        winrt::com_ptr<ID3D12GraphicsCommandList6> CreateGraphicsCommandList() const;
        winrt::com_ptr<ID3D12GraphicsCommandList6> CreateComputeCommandList() const;
        winrt::com_ptr<ID3D12GraphicsCommandList6> CreateBundleCommandList() const;
        winrt::com_ptr<ID3D12GraphicsCommandList6> CreateCopyCommandList() const;

		winrt::com_ptr<IDXGISwapChain3> CreateSwapChain(
			HWND hWnd, uint32_t width, uint32_t height,
			const winrt::com_ptr<ID3D12CommandQueue>& commandQueue,
			winrt::com_ptr<ID3D12Resource> renderTargets[VX_DOUBLE_BUFFER],
            winrt::com_ptr<ID3D12DescriptorHeap>& rtvHeap, uint32_t& descriptorSize) const;

		winrt::com_ptr<ID3D12RootSignature> CreateRootSignature(CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC versionedRootSignatureDesc) const;
        winrt::com_ptr<ID3D12PipelineState> CreateMeshPSO(
            const winrt::com_ptr<ID3D12RootSignature>& rootSignature,
            D3D12_SHADER_BYTECODE mesh, D3D12_SHADER_BYTECODE pixel,
            D3D12_SHADER_BYTECODE amplification = { NULL, 0 }) const;
	};
}