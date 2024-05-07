#include "Font.h"
#include "Defs.h"

#undef INFINITE
#include "msdf-atlas-gen.h"
#include "FontGeometry.h"
#include "GlyphGeometry.h"

#include "MSDFData.h"
#include <extern/msdf-atlas-gen/msdfgen/ext/import-font.h>
#include <extern/msdf-atlas-gen/msdf-atlas-gen/glyph-generators.cpp>

template<typename T, typename S, int N, msdf_atlas::GeneratorFunction<S, N> GenFunc>
static std::shared_ptr<Texture> CreateAndCacheAtlas(const std::string& fontName, float fontSize, const std::vector<msdf_atlas::GlyphGeometry>& glyphs,
	const msdf_atlas::FontGeometry& fontGeometry, uint32_t width, uint32_t height)
{
	msdf_atlas::GeneratorAttributes attributes;
	attributes.config.overlapSupport = true;
	attributes.scanlinePass = true;

	msdf_atlas::ImmediateAtlasGenerator<S, N, GenFunc, msdf_atlas::BitmapAtlasStorage<T, N>> generator(width, height);
	generator.setAttributes(attributes);
	generator.setThreadCount(8);
	generator.generate(glyphs.data(), (int)glyphs.size());

	msdfgen::BitmapConstRef<T, N> bitmap = (msdfgen::BitmapConstRef<T, N>)generator.atlasStorage();

	// Crear un buffer para almacenar los datos volteados
	//std::vector<T> flippedPixels(bitmap.width * bitmap.height * N);
	//for (int y = 0; y < bitmap.height; ++y) {
	//	for (int x = 0; x < bitmap.width; ++x) {
	//		int srcIndex = (y * bitmap.width + x) * N; // Índice original
	//		int destIndex = ((bitmap.height - 1 - y) * bitmap.width + (bitmap.width - 1 - x)) * N; // Índice de destino volteado
	//		std::memcpy(&flippedPixels[destIndex], &bitmap.pixels[srcIndex], N * sizeof(T));
	//	}
	//}

	std::shared_ptr<Texture> texture = std::make_shared<Texture>(glm::ivec2(bitmap.width, bitmap.height), 3);
	texture->SetData((void*)bitmap.pixels, bitmap.width * bitmap.height * 3);
	return texture;
}

Font::Font(const std::filesystem::path& filepath)
	: m_Data(new MSDFData())
{
	msdfgen::FreetypeHandle* ft = msdfgen::initializeFreetype();
	assert(ft);

	std::string fileString = filepath.string();

	// TODO(Yan): msdfgen::loadFontData loads from memory buffer which we'll need 
	msdfgen::FontHandle* font = msdfgen::loadFont(ft, fileString.c_str());
	if (!font)
	{
		LOG(LogType::LOG_ERROR, "Failed to load font: {}", fileString);
		return;
	}

	struct CharsetRange
	{
		uint32_t Begin, End;
	};

	// From imgui_draw.cpp
	static const CharsetRange charsetRanges[] =
	{
		{ 0x0020, 0x00FF }
	};

	msdf_atlas::Charset charset;
	for (CharsetRange range : charsetRanges)
	{
		for (uint32_t c = range.Begin; c <= range.End; c++)
			charset.add(c);
	}

	double fontScale = 1.0;
	m_Data->FontGeometry = msdf_atlas::FontGeometry(&m_Data->Glyphs);
	int glyphsLoaded = m_Data->FontGeometry.loadCharset(font, fontScale, charset);
	LOG(LogType::LOG_INFO, "Loaded %d glyphs from font (out of %d)", glyphsLoaded, charset.size());


	double emSize = 40.0;

	msdf_atlas::TightAtlasPacker atlasPacker;
	// atlasPacker.setDimensionsConstraint()
	atlasPacker.setPixelRange(2.0);
	atlasPacker.setMiterLimit(1.0);
	atlasPacker.setPadding(0);
	atlasPacker.setScale(emSize);
	int remaining = atlasPacker.pack(m_Data->Glyphs.data(), (int)m_Data->Glyphs.size());
	assert(remaining == 0);

	int width, height;
	atlasPacker.getDimensions(width, height);
	emSize = atlasPacker.getScale();

#define DEFAULT_ANGLE_THRESHOLD 3.0
#define LCG_MULTIPLIER 6364136223846793005ull
#define LCG_INCREMENT 1442695040888963407ull
#define THREAD_COUNT 8
	// if MSDF || MTSDF

	uint64_t coloringSeed = 0;
	bool expensiveColoring = false;
	if (expensiveColoring)
	{
		msdf_atlas::Workload([&glyphs = m_Data->Glyphs, &coloringSeed](int i, int threadNo) -> bool {
			unsigned long long glyphSeed = (LCG_MULTIPLIER * (coloringSeed ^ i) + LCG_INCREMENT) * !!coloringSeed;
			glyphs[i].edgeColoring(msdfgen::edgeColoringInkTrap, DEFAULT_ANGLE_THRESHOLD, glyphSeed);
			return true;
		}, m_Data->Glyphs.size()).finish(THREAD_COUNT);
	}
	else {
		unsigned long long glyphSeed = coloringSeed;
		for (msdf_atlas::GlyphGeometry& glyph : m_Data->Glyphs)
		{
			glyphSeed *= LCG_MULTIPLIER;
			glyph.edgeColoring(msdfgen::edgeColoringInkTrap, DEFAULT_ANGLE_THRESHOLD, glyphSeed);
		}
	}


	m_AtlasTexture = CreateAndCacheAtlas<uint8_t, float, 3, msdf_atlas::msdfGenerator>("Test", (float)emSize, m_Data->Glyphs, m_Data->FontGeometry, width, height);


#if 0
	msdfgen::Shape shape;
	if (msdfgen::loadGlyph(shape, font, 'C'))
	{
		shape.normalize();
		//                      max. angle
		msdfgen::edgeColoringSimple(shape, 3.0);
		//           image width, height
		msdfgen::Bitmap<float, 3> msdf(32, 32);
		//                     range, scale, translation
		msdfgen::generateMSDF(msdf, shape, 4.0, 1.0, msdfgen::Vector2(4.0, 4.0));
		msdfgen::savePng(msdf, "output.png");
	}
#endif

	msdfgen::destroyFont(font);
	msdfgen::deinitializeFreetype(ft);
}

Font::~Font()
{
	delete m_Data;
}


std::shared_ptr<Font> Font::GetDefault()
{
	static std::shared_ptr<Font> DefaultFont;
	if (!DefaultFont)
		DefaultFont = std::make_shared<Font>("Assets/Fonts/ComicSansMS.ttf");

	return DefaultFont;
}
