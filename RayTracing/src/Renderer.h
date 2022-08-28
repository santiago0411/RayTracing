#pragma once

#include "Walnut/Image.h"

#include <memory>
#include <glm/glm.hpp>

class Renderer
{
public:
	Renderer() = default;
	~Renderer() = default;

	void OnResize(uint32_t width, uint32_t height);
	void Render() const;

	void SetSphereColor(const glm::vec3& color) { m_SphereColor = color; }
	void SetLightDirection(const glm::vec3& dir) { m_LightDirection = dir; }

	std::shared_ptr<Walnut::Image> GetFinalImage() const { return m_FinalImage; }

private:
	glm::vec4 PerPixel(const glm::vec2& coord) const;

private:
	std::shared_ptr<Walnut::Image> m_FinalImage;
	uint32_t* m_ImageData = nullptr;

	glm::vec3 m_SphereColor;
	glm::vec3 m_LightDirection;
};
