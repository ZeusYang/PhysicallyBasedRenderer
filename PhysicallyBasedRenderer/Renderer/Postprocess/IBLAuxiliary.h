#pragma once

namespace Renderer
{
	class IBLAuxiliary
	{
	private:
		IBLAuxiliary() = default;
		~IBLAuxiliary() = default;

	public:
		static void convertToCubemap(int width, int height, unsigned int hdrTexIndex,
			unsigned int cuebmapTexIndex);

		static void convoluteDiffuseIntegral(int width, int height, unsigned int cubemapTexIndex,
			unsigned int irradianceTexIndex);

		static void convoluteSpecularIntegral(int width, int height, unsigned int cubemapTexIndex,
			unsigned int prefilteredTexIndex);

		static void convoluteSpecularBRDFIntegral(int width, int height, unsigned int brdfLutTexIndex);
	};
}
