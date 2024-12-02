#pragma once

#include "Camera.h"
#include "Ray.h"
#include "Scene.h"

#include "Walnut/Image.h"

#include <memory>
#include <glm/glm.hpp>

class Renderer {

public:

	Renderer() = default;

	void Render(const Scene& scene, const Camera& camera);

	void OnResize(uint32_t width, uint32_t height);

	std::shared_ptr<Walnut::Image> GetFinalImage() const { return mFinalImage; }
private:

	struct HitPayload {
		float HitDistance;
		glm::vec3 WorldPosition;
		glm::vec3 WorldNormal;

		int ObjectIndex;
	};

	glm::vec4 PerPixel(uint32_t x, uint32_t y); // RayGen

	HitPayload TraceRay(const Ray& ray);
	HitPayload ClosestHit(const Ray& ray, float hitDistance, int objectIndex);
	HitPayload Miss(const Ray& ray);

	const Scene* mActiveScene = nullptr;
	const Camera* mActiveCamera = nullptr;

	uint32_t* mImageData = nullptr;
	std::shared_ptr<Walnut::Image> mFinalImage;
};