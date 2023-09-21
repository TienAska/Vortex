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

	void Update();
	void Render();

	void MoveForward(float offset);
	void MoveRight(float offset);
	void MoveUp(float offset);

private:
	std::shared_ptr<Vortex::Renderer> m_renderer;
};