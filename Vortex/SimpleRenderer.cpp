#include "pch.h"

#include "SimpleRenderer.h"
#include "Renderer.h"


SimpleRenderer::SimpleRenderer(HWND hwnd, UINT width, UINT height)
{
	m_renderer = std::make_shared<Vortex::Renderer>(hwnd, width, height);
}

SimpleRenderer::~SimpleRenderer()
{
	m_renderer.reset();
}

void SimpleRenderer::Render()
{
	m_renderer->Render();
}
