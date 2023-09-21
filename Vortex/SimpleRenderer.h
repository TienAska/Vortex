#pragma once

namespace Vortex
{
	class Renderer;
}

class VORTEX_API SimpleRenderer
{
public:
	SimpleRenderer(HWND hwnd, UINT width, UINT height);
	~SimpleRenderer();

	void Render();

private:
	std::shared_ptr<Vortex::Renderer> m_renderer;
};