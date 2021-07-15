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
# include <Siv3D/Common.hpp>
# include <Siv3D/VertexShader.hpp>
# include <Siv3D/PixelShader.hpp>
# include <Siv3D/ConstantBuffer.hpp>
# include <Siv3D/Renderer3D/IRenderer3D.hpp>
# include <Siv3D/Renderer3D/Renderer3DCommon.hpp>
# include "GLES3Renderer3DCommand.hpp"

namespace s3d
{
	class CRenderer_GLES3;
	class CShader_GLES3;
	class CTexture_GLES3;
	class CMesh_GLES3;

	struct GLES3StandardVS3D
	{
		VertexShader forward;

		VertexShader::IDType forwardID;

		bool setup()
		{
			const bool result = !!forward;

			forwardID = forward.id();

			return result;
		}
	};

	struct GLES3StandardPS3D
	{
		PixelShader forwardShape;
		PixelShader forwardTexture;

		PixelShader::IDType forwardShapeID;
		PixelShader::IDType forwardTextureID;

		bool setup()
		{
			const bool result = forwardShape && forwardTexture;

			forwardShapeID = forwardShape.id();
			forwardTextureID = forwardTexture.id();

			return result;
		}
	};

	class CRenderer3D_GLES3 final : public ISiv3DRenderer3D
	{
	public:

		CRenderer3D_GLES3();

		~CRenderer3D_GLES3() override;

		void init() override;

		const Renderer3DStat& getStat() const override;

		void addMesh(uint32 startIndex, uint32 indexCount, const Mesh& mesh, const Mat4x4& mat, const Float4& color) override;

		void addTexturedMesh(uint32 startIndex, uint32 indexCount, const Mesh& mesh, const Texture& texture, const Mat4x4& mat, const Float4& color) override;


		BlendState getBlendState() const override;

		RasterizerState getRasterizerState() const override;

		DepthStencilState getDepthStencilState() const override;

		SamplerState getSamplerState(ShaderStage shaderStage, uint32 slot) const override;

		void setBlendState(const BlendState& state) override;

		void setRasterizerState(const RasterizerState& state) override;

		void setDepthStencilState(const DepthStencilState& state) override;

		void setSamplerState(ShaderStage shaderStage, uint32 slot, const SamplerState& state) override;


		void setScissorRect(const Rect& rect) override;

		Rect getScissorRect() const override;

		void setViewport(const Optional<Rect>& viewport) override;

		Optional<Rect> getViewport() const override;


		Optional<VertexShader> getCustomVS() const override;

		Optional<PixelShader> getCustomPS() const override;

		void setCustomVS(const Optional<VertexShader>& vs) override;

		void setCustomPS(const Optional<PixelShader>& ps) override;


		const Mat4x4& getCameraTransform() const override;

		void setCameraTransform(const Mat4x4& matrix) override;


		void setVSTexture(uint32 slot, const Optional<Texture>& texture) override;

		void setPSTexture(uint32 slot, const Optional<Texture>& texture) override;


		void setRenderTarget(const Optional<RenderTexture>& rt) override;

		Optional<RenderTexture> getRenderTarget() const override;


		void setConstantBuffer(ShaderStage stage, uint32 slot, const ConstantBufferBase& buffer, const float* data, uint32 num_vectors) override;


		void flush() override;

	private:

		CRenderer_GLES3* pRenderer = nullptr;
		CShader_GLES3* pShader = nullptr;
		CTexture_GLES3* pTexture = nullptr;
		CMesh_GLES3* pMesh = nullptr;

		std::unique_ptr<GLES3StandardVS3D> m_standardVS;
		std::unique_ptr<GLES3StandardPS3D> m_standardPS;

		ConstantBuffer<VSConstants3D> m_vsConstants3D;
		ConstantBuffer<PSConstants3D> m_psConstants3D;

		GLES3Renderer3DCommandManager m_commandManager;

		Optional<VertexShader> m_currentCustomVS;
		Optional<PixelShader> m_currentCustomPS;

		Renderer3DStat m_stat;
	};
}