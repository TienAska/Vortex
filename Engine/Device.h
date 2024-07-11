#pragma once

namespace Vortex
{
	class Device
	{
	public:
        inline winrt::hstring GetDescription() const { return m_adapterDesc.Description; }

		static void Initialize();
        inline static const Device& GetDevice0() { WINRT_ASSERT(s_deviceList.size() > 0); return s_deviceList[0]; }
		
		static winrt::com_ptr<ID3D12Resource> CreateResource();
	private:
		Device() = delete;
		Device(const winrt::com_ptr<IDXGIAdapter3>& adaptor);
		
		DXGI_ADAPTER_DESC2 m_adapterDesc;
		winrt::com_ptr<ID3D12Device> m_d3d12Device;

		static winrt::com_ptr<IDXGIFactory6> s_dxgiFactory;
		static std::vector<Device> s_deviceList;
        inline static size_t Count() { return s_deviceList.size(); }
	};
}