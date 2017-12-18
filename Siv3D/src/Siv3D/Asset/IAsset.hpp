﻿//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2017 Ryo Suzuki
//	Copyright (c) 2016-2017 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include <memory>
# include <Siv3D/Fwd.hpp>

namespace s3d
{
	enum class AssetType
	{
		Texture,
	};

	class ISiv3DAsset
	{
	public:

		static ISiv3DAsset* Create();

		virtual ~ISiv3DAsset() = default;

		virtual bool init() = 0;

		virtual void update() = 0;

		virtual bool registerAsset(AssetType assetType, const String& name, std::unique_ptr<IAsset>&& asset) = 0;

		virtual IAsset* getAsset(AssetType assetType, const String& name) = 0;

		virtual bool isReady(AssetType assetType, const String& name) = 0;
	};
}
