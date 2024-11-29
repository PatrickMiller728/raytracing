#include "Renderer.h"

#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"
#include "Walnut/Image.h"
#include "Walnut/Timer.h"

using Walnut::Image;
using Walnut::ImageFormat;
using Walnut::Timer;

class RayTracing : public Walnut::Layer {

public:
	virtual void OnUIRender() override {

		ImGui::Begin("Settings");

		ImGui::Text("Last render: %.3fms", mLastRenderTime);
		if (ImGui::Button("Render")) {
			Render();
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
		mRenderer.Render();

		mLastRenderTime = timer.ElapsedMillis();
	}
private:

	Renderer mRenderer;
	uint32_t mViewportWidth, mViewportHeight;
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