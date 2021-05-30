﻿//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2021 Ryo Suzuki
//	Copyright (c) 2016-2021 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include "Common.hpp"
# include "FormatData.hpp"

SIV3D_DISABLE_MSVC_WARNINGS_PUSH(4459)
# if SIV3D_INTRINSIC(SSE)
    # define _XM_SSE4_INTRINSICS_
# endif

# include <ThirdParty/DirectXMath/DirectXMath.h>
SIV3D_DISABLE_MSVC_WARNINGS_POP()

 # if !SIV3D_PLATFORM(WINDOWS)
    # undef __in
    # undef __out
# endif

namespace s3d
{
#if defined(_XM_SSE_INTRINSICS_) && !defined(_XM_NO_INTRINSICS_)

    using aligned_float4 = __m128;

#elif defined(_XM_ARM_NEON_INTRINSICS_) && !defined(_XM_NO_INTRINSICS_)
    
    using aligned_float4 = float32x4_t;

#else
    
    using aligned_float4 = __vector4;

#endif

# define SIV3D_VECTOR_CALL XM_CALLCONV

    void Formatter(FormatData& formatData, aligned_float4 value);
}