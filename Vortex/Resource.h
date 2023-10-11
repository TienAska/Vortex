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

	class StreamReader : public Microsoft::glTF::IStreamReader
	{
	public:
		StreamReader(/*std::filesystem::path filePath*/) : m_stream(std::make_shared<std::istream>(/*std::ios_base::app | std::ios_base::binary | std::ios_base::in | std::ios_base::out*/))
		{
			//assert(filePath.ex)
		}

		std::shared_ptr<std::istream> GetInputStream(const std::string&) const override
		{
			return m_stream;
		}

	private:
		std::shared_ptr<std::istream> m_stream;
	};


	class VORTEX_API Mesh
	{
	public:
		Mesh();
		~Mesh();

		ID3D12Resource* GetResource()
		{
			return m_Resource.get();
		}

	private:
		winrt::com_ptr<ID3D12Resource> m_Resource;
	};
}
