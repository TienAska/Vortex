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

void SimpleRenderer::Update()
{
	m_renderer->Update();
}

void SimpleRenderer::Render()
{
	m_renderer->Render();
}

void SimpleRenderer::MoveForward(float offset)
{
	m_renderer->GetCameraRef().MoveForward(offset * m_speed);
}

void SimpleRenderer::MoveRight(float offset)
{
	m_renderer->GetCameraRef().MoveRight(offset * m_speed);
}

void SimpleRenderer::MoveUp(float offset)
{
	m_renderer->GetCameraRef().MoveUp(offset * m_speed);
}

void SimpleRenderer::RotateX(float offset)
{
	m_renderer->GetCameraRef().Rotate(offset * m_speed, 0.0f);
}

void SimpleRenderer::RotateY(float offset)
{
	m_renderer->GetCameraRef().Rotate(0.0f * m_speed, offset);
}
