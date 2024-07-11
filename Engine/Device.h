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
		// Variables
		DXGI_ADAPTER_DESC2 m_adapterDesc;
		winrt::com_ptr<ID3D12Device> m_d3d12Device;
		winrt::com_ptr<ID3D12DescriptorHeap> m_shaderVisibleHeap;
		winrt::com_ptr<ID3D12DescriptorHeap> m_rtvHeap;
	public:
		// Functions
		winrt::com_ptr<ID3D12CommandQueue> CreateCommandQueue() const;
		winrt::com_ptr<IDXGISwapChain3> CreateSwapChain(const winrt::com_ptr<ID3D12CommandQueue>& commandQueue, HWND hWnd, uint32_t width, uint32_t height, winrt::com_ptr<ID3D12Resource> renderTargets[VX_DOUBLE_BUFFER]);
		// Getters
        inline winrt::hstring GetDescription() const { return m_adapterDesc.Description; }
	};
}