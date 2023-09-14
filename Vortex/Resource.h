#pragma once

namespace Vortex 
{
	//class VORTEX_API Resource
	//{
	//public:
	//	Resource();
	//	~Resource();
	//};

	//winrt::com_ptr<ID3D12Resource> CreateResource();

	//static 	winrt::com_ptr<ID3D12Resource> GetTextureResource(uint32_t index);


	class VORTEX_API Texture
	{
	public:
		//Texture();
		//~Texture();

		ID3D12Resource* GetResource()
		{
			return m_Resource.get();
		}

	private:
		winrt::com_ptr<ID3D12Resource> m_Resource;
	};
}
