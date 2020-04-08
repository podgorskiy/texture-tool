//Copyright 2020 Stanislav Pidhorskyi
//
//Licensed under the Apache License, Version 2.0 (the "License");
//you may not use this file except in compliance with the License.
//You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
//Unless required by applicable law or agreed to in writing, software
//distributed under the License is distributed on an "AS IS" BASIS,
//WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//See the License for the specific language governing permissions and
//limitations under the License.
//==============================================================================


#include "common.h"
#include "coordinate_transform.h"

#include <PVRTexture.h>
#include <PVRTextureUtilities.h>

#include <pybind11/operators.h>
#include <pybind11/functional.h>
#include <pybind11/stl.h>
#include <memory>
#include <vector>



template<char C1Name, uint8_t C1Bits, char C2Name = 0, uint8_t C2Bits = 0, char C3Name = 0, uint8_t C3Bits = 0, char C4Name = 0, uint8_t C4Bits = 0>
class PixelType
{
public:
	enum : uint64_t
	{
		ID = (static_cast<uint64_t>(C1Name) + (static_cast<uint64_t>(C2Name) << 8u) +
		     (static_cast<uint64_t>(C3Name) << 16u) + (static_cast<uint64_t>(C4Name) << 24u) +
		     (static_cast<uint64_t>(C1Bits) << 32u) + (static_cast<uint64_t>(C2Bits) << 40u) +
			 (static_cast<uint64_t>(C3Bits) << 48u) + (static_cast<uint64_t>(C4Bits) << 56u))
	};
};

struct DecodedType
{
	bool compressed;
	std::vector<char> channel_names;
	std::vector<uint8_t> channel_sizes;
};

DecodedType DecodePixelType(uint64_t pixel_format)
{
	bool compressed = (pixel_format & 0xFFFFFFFF00000000) == 0;
	if (compressed)
	{
		return {compressed, {}, {}};
	}
	std::vector<char> channels;
	std::vector<uint8_t> sizes;
	for (int i = 0; i < 4; ++i)
	{
		char v = pixel_format & 0xffu;
		if (v != 0)
		{
			channels.push_back(v);
		}
		pixel_format = pixel_format >> 8u;
	}
	for (int i = 0; i < 4; ++i)
	{
		uint8_t v = pixel_format & 0xffu;
		if (v != 0)
		{
			sizes.push_back(v);
		}
		pixel_format = pixel_format >> 8u;
	}

	return {false, channels, sizes};
}

size_t GetChannelCount(uint64_t f)
{
	auto decoded = DecodePixelType(f);
	return decoded.channel_names.size();
}

template<char C1Name, char C2Name, char C3Name, char C4Name>
class FourCC
{
public:
	enum : uint32_t
	{
		Value = (static_cast<uint32_t>(C1Name) + (static_cast<uint32_t>(C2Name) << 8u) +
		      (static_cast<uint32_t>(C3Name) << 16u) + (static_cast<uint32_t>(C4Name) << 24u))
	};
};


inline uint64_t
getPixelType(char C1Name, uint8_t C1Bits, char C2Name = 0, uint8_t C2Bits = 0, char C3Name = 0, uint8_t C3Bits = 0,
             char C4Name = 0, uint8_t C4Bits = 0)
{
	return (static_cast<uint64_t>(C1Name) + (static_cast<uint64_t>(C2Name) << 8u) +
	        (static_cast<uint64_t>(C3Name) << 16u) + (static_cast<uint64_t>(C4Name) << 24u) +
	        (static_cast<uint64_t>(C1Bits) << 32u) + (static_cast<uint64_t>(C2Bits) << 40u) +
	        (static_cast<uint64_t>(C3Bits) << 48u) + (static_cast<uint64_t>(C4Bits) << 56u));
}

