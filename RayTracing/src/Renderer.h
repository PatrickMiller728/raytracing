#pragma once

#include "Walnut/Image.h"

#include <memory>
#include <glm/glm.hpp>

class Renderer {

public:

	Renderer() = default;

	void Render();

	void OnResize(uint32_t width, uint32_t height);

	std::shared_ptr<Walnut::Image> GetFinalImage() const { return mFinalImage; }
private:

	uint32_t PerPixel(glm::vec2 coord);

	uint32_t* mImageData = nullptr;
	std::shared_ptr<Walnut::Image> mFinalImage;
};