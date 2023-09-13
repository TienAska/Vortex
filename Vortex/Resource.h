#pragma once
namespace Vortex {
	class VORTEX_API Resource
	{
	public:
		Resource();
		~Resource();
	};

	winrt::com_ptr<ID3D12Resource> CreateResource();
}