inline uint64_t parseType(const char* s)
{
	char buff[255];
	strncpy(buff, s, 255);

	int i = 0;
	while (buff[i] && i < 255)
	{
		buff[i] = (tolower(buff[i]));
		i++;
	}

	uint64_t type = 0;
	uint64_t mul = 1;
	for (i = 0; i < 4; i++)
	{
		if (buff[i] == 'r' || buff[i] == 'g' || buff[i] == 'b' || buff[i] == 'a' || buff[i] == 'l' ||
		    buff[i] == 'd')
		{
			type += buff[i] * mul;
			mul = mul << 8u;
		}
		else
		{
			break;
		}
	}
	if (i == 0)
	{
		throw runtime_error("Format should start from channel names, such as: r, g, b, a, l, d. Got %c", buff[0]);
	}
	for (int _i = i; _i < 4; _i++)
	{
		mul = mul << 8u;
	}

	if (!(buff[i] >= '0' && buff[i] <= '9'))
	{
		throw runtime_error("Unexpected symbol after channel names: %c. Expected digits", buff[i]);
	}

	int j = 0;
	const char* p = buff + i;
	for (; j < 4; j++)
	{
		if (*p >= '0' && *p <= '9')
		{
			int v = *p - '0';
			if (*p == '1' && *(p + 1) == '0')
			{
				v = 10;
				++p;
			}
			if (*p == '1' && *(p + 1) == '1')
			{
				v = 11;
				++p;
			}
			if (*p == '1' && *(p + 1) == '2')
			{
				v = 12;
				++p;
			}
			if (*p == '1' && *(p + 1) == '6')
			{
				v = 16;
				++p;
			}
			if (*p == '3' && *(p + 1) == '2')
			{
				v = 32;
				++p;
			}
			++p;
			type += v * mul;
			mul = mul << 8u;
		}
		else
		{
			break;
		}
	}

	if (*p != '\0')
	{
		throw runtime_error("Unexpected symbol at the end of the format: %c", *p);
	}

	if (i != j)
	{
		throw runtime_error("Number of channel names and sizes do not match, got %d and %d", i, j);
	}
	return type;
}


enum Format : uint64_t
{
	PVRTCI_2bpp_RGB,
	PVRTCI_2bpp_RGBA,
	PVRTCI_4bpp_RGB,
	PVRTCI_4bpp_RGBA,

	PVRTCII_2bpp,
	PVRTCII_4bpp,

	ETC1,

	DXT1,
	DXT2,
	DXT3,
	DXT4,
	DXT5,
	BC1 = DXT1,
	BC2 = DXT3,
	BC3 = DXT5,

	RGBG8888 = 20,
	GRGB8888,

	ETC2_RGB = 22,
	ETC2_RGBA,
	ETC2_RGB_A1,

	EAC_R11,
	EAC_RG11,

	ASTC_4x4,
	ASTC_5x4,
	ASTC_5x5,
	ASTC_6x5,
	ASTC_6x6,
	ASTC_8x5,
	ASTC_8x6,
	ASTC_8x8,
	ASTC_10x5,
	ASTC_10x6,
	ASTC_10x8,
	ASTC_10x10,
	ASTC_12x10,
	ASTC_12x12,

	RGBA8888 = PixelType<'r', 8, 'g', 8, 'b', 8, 'a', 8>::ID,
	RGBA1010102 = PixelType<'r', 10, 'g', 10, 'b', 10, 'a', 2>::ID,
	RGBA4444 = PixelType<'r', 4, 'g', 4, 'b', 4, 'a', 4>::ID,
	RGBA5551 = PixelType<'r', 5, 'g', 5, 'b', 5, 'a', 1>::ID,
	BGRA8888 = PixelType<'b', 8, 'g', 8, 'r', 8, 'a', 8>::ID,
	RGBA16161616 = PixelType<'r', 16, 'g', 16, 'b', 16, 'a', 16>::ID,
	RGBA32323232 = PixelType<'r', 32, 'g', 32, 'b', 32, 'a', 32>::ID,

