#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"
#include "Walnut/Timer.h"

#include "Camera.h"
#include "Renderer.h"

#include <glm/gtc/type_ptr.hpp>

using namespace Walnut;

class ExampleLayer : public Layer
{
public:
	ExampleLayer()
		: m_Camera(45.0f, 0.1f, 100.0f)
	{
		m_Scene.Spheres.push_back(Sphere{ {0.0f, 0.0f, 0.0f}, 0.5f, {1.0f, 0.0f, 1.0f} });
		m_Scene.Spheres.push_back(Sphere{ {1.0f, 0.0f, -5.0f}, 1.5f, {0.2f, 0.3f, 1.0f} });
	}

	void OnUpdate(float ts) override
	{
		m_Camera.OnUpdate(ts);
	}

	void OnUIRender() override
	{
		ImGui::Begin("Settings");
		ImGui::Text("Last render: %.3fms", m_LastRenderTime);
		ImGui::End();

		ImGui::Begin("Scene");
		for (size_t i = 0; i < m_Scene.Spheres.size(); i++)
		{
			ImGui::PushID(i);

			auto& [Position, Radius, Albedo] = m_Scene.Spheres[i];
			ImGui::DragFloat3("Position", glm::value_ptr(Position), 0.05f);
			ImGui::DragFloat("Radius", &Radius, 0.05f);
			ImGui::ColorEdit3("Albedo", glm::value_ptr(Albedo));

			ImGui::Separator();

			ImGui::PopID();
		}
		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Viewport");

		m_ViewportWidth = (uint32_t)ImGui::GetContentRegionAvail().x;
		m_ViewportHeight = (uint32_t)ImGui::GetContentRegionAvail().y;

		if (const auto image = m_Renderer.GetFinalImage())
			ImGui::Image(image->GetDescriptorSet(), { (float)image->GetWidth(), (float)image->GetHeight() },
				ImVec2(0, 1), ImVec2(1, 0));

		ImGui::End();
		ImGui::PopStyleVar();

		Render();
	}

	void Render()
	{
		Timer timer;

		m_Renderer.OnResize(m_ViewportWidth, m_ViewportHeight);
		m_Camera.OnResize(m_ViewportWidth, m_ViewportHeight);
		m_Renderer.Render(m_Scene, m_Camera);

		m_LastRenderTime = timer.ElapsedMillis();
	}

private:
	Camera m_Camera;
	Renderer m_Renderer;
	Scene m_Scene;
	uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

	float m_LastRenderTime = 0.0f;
};

Application* Walnut::CreateApplication(int argc, char** argv)
{
	ApplicationSpecification spec;
	spec.Name = "Walnut Example";

	auto* app = new Application(spec);
	app->PushLayer<ExampleLayer>();
	app->SetMenubarCallback([app]()
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Exit"))
			{
				app->Close();
			}
			ImGui::EndMenu();
		}
	});
	return app;
}