#include "Renderer.h"

#include <execution>

namespace Utils {
	uint32_t ConvertToRGBA(const glm::vec4& color) {

		 uint8_t r = (uint8_t)(color.r * 255.0f);
		 uint8_t g = (uint8_t)(color.g * 255.0f);
		 uint8_t b = (uint8_t)(color.b * 255.0f);
		 uint8_t a = (uint8_t)(color.a * 255.0f);

		 uint32_t result = (a << 24) | (b << 16) | (g << 8) | r;
		 return result;
	}

	static uint32_t PCG_Hash(uint32_t input) {

		uint32_t state = input * 747796405u + 2891336453u;
		uint32_t word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
		return (word >> 22u) ^ word;
	}

	static float RandomFloat(uint32_t& seed) {

		seed = PCG_Hash(seed);
		return (float)seed / (float)std::numeric_limits<uint32_t>::max();
	}

	static glm::vec3 InUnitSphere(uint32_t& seed) {
		
		return glm::normalize(glm::vec3(
			RandomFloat(seed) * 2.0f - 1.0f,
			RandomFloat(seed) * 2.0f - 1.0f,
			RandomFloat(seed) * 2.0f - 1.0f));
	}
}

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

	delete[] mAccumulationData;
	mAccumulationData = new glm::vec4[width * height];

	mImageHorizontalIter.resize(width);
	mImageVerticalIter.resize(height);

	for (uint32_t i = 0; i < width; i++) {
		mImageHorizontalIter[i] = i;
	}

	for (uint32_t i = 0; i < height; i++) {
		mImageVerticalIter[i] = i;
	}
}

glm::vec4 Renderer::PerPixel(uint32_t x, uint32_t y) {

	Ray ray;
	ray.Origin = mActiveCamera->GetPosition();
	ray.Direction = mActiveCamera->GetRayDirections()[x + y * mFinalImage->GetWidth()];
	
	glm::vec3 light(0.0f);
	glm::vec3 contribution(1.0f);

	uint32_t seed = x + y * mFinalImage->GetWidth();
	seed *= mFrameIndex;

	int bounces = 5;
	for (int i = 0; i < bounces; i++) {

		seed += i;

		Renderer::HitPayload payload = TraceRay(ray);
		if (payload.HitDistance < 0.0f) {

			glm::vec3 skyColor = glm::vec3(0.6f, 0.7f, 0.9f);
			// light += skyColor * contribution;
			break;
		}

		const Sphere& sphere = mActiveScene->Spheres[payload.ObjectIndex];
		const Material& material = mActiveScene->Materials[sphere.MaterialIndex];

		light += material.GetEmission();
		contribution *= material.Albedo;

		ray.Origin = payload.WorldPosition + payload.WorldNormal * 0.0001f;
		ray.Direction = glm::normalize(payload.WorldNormal + Utils::InUnitSphere(seed));
	}

	return glm::vec4(light, 1.0f);
}

Renderer::HitPayload Renderer::ClosestHit(const Ray& ray, float hitDistance, int objectIndex) {

	Renderer::HitPayload payload;
	payload.HitDistance = hitDistance;
	payload.ObjectIndex = objectIndex;

	const Sphere& closestSphere = mActiveScene->Spheres[objectIndex];

	glm::vec3 origin = ray.Origin - closestSphere.Position;
	payload.WorldPosition = origin + ray.Direction * hitDistance;
	payload.WorldNormal = glm::normalize(payload.WorldPosition);

	payload.WorldPosition += closestSphere.Position;

	return payload;
}

Renderer::HitPayload Renderer::Miss(const Ray& ray) {
		
	Renderer::HitPayload payload;
	payload.HitDistance = -1;
	return payload;
}

void Renderer::Render(const Scene& scene, const Camera& camera) {

	mActiveScene = &scene;
	mActiveCamera = &camera;

	if (mFrameIndex == 1) {
		memset(mAccumulationData, 0, mFinalImage->GetWidth() * mFinalImage->GetHeight() * sizeof(glm::vec4));
	}

	std::for_each(std::execution::par, mImageVerticalIter.begin(), mImageVerticalIter.end(),
		[this](uint32_t y) {
			std::for_each(std::execution::par, mImageHorizontalIter.begin(), mImageHorizontalIter.end(),
			[this, y](uint32_t x) {
					glm::vec4 color = PerPixel(x, y);
					mAccumulationData[x + y * mFinalImage->GetWidth()] += color;

					glm::vec4 accumulatedColor = mAccumulationData[x + y * mFinalImage->GetWidth()];
					accumulatedColor /= (float)mFrameIndex;

					accumulatedColor = glm::clamp(accumulatedColor, glm::vec4(0.0f), glm::vec4(1.0f));
					mImageData[x + y * mFinalImage->GetWidth()] = Utils::ConvertToRGBA(accumulatedColor);
				});
		});

	mFinalImage->SetData(mImageData);

	if (mSettings.Accumulate) {
		mFrameIndex++;
	}
	else {
		mFrameIndex = 1;
	}
}

Renderer::HitPayload Renderer::TraceRay(const Ray& ray) {

	// THE 2D RAY TRACING FORMULA
	// [    var a   ]     [     var b    ]    [      var c      ]
	// (bx^2 + by^2)t^2 + (2(axbx + ayby))t + (ax^2 + ay^2 - r^2) = 0

	// a = ray origin
	// b = ray direction
	// r = radius 
	// t = hit distance

	int closestSphere = -1;
	float hitDistance = std::numeric_limits<float>::max();

	for (size_t i = 0; i < mActiveScene->Spheres.size(); i++) {

		const Sphere& sphere = mActiveScene->Spheres[i];

		glm::vec3 origin = ray.Origin - sphere.Position;

		float a = glm::dot(ray.Direction, ray.Direction);
		float b = 2.0f * glm::dot(origin, ray.Direction);
		float c = glm::dot(origin, origin) - sphere.Radius * sphere.Radius;

		// Quadratic formula discriminant
		// b^2 - 4ac
		float discriminant = b * b - 4.0f * a * c;

		if (discriminant <= 0.0f) {
			continue;
		}

		// (-b +- sqrt(discriminant)) / 2a
		float closestT = (-b - glm::sqrt(discriminant)) / (2.0f * a);

		if (closestT > 0.0f && closestT < hitDistance) {
			hitDistance = closestT;
			closestSphere = (int)i;
		}
	}

	if (closestSphere < 0) {
		return Miss(ray);
	}

	return ClosestHit(ray, hitDistance, closestSphere);
}