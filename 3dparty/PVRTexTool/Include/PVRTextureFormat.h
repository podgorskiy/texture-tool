#ifndef _PVRT_PIXEL_FORMAT_H
#define _PVRT_PIXEL_FORMAT_H

#include "PVRTextureDefines.h"

namespace pvrtexture
{
	//Channel Names
	enum EChannelName
	{
		eNoChannel,
		eRed,
		eGreen,
		eBlue,
		eAlpha,
		eLuminance,
		eIntensity,
		eUnspecified,
		eNumChannels
	};

	//PixelType union
	union PVR_DLL PixelType
	{
		/*!***********************************************************************
		 @Function		PixelType
		 @Return		A new PixelType
		 @Description	Creates an empty pixeltype.
		*************************************************************************/
		PixelType() {};

		/*!***********************************************************************
		 @Function		PixelType
		 @Input			Type
		 @Return		A new PixelType
		 @Description	Initialises a new pixel type from a 64 bit integer value.
		*************************************************************************/
		PixelType(uint64 Type): PixelTypeID(Type) {};

		/*!***********************************************************************
		 @Function		PixelType
		 @Input			C1Name
		 @Input			C2Name
		 @Input			C3Name
		 @Input			C4Name
		 @Input			C1Bits
		 @Input			C2Bits
		 @Input			C3Bits
		 @Input			C4Bits
		 @Return		A new PixelType
		 @Description	Takes up to 4 characters (CnName) and 4 values (CnBits)
						to create a new PixelType. Any unused channels should be set to 0.
						For example: PixelType('r','g','b',0,8,8,8,0);
		*************************************************************************/

		struct PVR_DLL LowHigh
		{
			uint32	Low;
			uint32	High;
		} Part;

		uint64	PixelTypeID;
		uint8	PixelTypeChar[8];
	};
}

#endif
