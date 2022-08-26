#ifndef FONT_MANAGER_H
#define FONT_MANAGER_H

#include <filesystem>
#include <string>
#include <vector>

namespace fontManager {
	enum class FontWeight {
		Undefined = 0,
		Thin = 100,
		UltraLight = 200,
		Light = 300,
		Normal = 400,
		Medium = 500,
		SemiBold = 600,
		Bold = 700,
		UltraBold = 800,
		Heavy = 900
	};

	enum class FontWidth {
		Undefined = 0,
		UltraCondensed = 1,
		ExtraCondensed = 2,
		Condensed = 3,
		SemiCondensed = 4,
		Normal = 5,
		SemiExpanded = 6,
		Expanded = 7,
		ExtraExpanded = 8,
		UltraExpanded = 9
	};

	struct FontDescriptor {
	   public:
		std::filesystem::path path;
		std::string postscriptName;
		std::string family;
		std::string style;
		FontWeight weight;
		FontWidth width;
		bool italic;
		bool monospace;

		FontDescriptor()
			: weight(FontWeight::Undefined),
			  width(FontWidth::Undefined),
			  italic(false),
			  monospace(false) {}

		FontDescriptor(
			const char* path, const char* postscriptName, const char* family,
			const char* style, FontWeight weight, FontWidth width, bool italic,
			bool monospace)
			: path(path),
			  postscriptName(postscriptName),
			  family(family),
			  style(style),
			  weight(weight),
			  width(width),
			  italic(italic),
			  monospace(monospace) {}
	};
	std::vector<FontDescriptor> getAllFonts();
	FontDescriptor findFont(const FontDescriptor& desc);
	inline FontDescriptor findFont(const std::string& fontFamily) {
		FontDescriptor desc = FontDescriptor();
		desc.family = fontFamily;
		return findFont(desc);
	}
}  // namespace fontManager

#endif	// FONT_MANAGER_H