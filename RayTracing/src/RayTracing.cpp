#include "Renderer.h"
#include "Camera.h"

#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"
#include "Walnut/Image.h"
#include "Walnut/Timer.h"

#include <glm/gtc/type_ptr.hpp>

using Walnut::Image;
using Walnut::ImageFormat;
using Walnut::Timer;

class RayTracing : public Walnut::Layer {

public:

	RayTracing()
		: mCamera(45.0f, 0.1, 100.0f) {
		
		{
			Sphere sphere;
			sphere.Position = { 0.0f, 0.0f, 0.0f };
			sphere.Radius = 0.5f;
			sphere.Albedo = { 1.0f, 0.0f, 1.0f };

			mScene.Spheres.push_back(sphere);
		}

		{
			Sphere sphere;
			sphere.Position = { 1.0f, 0.0f, -5.0f };
			sphere.Radius = 1.5f;
			sphere.Albedo = { 0.2f, 0.3f, 1.0f };

			mScene.Spheres.push_back(sphere);
		}
	}

	virtual void OnUpdate(float ts) override {
		
		mCamera.OnUpdate(ts);
	}

	virtual void OnUIRender() override {

		ImGui::Begin("Settings");

		ImGui::Text("Last render: %.3fms", mLastRenderTime);
		if (ImGui::Button("Render")) {
			Render();
		}

		ImGui::End();

		ImGui::Begin("Scene");
		for (size_t i = 0; i < mScene.Spheres.size(); i++) {

			ImGui::PushID(i);

			Sphere& sphere = mScene.Spheres[i];
			ImGui::DragFloat3("Position", glm::value_ptr(sphere.Position), 0.1f);
			ImGui::DragFloat("Rotation", &sphere.Radius, 0.1f);
			ImGui::ColorEdit3("Albedo", glm::value_ptr(sphere.Albedo));

			ImGui::Separator();
			ImGui::PopID();
		}
		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Viewport");

		mViewportWidth = ImGui::GetContentRegionAvail().x;
		mViewportHeight = ImGui::GetContentRegionAvail().y;

		auto image = mRenderer.GetFinalImage();
		if (image) {

			ImGui::Image(image->GetDescriptorSet(), { (float)image->GetWidth(), (float)image->GetHeight() }, ImVec2(0, 1), ImVec2(1,0));
		}

		ImGui::End();
		ImGui::PopStyleVar();

		Render();

	}

	void Render() {

		Timer timer;

		mRenderer.OnResize(mViewportWidth, mViewportHeight);
		mCamera.OnResize(mViewportWidth, mViewportHeight);
		mRenderer.Render(mScene, mCamera);

		mLastRenderTime = timer.ElapsedMillis();
	}
private:

	Renderer mRenderer;
	Camera mCamera;
	Scene mScene;
	uint32_t mViewportWidth = 0, mViewportHeight = 0;

	float mLastRenderTime = 0.0f;
};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv) {

	Walnut::ApplicationSpecification spec;
	spec.Name = "Ray Tracing";
	spec.CustomTitlebar = true;

	Walnut::Application* app = new Walnut::Application(spec);
	app->PushLayer<RayTracing>();
	app->SetMenubarCallback([app]() {

			if (ImGui::BeginMenu("File")) {

				if (ImGui::MenuItem("Exit")) {

					app->Close();
				}
				ImGui::EndMenu();
			}
		});
	return app;
}