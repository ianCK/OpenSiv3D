﻿//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2019 Ryo Suzuki
//	Copyright (c) 2016-2019 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include "CPrint.hpp"

namespace s3d
{
	ISiv3DPrint* ISiv3DPrint::Create()
	{
		return new CPrint;
	}
}

