#include "Renderer.h"

#include "Walnut/Random.h"

using Walnut::Random;

void Renderer::OnResize(uint32_t width, uint32_t height) {

	if (mFinalImage) {

		// No resize necessary
		if (mFinalImage->GetWidth() == width && mFinalImage->GetHeight() == height) {
			return;
		}

		mFinalImage->Resize(width, height);
	}
	else {

		mFinalImage = std::make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA);
	}

	delete[] mImageData;
	mImageData = new uint32_t[width * height];
}

void Renderer::Render() {

	for (uint32_t y = 0; y < mFinalImage->GetHeight(); y++) {

		for (uint32_t x = 0; x < mFinalImage->GetWidth(); x++) {

			glm::vec2 coord = { (float)x / (float)mFinalImage->GetWidth(), (float)y / (float)mFinalImage->GetHeight() };
			coord = coord * 2.0f - 1.0f; // -1 -> 1
			mImageData[x + y * mFinalImage->GetWidth()] = PerPixel(coord);
		}
	}
	mFinalImage->SetData(mImageData);
}

uint32_t Renderer::PerPixel(glm::vec2 coord) {

	// camera origin
	glm::vec3 rayOrigin(0.0f, 0.0f, 2.0f);
	glm::vec3 rayDirection(coord.x, coord.y, -1.0f);
	float radius = 0.5f;
	// rayDirection = glm::normalize(rayDirection);

	// THE 2D RAY TRACING FORMULA
	// [    var a   ]     [     var b    ]    [      var c      ]
	// (bx^2 + by^2)t^2 + (2(axbx + ayby))t + (ax^2 + ay^2 - r^2) = 0

	// a = ray origin
	// b = ray direction
	// r = radius 
	// t = hit distance

	float a = glm::dot(rayDirection, rayDirection);
	float b = 2.0f * glm::dot(rayOrigin, rayDirection);
	float c = glm::dot(rayOrigin, rayOrigin) - radius * radius;

	// Quadratic formula discriminant
	// b^2 - 4ac

	float discriminant = b * b - 4.0f * a * c;

	if (discriminant >= 0.0f) {

		return 0xffff00ff;
	}

	return 0xff000000;
}

