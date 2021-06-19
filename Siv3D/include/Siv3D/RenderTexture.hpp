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
# include "Texture.hpp"

namespace s3d
{
	class RenderTexture : public Texture
	{
	public:

		SIV3D_NODISCARD_CXX20
		RenderTexture();

		SIV3D_NODISCARD_CXX20
		RenderTexture(uint32 width, uint32 height, const ColorF& color = ColorF{ 0.0, 1.0 }, const TextureFormat& format = TextureFormat::R8G8B8A8_Unorm);

		SIV3D_NODISCARD_CXX20
		explicit RenderTexture(const Size& size, const ColorF& color = ColorF{ 0.0, 1.0 }, const TextureFormat& format = TextureFormat::R8G8B8A8_Unorm);

		SIV3D_NODISCARD_CXX20
		RenderTexture(uint32 width, uint32 height, const TextureFormat& format);

		SIV3D_NODISCARD_CXX20
		RenderTexture(const Size& size, const TextureFormat& format);

		SIV3D_NODISCARD_CXX20
		RenderTexture(const Image& image);

		SIV3D_NODISCARD_CXX20
		RenderTexture(const Grid<float>& image);

		SIV3D_NODISCARD_CXX20
		RenderTexture(const Grid<Float2>& image);

		SIV3D_NODISCARD_CXX20
		RenderTexture(const Grid<Float4>& image);

		virtual ~RenderTexture();

		void clear(const ColorF& color) const;

		// TextureFormat::R8G8B8A8_Unorm のみサポート
		void readAsImage(Image& image) const;

		// TextureFormat::R32_Float のみサポート
		void read(Grid<float>& image) const;

		// TextureFormat::R32G32_Float のみサポート
		void read(Grid<Float2>& image) const;

		// TextureFormat::R32G32B32A32_Float のみサポート
		void read(Grid<Float4>& image) const;

	protected:

		struct MSRender {};

		SIV3D_NODISCARD_CXX20
		RenderTexture(MSRender, const Size& size, const TextureFormat& format);
	};
}