	RGB888 = PixelType<'r', 8, 'g', 8, 'b', 8>::ID,
	RGB161616 = PixelType<'r', 16, 'g', 16, 'b', 16>::ID,
	RGB565 = PixelType<'r', 5, 'g', 6, 'b', 5>::ID,
	RGB323232 = PixelType<'r', 32, 'g', 32, 'b', 32>::ID,
	BGR101111 = PixelType<'b', 10, 'g', 11, 'r', 11>::ID,

	RG88 = PixelType<'r', 8, 'g', 8>::ID,
	LA88 = PixelType<'l', 8, 'a', 8>::ID,
	RG1616 = PixelType<'r', 16, 'g', 16>::ID,
	RG3232 = PixelType<'r', 32, 'g', 32>::ID,

	R8 = PixelType<'r', 8>::ID,
	A8 = PixelType<'a', 8>::ID,
	L8 = PixelType<'l', 8>::ID,
	R16 = PixelType<'r', 16>::ID,
	R32 = PixelType<'r', 32>::ID,
};

uint64_t getType(int channel_count, int bits)
{
	std::string part_a;
	std::string part_b;
	const char* names = "rgba";
	char bitsstr[32];
	sprintf(bitsstr, "%d", bits);
	for (int i = 0; i < channel_count; ++i)
	{
		part_a += names[i];
		part_b += bitsstr;
	}
	return parseType((part_a + part_b).c_str());
}

template<typename T>
pvrtexture::CPVRTexture* from_numpy(py::array_t<T, py::array::c_style> array, EPVRTColourSpace eColourSpace, bool normed, bool premultiplied)
{
	py::buffer_info buf = array.request();
	if (buf.ndim != 3 && buf.ndim != 4)
		throw std::runtime_error(
				"Number of dimensions must be 3 or 4 for 2D and 3D textures. Shape should be [H, W, C] or [D, H, W, C]. If number of channels is one, dimension should not be reduced");
	if (buf.shape.back() < 1 or buf.shape.back() > 4)
		throw std::runtime_error("Wrong number of channels. Number of channels should be one of: 1, 2, 3, 4");
	uint64_t ptype = getType(buf.shape.back(), sizeof(T) * 8);
	uint32_t width = buf.shape[buf.ndim - 2];
	uint32_t height = buf.shape[buf.ndim - 3];
	uint32_t depth = 1;
	if (buf.ndim == 4)
		depth = buf.shape[0];
	if (width > 65535 || height > 65535  || depth > 65535)
		throw runtime_error("Wrong number texture size. Got %dx%dx%d", width, height, depth);
	EPVRTVariableType channelType = (EPVRTVariableType)0;
	switch (sizeof(T))
	{
		case 1: channelType = ePVRTVarTypeUnsignedByteNorm; break;
		case 2: channelType = ePVRTVarTypeUnsignedShortNorm; break;
		case 4: channelType = ePVRTVarTypeUnsignedIntegerNorm; break;
		default:
			throw runtime_error("Wrong size of type, got %d bytes, but expected 1, 2, or 4", sizeof(T));
	}
	if (std::is_signed<T>::value)
		channelType = (EPVRTVariableType)((uint8_t)channelType | 0b01u);
	if (!normed)
		channelType = (EPVRTVariableType)((uint8_t)channelType | 0b10u);
	if (std::is_floating_point<T>::value)
		channelType = ePVRTVarTypeFloat;

	pvrtexture::CPVRTextureHeader header(ptype, height, width, depth, 1, 1, 1, eColourSpace, channelType, premultiplied);

	auto pvr = new pvrtexture::CPVRTexture(header, buf.ptr);
	pvr->setOrientation((EPVRTOrientation)(ePVRTOrientRight | ePVRTOrientDown));
	return pvr;
}


