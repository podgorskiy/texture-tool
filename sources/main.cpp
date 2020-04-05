/*
 * Copyright 2019-2020 Stanislav Pidhorskyi. All rights reserved.
 * License: https://raw.githubusercontent.com/podgorskiy/bimpy/master/LICENSE.txt
 */

#include "common.h"

#include <PVRTexture.h>
#include <PVRTextureUtilities.h>

#include <pybind11/operators.h>
#include <pybind11/functional.h>
#include <pybind11/stl.h>
#include <memory>



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


PYBIND11_MODULE(_pypvrtex, m)
{
	m.doc() = "_pypvrtex";

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
			.value("left", ePVRTOrientLeft)
			.value("right", ePVRTOrientRight)
			.value("up", ePVRTOrientUp)
			.value("down", ePVRTOrientDown)
			.value("in", ePVRTOrientIn)
			.value("out", ePVRTOrientOut)
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
			.def_property_readonly("pixel_format", [](pvrtexture::CPVRTexture& self){
				auto pixel_format = self.getPixelType();
				return (Format)pixel_format.PixelTypeID;
			})

			.def_property_readonly("bpp", &pvrtexture::CPVRTexture::getBitsPerPixel)
			.def_property_readonly("colour_space", &pvrtexture::CPVRTexture::getColourSpace)
			.def_property_readonly("channel_type", &pvrtexture::CPVRTexture::getChannelType)
			.def_property_readonly("width", &pvrtexture::CPVRTexture::getWidth)
			.def_property_readonly("height", &pvrtexture::CPVRTexture::getHeight)
			.def_property_readonly("depth", &pvrtexture::CPVRTexture::getDepth)
			.def_property_readonly("num_array", &pvrtexture::CPVRTexture::getNumArrayMembers)
			.def_property_readonly("num_faces", &pvrtexture::CPVRTexture::getNumFaces)
			.def_property_readonly("num_mip_levels", &pvrtexture::CPVRTexture::getNumMIPLevels)
			.def_property_readonly("is_compressed", &pvrtexture::CPVRTexture::isFileCompressed)

			.def("get_orientation", &pvrtexture::CPVRTexture::getOrientation)

			.def_property_readonly("get_d3d_format", &pvrtexture::CPVRTexture::getD3DFormat)
			.def_property_readonly("get_ogl_format", [](pvrtexture::CPVRTexture& self){
				uint32_t internalFormat;
				uint32_t format;
				uint32_t type;
				self.getOGLFormat(internalFormat, format, type);
				return std::make_tuple(internalFormat, format, type);
			})
			.def_property_readonly("get_ogles_format", [](pvrtexture::CPVRTexture& self){
				uint32_t internalFormat;
				uint32_t format;
				uint32_t type;
				self.getOGLESFormat(internalFormat, format, type);
				return std::make_tuple(internalFormat, format, type);
			})

			.def("save", [](pvrtexture::CPVRTexture& self, const char* filename){
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
			;

	m.def("load", [](const char* filename)
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
}
