#include "fontManager.hpp"

#include <iostream>

#ifdef _WIN32
#include <dwrite.h>
#include <dwrite_1.h>

#include <cassert>
#include <cstdio>
#include <cstring>

#define MAX_BUFFER_SIZE 10000

#define HR(hr) \
	if (FAILED(hr)) throw "Font loading error";

// SafeRelease inline function.
template <class T> inline void SafeRelease(T **ppT) {
	if (*ppT) {
		(*ppT)->Release();
		*ppT = nullptr;
	}
}

char *utf16ToUtf8(const WCHAR *input) {
	unsigned int len =
		WideCharToMultiByte(CP_UTF8, 0, input, -1, NULL, 0, NULL, NULL);
	char *output = new char[len];
	WideCharToMultiByte(CP_UTF8, 0, input, -1, output, len, NULL, NULL);
	return output;
}

// returns the index of the user's locale in the set of localized strings
unsigned int getLocaleIndex(IDWriteLocalizedStrings *strings) {
	unsigned int index = 0;
	BOOL exists = false;
	wchar_t localeName[LOCALE_NAME_MAX_LENGTH];

	// Get the default locale for this user.
	int success = GetUserDefaultLocaleName(localeName, LOCALE_NAME_MAX_LENGTH);

	// If the default locale is returned, find that locale name, otherwise use
	// "en-us".
	if (success) {
		HR(strings->FindLocaleName(localeName, &index, &exists));
	}

	// if the above find did not find a match, retry with US English
	if (!exists) {
		HR(strings->FindLocaleName(L"en-us", &index, &exists));
	}

	if (!exists) index = 0;

	return index;
}

char *getString(IDWriteFont *font, DWRITE_INFORMATIONAL_STRING_ID string_id) {
	char *res = NULL;
	IDWriteLocalizedStrings *strings = NULL;

	BOOL exists = false;
	HR(font->GetInformationalStrings(string_id, &strings, &exists));

	if (exists) {
		unsigned int index = getLocaleIndex(strings);
		unsigned int len = 0;
		WCHAR *str = NULL;

		HR(strings->GetStringLength(index, &len));
		str = new WCHAR[len + 1];

		HR(strings->GetString(index, str, len + 1));

		// convert to utf8
		res = utf16ToUtf8(str);
		delete str;

		strings->Release();
	}

	if (!res) {
		res = new char[1];
		res[0] = '\0';
	}

	return res;
}

void resultFromFont(
	IDWriteFont *font, std::vector<fontManager::FontDescriptor> &fonts) {
	IDWriteFontFace *face = nullptr;
	unsigned int numFiles = 0;

	HR(font->CreateFontFace(&face));

	// get the font files from this font face
	IDWriteFontFile *files = nullptr;
	HR(face->GetFiles(&numFiles, nullptr));
	HR(face->GetFiles(&numFiles, &files));

	for (unsigned i = 0; i < numFiles; ++i) {
		IDWriteFontFileLoader *loader = nullptr;
		IDWriteLocalFontFileLoader *fileLoader = nullptr;

		unsigned int nameLength = 0;
		const void *referenceKey = nullptr;
		unsigned int referenceKeySize = 0;
		WCHAR name[MAX_BUFFER_SIZE];

		HR(files[0].GetLoader(&loader));

		// check if this is a local font file
		HRESULT hr = loader->QueryInterface(
			__uuidof(IDWriteLocalFontFileLoader), (void **)&fileLoader);

		if (SUCCEEDED(hr)) {
			// get the file path
			HR(files[0].GetReferenceKey(&referenceKey, &referenceKeySize));
			HR(fileLoader->GetFilePathLengthFromKey(
				referenceKey, referenceKeySize, &nameLength));
			assert(nameLength < MAX_BUFFER_SIZE);

			HR(fileLoader->GetFilePathFromKey(
				referenceKey, referenceKeySize, name, nameLength + 1));

			char *psName = utf16ToUtf8(name);
			char *postscriptName =
				getString(font, DWRITE_INFORMATIONAL_STRING_POSTSCRIPT_NAME);
			char *family =
				getString(font, DWRITE_INFORMATIONAL_STRING_WIN32_FAMILY_NAMES);
			char *style = getString(
				font, DWRITE_INFORMATIONAL_STRING_WIN32_SUBFAMILY_NAMES);
			bool monospace = false;
			// this method requires windows 7, so we need to cast to an
			// IDWriteFontFace1

			IDWriteFontFace1 *face1;
			HRESULT hr = face->QueryInterface(
				__uuidof(IDWriteFontFace1), (void **)&face1);
			if (SUCCEEDED(hr)) {
				monospace = face1->IsMonospacedFont() == TRUE;
			}

			fonts.emplace_back(
				psName,
				postscriptName,
				family,
				style,
				(fontManager::FontWeight)font->GetWeight(),
				(fontManager::FontWidth)font->GetStretch(),
				font->GetStyle() == DWRITE_FONT_STYLE_ITALIC,
				monospace);

			fileLoader->Release();
		}
		loader->Release();
	}

	face->Release();
	files->Release();
}

std::vector<fontManager::FontDescriptor> fontManager::getAllFonts() {
	std::vector<fontManager::FontDescriptor> fonts;

	IDWriteFactory *factory = nullptr;

	HR(DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown **>(&factory)));

	// Get the system font collection.
	IDWriteFontCollection *collection = nullptr;
	HR(factory->GetSystemFontCollection(&collection));

	// Get the number of font families in the collection.
	const size_t familyCount = collection->GetFontFamilyCount();

	for (size_t i = 0; i < familyCount; ++i) {
		IDWriteFontFamily *family = nullptr;

		// Get the font family.
		HR(collection->GetFontFamily(i, &family));

		const size_t fontCount = family->GetFontCount();

		for (size_t j = 0; j < fontCount; ++j) {
			IDWriteFont *font = nullptr;

			// Get the Font Data
			HR(family->GetFont(j, &font));

			// append font to set if not duplicate
			resultFromFont(font, fonts);

			font->Release();
		}
		family->Release();
	}
	collection->Release();
	factory->Release();
	return fonts;
}

#elif __APPLE__

#elif LINUX

#endif

auto fontMatches(
	const fontManager::FontDescriptor &result,
	const fontManager::FontDescriptor &description) {
#define OPT_EQUAL_UTIL(value, desc, property, defaultValue) \
	(desc.property == defaultValue || desc.property == value.property)

#define EQUAL_UTIL(value, desc, property) desc.property == value.property
	return OPT_EQUAL_UTIL(result, description, postscriptName, "") &&
		   OPT_EQUAL_UTIL(result, description, family, "") &&
		   OPT_EQUAL_UTIL(result, description, style, "") &&
		   OPT_EQUAL_UTIL(
			   result,
			   description,
			   weight,
			   fontManager::FontWeight::Undefined) &&
		   OPT_EQUAL_UTIL(
			   result, description, width, fontManager::FontWidth::Undefined) &&
		   EQUAL_UTIL(result, description, italic) &&
		   EQUAL_UTIL(result, description, monospace) && true;

#undef OPT_EQUAL_UTIL
#undef EQUAL_UTIL
}

fontManager::FontDescriptor fontManager::findFont(
	const fontManager::FontDescriptor &desc) {
	auto allFonts = getAllFonts();
	auto result = std::find_if(
		allFonts.begin(), allFonts.end(), [&desc](const auto &res) {
			return fontMatches(res, desc);
		});
	if (result != allFonts.end()) {
		return *result;
	}
	return allFonts.at(0);
}