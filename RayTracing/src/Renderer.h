#pragma once

#include "Camera.h"
#include "Ray.h"

#include "Walnut/Image.h"

#include <memory>
#include <glm/glm.hpp>

class Renderer {

public:

	Renderer() = default;

	void Render(const Camera& camera);

	void OnResize(uint32_t width, uint32_t height);

	std::shared_ptr<Walnut::Image> GetFinalImage() const { return mFinalImage; }
private:

	glm::vec4 TraceRay(const Ray& ray);

	uint32_t* mImageData = nullptr;
	std::shared_ptr<Walnut::Image> mFinalImage;
};