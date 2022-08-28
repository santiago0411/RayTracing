#include "Renderer.h"

#include "Walnut/Random.h"

namespace Utils
{
	static uint32_t ConvertToRGBA(const glm::vec4& color)
	{
		uint8_t r = (uint8_t)(color.r * 255.0f);
		uint8_t g = (uint8_t)(color.g * 255.0f);
		uint8_t b = (uint8_t)(color.b * 255.0f);
		uint8_t a = (uint8_t)(color.a * 255.0f);

		uint32_t result = a << 24 | b << 16 | g << 8 | r;
		return result;
	}
}

void Renderer::OnResize(uint32_t width, uint32_t height)
{
	if (m_FinalImage)
	{
		if (m_FinalImage->GetWidth() == width && m_FinalImage->GetHeight() == height)
			return;

		m_FinalImage->Resize(width, height);
	}
	else
	{
		m_FinalImage = std::make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA);
	}

	delete[] m_ImageData;
	m_ImageData = new uint32_t[width * height];
}

void Renderer::Render() const
{
	for (uint32_t y = 0; y < m_FinalImage->GetHeight(); y++)
	{
		for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++)
		{
			glm::vec2 coord = { (float)x / (float)m_FinalImage->GetWidth(), (float)y / (float)m_FinalImage->GetHeight() };
			coord = coord * 2.0f - 1.0f; // Remap to -1 -> 1

			glm::vec4 color = PerPixel(coord);
			color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));
			m_ImageData[x + y * m_FinalImage->GetWidth()] = Utils::ConvertToRGBA(color);
		}
	}

	m_FinalImage->SetData(m_ImageData);
}

glm::vec4 Renderer::PerPixel(const glm::vec2& coord) const
{
	constexpr glm::vec3 rayOrigin(0.0f, 0.0f, 1.0f);
	const glm::vec3 rayDirection(coord.x, coord.y, -1.0f);
	// rayDirection = glm::normalize(rayDirection);
	constexpr float radius = 0.5f;

	// (bx^2 + by^2 + bz^2)t^2 + (2(axbx + ayby + azbz))t + (ax^2 + ay^2 + az^2 - r^2) = 0
	// where
	// a = ray origin
	// b = ray direction
	// r = radius
	// t = hit distance

	const float a = glm::dot(rayDirection, rayDirection);
	const float b = 2.0f * glm::dot(rayOrigin, rayDirection);
	const float c = glm::dot(rayOrigin, rayOrigin) - radius * radius;

	// Quadratic formula discriminant:
	// b^2 - 4ac

	// -b +- sqrt(discriminant) / 2a

	const float discriminant = b * b - 4.0f * a * c;

	if (discriminant < 0.0f)
		return { 0, 0, 0, 1 };

	const float t0 = (- b + glm::sqrt(discriminant)) / (2.0f * a);
	const float closestT = (- b - glm::sqrt(discriminant)) / (2.0f * a);

	const glm::vec3 hitPoint = rayOrigin + rayDirection * closestT;
	const glm::vec3 normal = glm::normalize(hitPoint);

	const glm::vec3 lightDir = glm::normalize(m_LightDirection);

	// Invert the light direction to have an outgoing vector (from the sphere towards the light)
	const float d = glm::max(glm::dot(normal, -lightDir), 0.0f); // == cos(angle)

	glm::vec3 sphereColor = m_SphereColor; // (1, 0, 1);
	sphereColor *= d;
	return { sphereColor, 1.0f };
}
