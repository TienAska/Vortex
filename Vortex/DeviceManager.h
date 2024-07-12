#pragma once

namespace Vortex
{
	class DeviceManager
	{
	public:
		static DeviceManager& Instance()
		{
			static DeviceManager *instance = new DeviceManager();
			return *instance;
		}
		
		static winrt::com_ptr<ID3D12Device2> GetDevice()
		{
			return Instance().m_device;
		}

		static winrt::com_ptr<IDXGIFactory6> GetFactory()
		{
			return Instance().m_factory;
		}

	private:
		DeviceManager();

		// D3D12 objects.
		winrt::com_ptr<IDXGIFactory6> m_factory;
		winrt::com_ptr<ID3D12Device2> m_device;
	};
}