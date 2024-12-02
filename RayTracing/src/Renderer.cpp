#include "Renderer.h"

#include "Walnut/Random.h"

using Walnut::Random;

namespace Utils {
	uint32_t ConvertToRGBA(const glm::vec4& color) {

		 uint8_t r = (uint8_t)(color.r * 255.0f);
		 uint8_t g = (uint8_t)(color.g * 255.0f);
		 uint8_t b = (uint8_t)(color.b * 255.0f);
		 uint8_t a = (uint8_t)(color.a * 255.0f);

		 uint32_t result = (a << 24) | (b << 16) | (g << 8) | r;
		 return result;
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
}

glm::vec4 Renderer::PerPixel(uint32_t x, uint32_t y) {

	Ray ray;
	ray.Origin = mActiveCamera->GetPosition();
	ray.Direction = mActiveCamera->GetRayDirections()[x + y * mFinalImage->GetWidth()];
	
	glm::vec3 color(0.0f);

	float multiplier = 1.0f;

	int bounces = 2;
	for (int i = 0; i < bounces; i++) {

		Renderer::HitPayload payload = TraceRay(ray);
		if (payload.HitDistance < 0.0f) {

			glm::vec3 skyColor = glm::vec3(0.0f, 0.0f, 0.0f);
			color += skyColor * multiplier;
			break;
		}

		glm::vec3 lightDir = glm::normalize(glm::vec3(-1, -1, -1));

		float lightIntensity = glm::max(glm::dot(payload.WorldNormal, -lightDir), 0.0f); // == cos(angle)

		const Sphere& sphere = mActiveScene->Spheres[payload.ObjectIndex];

		glm::vec3 sphereColor = sphere.Albedo;
		sphereColor *= lightIntensity;
		color += sphereColor * multiplier;

		multiplier *= 0.7f;

		ray.Origin = payload.WorldPosition + payload.WorldNormal * 0.0001f;
		ray.Direction = glm::reflect(ray.Direction, payload.WorldNormal);
	}

	return glm::vec4(color, 1.0f);
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

	for (uint32_t y = 0; y < mFinalImage->GetHeight(); y++) {

		for (uint32_t x = 0; x < mFinalImage->GetWidth(); x++) {

			glm::vec4 color = PerPixel(x, y);

			color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));
			mImageData[x + y * mFinalImage->GetWidth()] = Utils::ConvertToRGBA(color);
		}
	}
	mFinalImage->SetData(mImageData);
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