pvrtexture::CPVRTexture* new_texture(Format pixel_type, int width, int height, int depth, int num_mipmap, int num_array, int num_faces, EPVRTColourSpace eColourSpace, EPVRTVariableType channelType, bool premultiplied)
{
	pvrtexture::CPVRTextureHeader header(pixel_type, height, width, depth, num_mipmap, num_array, num_faces, eColourSpace, channelType, premultiplied);

	auto pvr = new pvrtexture::CPVRTexture(header);
	pvr->setOrientation((EPVRTOrientation)(ePVRTOrientRight | ePVRTOrientDown));
	return pvr;
}


struct TexView
{
	void *ptr;
	size_t itemsize;
	std::string format;
	size_t ndim;
	std::vector<int> shape;
	std::vector<int> strides;
};


PYBIND11_MODULE(_pypvrtex, m)
{
	m.doc() = "_pypvrtex";

	py::enum_<pvrtexture::ECompressorQuality>(m, "Quality")
			.value("Fastest", pvrtexture::ePVRTCFastest)
			.value("Fast", pvrtexture::ePVRTCFast)
			.value("Normal", pvrtexture::ePVRTCNormal)
			.value("High", pvrtexture::ePVRTCHigh)
			.value("Best", pvrtexture::ePVRTCBest)
			.export_values();

	py::enum_<pvrtexture::EResizeMode>(m, "ResizeMode")
			.value("Nearest", pvrtexture::eResizeNearest)
			.value("Linear", pvrtexture::eResizeLinear)
			.value("Cubic", pvrtexture::eResizeCubic)
			.export_values();

	py::enum_<EPVRTColourSpace>(m, "ColourSpace")
			.value("lRGB", ePVRTCSpacelRGB)
			.value("sRGB", ePVRTCSpacesRGB)
			.export_values();

	py::enum_<EPVRTVariableType>(m, "ChannelType")
			.value("UnsignedByteNorm", ePVRTVarTypeUnsignedByteNorm)
			.value("SignedByteNorm", ePVRTVarTypeSignedByteNorm)
			.value("UnsignedByte", ePVRTVarTypeUnsignedByte)
			.value("SignedByte", ePVRTVarTypeSignedByte)
			.value("UnsignedShortNorm", ePVRTVarTypeUnsignedShortNorm)
			.value("SignedShortNorm", ePVRTVarTypeSignedShortNorm)
			.value("UnsignedShort", ePVRTVarTypeUnsignedShort)
			.value("SignedShort", ePVRTVarTypeSignedShort)
			.value("UnsignedIntegerNorm", ePVRTVarTypeUnsignedIntegerNorm)
			.value("SignedIntegerNorm", ePVRTVarTypeSignedIntegerNorm)
			.value("UnsignedInteger", ePVRTVarTypeUnsignedInteger)
			.value("SignedInteger", ePVRTVarTypeSignedInteger)
			.value("Float", ePVRTVarTypeFloat)
			.value("SignedFloat", ePVRTVarTypeSignedFloat)
			.value("UnsignedFloat", ePVRTVarTypeUnsignedFloat)
			.export_values();

	py::enum_<EPVRTAxis>(m, "Axis")
			.value("x", ePVRTAxisX)
			.value("y", ePVRTAxisY)
			.value("z", ePVRTAxisZ)
			.export_values();

	py::enum_<EPVRTOrientation>(m, "Orientation", py::arithmetic())
			.value("not_flipped", ePVRTOrientRight)
			.value("flipped_horizontal", ePVRTOrientLeft)
			.value("flipped_vertical", ePVRTOrientUp)
			.value("flipped_depth", ePVRTOrientOut)
			.export_values();

	py::enum_<pvrtexture::ELegacyApi>(m, "LegacyApi")
			.value("OGLES", pvrtexture::eOGLES)
			.value("OGLES2", pvrtexture::eOGLES2)
			.value("D3DM", pvrtexture::eD3DM)
			.value("OGL", pvrtexture::eOGL)
			.value("DX9", pvrtexture::eDX9)
			.value("DX10", pvrtexture::eDX10)
			.value("OVG", pvrtexture::eOVG)
			.value("MGL", pvrtexture::eMGL)
			.export_values();

	py::enum_<Format>(m, "PixelFormat")
			.value("PVRTCI_2bpp_RGB", PVRTCI_2bpp_RGB)
			.value("PVRTCI_2bpp_RGBA", PVRTCI_2bpp_RGBA)
			.value("PVRTCI_4bpp_RGB", PVRTCI_4bpp_RGB)
			.value("PVRTCI_4bpp_RGBA", PVRTCI_4bpp_RGBA)
			.value("PVRTCII_2bpp", PVRTCII_2bpp)
			.value("PVRTCII_4bpp", PVRTCII_4bpp)

			.value("ETC1", ETC1)
			.value("DXT1", DXT1)
			.value("DXT2", DXT2)
			.value("DXT3", DXT3)
			.value("DXT4", DXT4)
			.value("DXT5", DXT5)

			.value("RGBG8888", RGBG8888)
			.value("GRGB8888", GRGB8888)

			.value("ETC2_RGB", ETC2_RGB)
			.value("ETC2_RGBA", ETC2_RGBA)
			.value("EAC_R11", EAC_R11)
			.value("EAC_RG11", EAC_RG11)

			.value("RGBA8888", RGBA8888)
			.value("RGBA1010102", RGBA1010102)
			.value("RGBA4444", RGBA4444)
			.value("RGBA5551", RGBA5551)
			.value("BGRA8888", BGRA8888)
			.value("RGBA16161616", RGBA16161616)
			.value("RGBA32323232", RGBA32323232)

			.value("RGB888", RGB888)
			.value("RGB161616", RGB161616)
			.value("RGB565", RGB565)
			.value("RGB323232", RGB323232)
			.value("BGR101111", BGR101111)

			.value("RG88", RG88)
			.value("LA88", LA88)
			.value("RG1616", RG1616)
			.value("RG3232", RG3232)

			.value("R8", R8)
			.value("A8", A8)
			.value("L8", L8)
			.value("R16", R16)
			.value("R32", R32)
			.export_values();

	py::class_<pvrtexture::CPVRTexture>(m, "PVRTexture")
			.def(py::init<>())
			.def_property("pixel_format", [](pvrtexture::CPVRTexture& self){
				auto pixel_format = self.getPixelType();
				return (Format)pixel_format.PixelTypeID;
			}, [](pvrtexture::CPVRTexture& self, Format format){
				self.setPixelFormat(format);
			})
			.def_property_readonly("bpp", &pvrtexture::CPVRTexture::getBitsPerPixel)
			.def_property("colour_space", &pvrtexture::CPVRTexture::getColourSpace, &pvrtexture::CPVRTexture::setColourSpace)
			.def_property("channel_type", &pvrtexture::CPVRTexture::getChannelType, &pvrtexture::CPVRTexture::setChannelType)
			.def("get_width", &pvrtexture::CPVRTexture::getWidth, py::arg("mipmap")=0)
			.def("get_height", &pvrtexture::CPVRTexture::getHeight, py::arg("mipmap")=0)
			.def("get_depth", &pvrtexture::CPVRTexture::getDepth, py::arg("mipmap")=0)
			.def_property("num_array", &pvrtexture::CPVRTexture::getNumArrayMembers, &pvrtexture::CPVRTexture::setNumArrayMembers)
			.def_property("num_faces", &pvrtexture::CPVRTexture::getNumFaces, &pvrtexture::CPVRTexture::setNumFaces)
			.def_property("num_mip_levels", &pvrtexture::CPVRTexture::getNumMIPLevels, &pvrtexture::CPVRTexture::setNumMIPLevels)
			.def_property_readonly("is_compressed", &pvrtexture::CPVRTexture::isFileCompressed)

			.def("get_orientation", [](pvrtexture::CPVRTexture& self, EPVRTAxis axis){
				self.m_MetaData[FourCC<'P', 'V', 'R', 3>::Value][3].DevFOURCC = FourCC<'P', 'V', 'R', 3>::Value;
				self.m_MetaData[FourCC<'P', 'V', 'R', 3>::Value][3].u32Key = 3;
				self.m_MetaData[FourCC<'P', 'V', 'R', 3>::Value][3].u32DataSize = 3;
				if (self.m_MetaData[FourCC<'P', 'V', 'R', 3>::Value][3].Data == nullptr)
				{
					self.m_MetaData[FourCC<'P', 'V', 'R', 3>::Value][3].Data = new uint8_t[3];
					self.m_MetaData[FourCC<'P', 'V', 'R', 3>::Value][3].Data[0] = 0;
					self.m_MetaData[FourCC<'P', 'V', 'R', 3>::Value][3].Data[1] = 0;
					self.m_MetaData[FourCC<'P', 'V', 'R', 3>::Value][3].Data[2] = 0;
				}
				return self.m_MetaData[FourCC<'P', 'V', 'R', 3>::Value][3].Data[axis];
			})
			.def("set_orientation", [](pvrtexture::CPVRTexture& self, EPVRTAxis axis, bool orientation)
			{
				self.m_MetaData[FourCC<'P', 'V', 'R', 3>::Value][3].DevFOURCC = FourCC<'P', 'V', 'R', 3>::Value;
				self.m_MetaData[FourCC<'P', 'V', 'R', 3>::Value][3].u32Key = 3;
				self.m_MetaData[FourCC<'P', 'V', 'R', 3>::Value][3].u32DataSize = 3;
				if (self.m_MetaData[FourCC<'P', 'V', 'R', 3>::Value][3].Data == nullptr)
				{
					self.m_MetaData[FourCC<'P', 'V', 'R', 3>::Value][3].Data = new uint8_t[3];
					self.m_MetaData[FourCC<'P', 'V', 'R', 3>::Value][3].Data[0] = 0;
					self.m_MetaData[FourCC<'P', 'V', 'R', 3>::Value][3].Data[1] = 0;
					self.m_MetaData[FourCC<'P', 'V', 'R', 3>::Value][3].Data[2] = 0;
				}
				self.m_MetaData[FourCC<'P', 'V', 'R', 3>::Value][3].Data[axis] = ((orientation ? 1u : 0u) << axis);
			})

			.def_property_readonly("d3d_format", &pvrtexture::CPVRTexture::getD3DFormat)
			.def_property_readonly("ogl_format", [](pvrtexture::CPVRTexture& self){
				uint32_t internalFormat;
				uint32_t format;
				uint32_t type;
				self.getOGLFormat(internalFormat, format, type);
				return std::make_tuple(internalFormat, format, type);
			})
			.def_property_readonly("ogles_format", [](pvrtexture::CPVRTexture& self){
				uint32_t internalFormat;
				uint32_t format;
				uint32_t type;
				self.getOGLESFormat(internalFormat, format, type);
				return std::make_tuple(internalFormat, format, type);
			})

			.def("save_pvr", [](pvrtexture::CPVRTexture& self, const char* filename){
				FILE* file = fopen(filename, "wb");
				self.privateSavePVRFile(file);
				fclose(file);
			})
			.def("save_dds", [](pvrtexture::CPVRTexture& self, const char* filename){
				FILE* file = fopen(filename, "wb");
				self.privateSaveDDSFile(file);
				fclose(file);
			})
//			.def("save_ktx", [](pvrtexture::CPVRTexture& self, const char* filename){
//				FILE* file = fopen(filename, "wb");
//				self.privateSaveKTXFile(file);
//				fclose(file);
//			})
			.def("save_legacy_pvr", [](pvrtexture::CPVRTexture& self, const char* filename, pvrtexture::ELegacyApi api){
				FILE* file = fopen(filename, "wb");
				self.privateSaveLegacyPVRFile(file, api);
				fclose(file);
			})
			.def("open_view", [](pvrtexture::CPVRTexture& self, int mipmap, int face)
			{
				void* ptr = self.getDataPtr(mipmap, 0, face);
				size_t size = self.getDataSize(mipmap, 0, face);
				size_t width = self.getWidth(mipmap);
				size_t height = self.getHeight(mipmap);
				size_t depth = self.getDepth(mipmap);
				std::vector<int> shape;
				std::vector<int> strides;
				int channel_count = GetChannelCount(self.getPixelType().PixelTypeID);
				if (depth > 1)
				{
					shape.push_back(depth);
					strides.push_back(width * height * self.getBitsPerPixel() / 8);
				}
				shape.push_back(height);
				strides.push_back(width * self.getBitsPerPixel() / 8);
				shape.push_back(width);
				strides.push_back(self.getBitsPerPixel() / 8);
				shape.push_back(channel_count);
				strides.push_back((self.getBitsPerPixel() / 8) / channel_count);
				std::string dtype = "";
				size_t item_size = (self.getBitsPerPixel() / 8) / channel_count;
				switch(self.getChannelType())
				{
                    case ePVRTVarTypeUnsignedByteNorm: dtype = py::format_descriptor<uint8_t>::format(); break;
                    case ePVRTVarTypeSignedByteNorm: dtype = py::format_descriptor<int8_t>::format(); break;
                    case ePVRTVarTypeUnsignedByte: dtype = py::format_descriptor<uint8_t>::format(); break;
                    case  ePVRTVarTypeSignedByte: dtype = py::format_descriptor<int8_t>::format(); break;
                    case ePVRTVarTypeUnsignedShortNorm: dtype = py::format_descriptor<uint16_t>::format(); break;
                    case ePVRTVarTypeSignedShortNorm: dtype = py::format_descriptor<int16_t>::format(); break;
                    case ePVRTVarTypeUnsignedShort: dtype = py::format_descriptor<uint16_t>::format(); break;
                    case ePVRTVarTypeSignedShort: dtype = py::format_descriptor<int16_t>::format(); break;
                    case ePVRTVarTypeUnsignedIntegerNorm: dtype = py::format_descriptor<uint32_t>::format(); break;
                    case ePVRTVarTypeSignedIntegerNorm: dtype = py::format_descriptor<int32_t>::format(); break;
                    case ePVRTVarTypeUnsignedInteger: dtype = py::format_descriptor<uint32_t>::format(); break;
                    case ePVRTVarTypeSignedInteger: dtype = py::format_descriptor<int32_t>::format(); break;
                    case ePVRTVarTypeFloat:
                    case ePVRTVarTypeUnsignedFloat:
                        dtype = py::format_descriptor<float>::format(); break;
				}
				return TexView{
						ptr,
						item_size,
						dtype,
						shape.size(),
						shape,
						strides
				};
			})
			;

	py::class_<TexView>(m, "TexView", py::buffer_protocol())
			.def_buffer([](TexView& m) -> py::buffer_info
			            {
				            return py::buffer_info(
						            m.ptr,
						            m.itemsize,
						            m.format,
						            m.ndim,
						            m.shape,
						            m.strides
				            );
			            });


	m.def("load_pvr", [](const char* filename)
	{
		auto pvr = new pvrtexture::CPVRTexture(filename);
		return pvr;
	});

	m.def("load_dds", [](const char* filename)
	{
		FILE* file = fopen(filename, "rb");
		auto pvr = new pvrtexture::CPVRTexture();
		pvr->privateLoadDDSFile(file);
		fclose(file);
		return pvr;
	});

	m.def("load_ktx", [](const char* filename)
	{
		FILE* file = fopen(filename, "rb");
		auto pvr = new pvrtexture::CPVRTexture();
		pvr->privateLoadKTXFile(file);
		fclose(file);
		return pvr;
	});

	m.def("check_if_pvr", [](const char* filename)
	{
		FILE* file = fopen(filename, "rb");
		uint32_t w;
		fread(&w, 4, 1, file);
		fclose(file);
		return w == 0x03525650 || w == 0x50565203;
	});

	m.def("check_if_dds", [](const char* filename)
	{
		FILE* file = fopen(filename, "rb");
		uint32_t w;
		fread(&w, 4, 1, file);
		fclose(file);
		return w == 0x20534444;
	});

	m.def("from_numpy", from_numpy<uint8_t>, py::arg("array"), py::arg("eColourSpace") = ePVRTCSpacelRGB, py::arg("normed") = true, py::arg("premultiplied") = false);
	m.def("from_numpy", from_numpy<int8_t>, py::arg("array"), py::arg("eColourSpace") = ePVRTCSpacelRGB, py::arg("normed") = true, py::arg("premultiplied") = false);
	m.def("from_numpy", from_numpy<uint16_t>, py::arg("array"), py::arg("eColourSpace") = ePVRTCSpacelRGB, py::arg("normed") = true, py::arg("premultiplied") = false);
	m.def("from_numpy", from_numpy<int16_t>, py::arg("array"), py::arg("eColourSpace") = ePVRTCSpacelRGB, py::arg("normed") = true, py::arg("premultiplied") = false);
	m.def("from_numpy", from_numpy<uint32_t>, py::arg("array"), py::arg("eColourSpace") = ePVRTCSpacelRGB, py::arg("normed") = true, py::arg("premultiplied") = false);
	m.def("from_numpy", from_numpy<int32_t>, py::arg("array"), py::arg("eColourSpace") = ePVRTCSpacelRGB, py::arg("normed") = true, py::arg("premultiplied") = false);
	m.def("from_numpy", from_numpy<float>, py::arg("array"), py::arg("eColourSpace") = ePVRTCSpacelRGB, py::arg("normed") = true, py::arg("premultiplied") = false);

	m.def("copy", [](pvrtexture::CPVRTexture& texture)
	{
		return new pvrtexture::CPVRTexture(texture);
	});

	m.def("inplace_resize", pvrtexture::Resize, py::arg("texture"), py::arg("width"), py::arg("height"), py::arg("depth") = 1, py::arg("resize_mode") = pvrtexture::eResizeCubic);
	m.def("inplace_resize_canvas", pvrtexture::ResizeCanvas, py::arg("texture"), py::arg("width"), py::arg("height"), py::arg("depth"), py::arg("x_offset"), py::arg("y_offset"), py::arg("z_offset"));
	m.def("inplace_rotate90", pvrtexture::Rotate90, py::arg("texture"), py::arg("axis"), py::arg("forward"));
	m.def("inplace_flip", pvrtexture::Flip, py::arg("texture"), py::arg("axis"));
	m.def("inplace_premultiply_alpha", pvrtexture::PreMultiplyAlpha, py::arg("texture"));
	m.def("inplace_bleed", pvrtexture::Bleed, py::arg("texture"));
	m.def("inplace_generate_mipmaps", pvrtexture::GenerateMIPMaps, py::arg("texture"), py::arg("filter"), py::arg("mipmaps"));
	m.def("inplace_colour_mipmaps", pvrtexture::ColourMIPMaps, py::arg("texture"));
	m.def("inplace_transcode", [](pvrtexture::CPVRTexture& texture, Format format, const EPVRTVariableType channel_type, const EPVRTColourSpace colour_space, const pvrtexture::ECompressorQuality quality, const bool dither)
	{
		return pvrtexture::Transcode(texture, format, channel_type, colour_space, quality, dither);
	}, py::arg("texture"), py::arg("format"), py::arg("channel_type"), py::arg("colour_space"), py::arg("quality")=pvrtexture::ePVRTCNormal, py::arg("dither")=false);
	m.def("format_from_string", [](const char* str){
		uint64_t format = parseType(str);
		return (Format)format;
	});
	m.def("new_texture", new_texture);
}
