#pragma once

#include "Vortex.h"

namespace Vortex
{
	class VORTEX_API Shader
	{
	public:
		enum Type
		{
			Vertex,
			Amplification,
			Mesh,
			Pixel
		};


		Shader(const winrt::hstring& filename, const winrt::hstring& entryPoint, Type type);

		D3D12_SHADER_BYTECODE GetBytecode() const;

		winrt::hstring GetBinaryFilename() const;

	private:
		Type m_type;
		winrt::com_ptr<IDxcBlob> m_dxcBinary;
		winrt::com_ptr<IDxcBlobWide> m_dxcBinaryFilename;
	};
}