#include "CAEFile.h"

#include <array>
#include <string>

const std::string CAEFile::getKeyErrorMsg() const {
	return "Invalid CAE key\nCAE Key:\n";
}

const std::array<uint32_t, 44> CAEFile::getKeyParity() const {
	return {{1, 0, 1, 0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 0, 0}};
}

const std::array<uint32_t, 44> CAEFile::getBuiltinKey() const {
	return {{
		0x477fa6a2,
		0xfb9b5e2b,
		0x77bcac57,
		0x2d7cef8c,
		0x69825182,
		0xfa231194,
		0x96ee6d48,
		0x520a9b74,
		0x0619cb60,
		0x95918dfb,
		0x1c829771,
		0x03f6655c,
		0xbba3b302,
		0xf3cbcc66,
		0xb42e9ac7,
		0x417b37dd,
		0x34854b8c,
		0xf95a9547,
		0x7950401e,
		0xc3271f83,
		0x0e7c9a6e,
		0xcfa7f799,
		0x616d9d05,
		0x200ac08f,
		0x7cdb242f,
		0x30d3bc5e,
		0x2983cc29,
		0x9da249c9,
		0x7509f015,
		0x6632580e,
		0x83247f04,
		0x6525ed71,
		0x02fa242a,
		0x47b12928,
		0x7ed51b5d,
		0xf69cd51b,
		0x66f24c77,
		0x042856b9,
		0x00e37970,
		0x88b6624d,
		0x6826cd76,
		0xd2a4c9fe,
		0x2eff487a,
		0x09648fae
	}};
}
