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

# include "CRenderer2D_D3D11.hpp"
# include <Siv3D/Error.hpp>
# include <Siv3D/Resource.hpp>
# include <Siv3D/EngineLog.hpp>
# include <Siv3D/ScopeGuard.hpp>
# include <Siv3D/Mat3x2.hpp>
# include <Siv3D/ShaderCommon.hpp>
# include <Siv3D/Common/Siv3DEngine.hpp>
# include <Siv3D/Renderer/D3D11/CRenderer_D3D11.hpp>
# include <Siv3D/Shader/D3D11/CShader_D3D11.hpp>

/*
#	define LOG_COMMAND(...) LOG_TRACE(__VA_ARGS__)
/*/
#	define LOG_COMMAND(...) ((void)0)
//*/

namespace s3d
{
	CRenderer2D_D3D11::CRenderer2D_D3D11()
	{

	}

	CRenderer2D_D3D11::~CRenderer2D_D3D11()
	{
		LOG_SCOPED_TRACE(U"CRenderer2D_D3D11::~CRenderer2D_D3D11()");
	}

	void CRenderer2D_D3D11::init()
	{
		LOG_SCOPED_TRACE(U"CRenderer2D_D3D11::init()");
		
		// 各種ポインタを保存
		{
			pRenderer	= dynamic_cast<CRenderer_D3D11*>(SIV3D_ENGINE(Renderer)); assert(pRenderer);
			pShader		= dynamic_cast<CShader_D3D11*>(SIV3D_ENGINE(Shader)); assert(pShader);
			m_device	= pRenderer->getDevice(); assert(m_device);
			m_context	= pRenderer->getContext(); assert(m_context);
		}

		// 標準 VS をロード
		{
			LOG_INFO(U"📦 Loading vertex shaders for CRenderer2D_D3D11:");
			m_standardVS = std::make_unique<D3D11StandardVS2D>();
			m_standardVS->sprite				= HLSL(FileOrResource(U"engine/shader/d3d11/sprite.vs"));
			m_standardVS->fullscreen_triangle	= HLSL(FileOrResource(U"engine/shader/d3d11/fullscreen_triangle.vs"));
			if (not m_standardVS->setup())
			{
				throw EngineError(U"CRenderer2D_D3D11::m_standardVS initialization failed");
			}
		}
		
		// 標準 PS をロード
		{
			LOG_INFO(U"📦 Loading pixel shaders for CRenderer2D_D3D11:");
			m_standardPS = std::make_unique<D3D11StandardPS2D>();
			m_standardPS->shape					= HLSL(FileOrResource(U"engine/shader/d3d11/shape.ps"));
			m_standardPS->fullscreen_triangle	= HLSL(FileOrResource(U"engine/shader/d3d11/fullscreen_triangle.ps"));
			if (not m_standardPS->setup())
			{
				throw EngineError(U"CRenderer2D_D3D11::m_standardPS initialization failed");
			}
		}

		// 標準 InputLayout を作成
		{
			const D3D11_INPUT_ELEMENT_DESC layout[3] = {
				{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT,       0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0,  8, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			};

			const Blob& binary = m_standardVS->sprite.getBinary();
			if (FAILED(m_device->CreateInputLayout(layout, 3, binary.data(), binary.size(), &m_inputLayout)))
			{
				throw EngineError(U"ID3D11Device::CreateInputLayout() failed");
			}
		}

		// Batch 管理を初期化
		{
			if (not m_batches.init(m_device, m_context))
			{
				throw EngineError(U"D3D11Vertex2DBatch::init() failed");
			}
		}

		// バッファ作成関数を作成
		m_bufferCreator = [this](Vertex2D::IndexType vertexSize, Vertex2D::IndexType indexSize)
		{
			return m_batches.requestBuffer(vertexSize, indexSize, m_commandManager);
		};
	}

	void CRenderer2D_D3D11::addLine(const Float2& begin, const Float2& end, const float thickness, const Float4(&colors)[2])
	{
		if (const auto indexCount = Vertex2DBuilder::BuildDefaultLine(m_bufferCreator, begin, end, thickness, colors))
		{
			if (not m_currentCustomVS)
			{
				m_commandManager.pushStandardVS(m_standardVS->spriteID);
			}

			if (not m_currentCustomPS)
			{
				m_commandManager.pushStandardPS(m_standardPS->shapeID);
			}

			m_commandManager.pushDraw(indexCount);
		}
	}

	void CRenderer2D_D3D11::addTriangle(const Float2(&points)[3], const Float4& color)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildTriangle(m_bufferCreator, points, color))
		{
			if (not m_currentCustomVS)
			{
				m_commandManager.pushStandardVS(m_standardVS->spriteID);
			}

			if (not m_currentCustomPS)
			{
				m_commandManager.pushStandardPS(m_standardPS->shapeID);
			}

			m_commandManager.pushDraw(indexCount);
		}
	}

	void CRenderer2D_D3D11::addTriangle(const Float2(&points)[3], const Float4(&colors)[3])
	{
		if (const auto indexCount = Vertex2DBuilder::BuildTriangle(m_bufferCreator, points, colors))
		{
			if (not m_currentCustomVS)
			{
				m_commandManager.pushStandardVS(m_standardVS->spriteID);
			}

			if (not m_currentCustomPS)
			{
				m_commandManager.pushStandardPS(m_standardPS->shapeID);
			}

			m_commandManager.pushDraw(indexCount);
		}
	}

	void CRenderer2D_D3D11::addRect(const FloatRect& rect, const Float4& color)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildRect(m_bufferCreator, rect, color))
		{
			if (not m_currentCustomVS)
			{
				m_commandManager.pushStandardVS(m_standardVS->spriteID);
			}

			if (not m_currentCustomPS)
			{
				m_commandManager.pushStandardPS(m_standardPS->shapeID);
			}

			m_commandManager.pushDraw(indexCount);
		}
	}

	void CRenderer2D_D3D11::addRect(const FloatRect& rect, const Float4(&colors)[4])
	{
		if (const auto indexCount = Vertex2DBuilder::BuildRect(m_bufferCreator, rect, colors))
		{
			if (not m_currentCustomVS)
			{
				m_commandManager.pushStandardVS(m_standardVS->spriteID);
			}

			if (not m_currentCustomPS)
			{
				m_commandManager.pushStandardPS(m_standardPS->shapeID);
			}

			m_commandManager.pushDraw(indexCount);
		}
	}

	void CRenderer2D_D3D11::addRectFrame(const FloatRect& rect, const float thickness, const Float4& innerColor, const Float4& outerColor)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildRectFrame(m_bufferCreator, rect, thickness, innerColor, outerColor))
		{
			if (not m_currentCustomVS)
			{
				m_commandManager.pushStandardVS(m_standardVS->spriteID);
			}

			if (not m_currentCustomPS)
			{
				m_commandManager.pushStandardPS(m_standardPS->shapeID);
			}

			m_commandManager.pushDraw(indexCount);
		}
	}

	void CRenderer2D_D3D11::addCircle(const Float2& center, const float r, const Float4& innerColor, const Float4& outerColor)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildCircle(m_bufferCreator, center, r, innerColor, outerColor, getMaxScaling()))
		{
			if (not m_currentCustomVS)
			{
				m_commandManager.pushStandardVS(m_standardVS->spriteID);
			}

			if (not m_currentCustomPS)
			{
				m_commandManager.pushStandardPS(m_standardPS->shapeID);
			}

			m_commandManager.pushDraw(indexCount);
		}
	}

	void CRenderer2D_D3D11::addCircleFrame(const Float2& center, const float rInner, const float thickness, const Float4& innerColor, const Float4& outerColor)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildCircleFrame(m_bufferCreator, center, rInner, thickness, innerColor, outerColor, getMaxScaling()))
		{
			if (not m_currentCustomVS)
			{
				m_commandManager.pushStandardVS(m_standardVS->spriteID);
			}

			if (not m_currentCustomPS)
			{
				m_commandManager.pushStandardPS(m_standardPS->shapeID);
			}

			m_commandManager.pushDraw(indexCount);
		}
	}

	void CRenderer2D_D3D11::addCirclePie(const Float2& center, const float r, const float startAngle, const float angle, const Float4& innerColor, const Float4& outerColor)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildCirclePie(m_bufferCreator, center, r, startAngle, angle, innerColor, outerColor, getMaxScaling()))
		{
			if (not m_currentCustomVS)
			{
				m_commandManager.pushStandardVS(m_standardVS->spriteID);
			}

			if (not m_currentCustomPS)
			{
				m_commandManager.pushStandardPS(m_standardPS->shapeID);
			}

			m_commandManager.pushDraw(indexCount);
		}
	}

	void CRenderer2D_D3D11::addCircleArc(const Float2& center, const float rInner, const float startAngle, const float angle, const float thickness, const Float4& innerColor, const Float4& outerColor)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildCircleArc(m_bufferCreator, center, rInner, startAngle, angle, thickness, innerColor, outerColor, getMaxScaling()))
		{
			if (not m_currentCustomVS)
			{
				m_commandManager.pushStandardVS(m_standardVS->spriteID);
			}

			if (not m_currentCustomPS)
			{
				m_commandManager.pushStandardPS(m_standardPS->shapeID);
			}

			m_commandManager.pushDraw(indexCount);
		}
	}

	void CRenderer2D_D3D11::addEllipse(const Float2& center, const float a, const float b, const Float4& innerColor, const Float4& outerColor)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildEllipse(m_bufferCreator, center, a, b, innerColor, outerColor, getMaxScaling()))
		{
			if (not m_currentCustomVS)
			{
				m_commandManager.pushStandardVS(m_standardVS->spriteID);
			}

			if (not m_currentCustomPS)
			{
				m_commandManager.pushStandardPS(m_standardPS->shapeID);
			}

			m_commandManager.pushDraw(indexCount);
		}
	}

	void CRenderer2D_D3D11::addEllipseFrame(const Float2& center, const float aInner, const float bInner, const float thickness, const Float4& innerColor, const Float4& outerColor)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildEllipseFrame(m_bufferCreator, center, aInner, bInner, thickness, innerColor, outerColor, getMaxScaling()))
		{
			if (not m_currentCustomVS)
			{
				m_commandManager.pushStandardVS(m_standardVS->spriteID);
			}

			if (not m_currentCustomPS)
			{
				m_commandManager.pushStandardPS(m_standardPS->shapeID);
			}

			m_commandManager.pushDraw(indexCount);
		}
	}

	void CRenderer2D_D3D11::addQuad(const FloatQuad& quad, const Float4& color)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildQuad(m_bufferCreator, quad, color))
		{
			if (not m_currentCustomVS)
			{
				m_commandManager.pushStandardVS(m_standardVS->spriteID);
			}

			if (not m_currentCustomPS)
			{
				m_commandManager.pushStandardPS(m_standardPS->shapeID);
			}

			m_commandManager.pushDraw(indexCount);
		}
	}

	void CRenderer2D_D3D11::addQuad(const FloatQuad& quad, const Float4(&colors)[4])
	{
		if (const auto indexCount = Vertex2DBuilder::BuildQuad(m_bufferCreator, quad, colors))
		{
			if (not m_currentCustomVS)
			{
				m_commandManager.pushStandardVS(m_standardVS->spriteID);
			}

			if (not m_currentCustomPS)
			{
				m_commandManager.pushStandardPS(m_standardPS->shapeID);
			}

			m_commandManager.pushDraw(indexCount);
		}
	}

	void CRenderer2D_D3D11::addRoundRect(const FloatRect& rect, const float w, const float h, const float r, const Float4& color)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildRoundRect(m_bufferCreator, rect, w, h, r, color, getMaxScaling()))
		{
			if (not m_currentCustomVS)
			{
				m_commandManager.pushStandardVS(m_standardVS->spriteID);
			}

			if (not m_currentCustomPS)
			{
				m_commandManager.pushStandardPS(m_standardPS->shapeID);
			}

			m_commandManager.pushDraw(indexCount);
		}
	}

	void CRenderer2D_D3D11::addLineString(const Vec2* points, const size_t size, const Optional<Float2>& offset, const float thickness, const bool inner, const Float4& color, const CloseRing closeRing)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildDefaultLineString(m_bufferCreator, points, size, offset, thickness, inner, color, closeRing, getMaxScaling()))
		{
			if (not m_currentCustomVS)
			{
				m_commandManager.pushStandardVS(m_standardVS->spriteID);
			}

			if (not m_currentCustomPS)
			{
				m_commandManager.pushStandardPS(m_standardPS->shapeID);
			}

			m_commandManager.pushDraw(indexCount);
		}
	}

	void CRenderer2D_D3D11::addLineString(const Vec2* points, const ColorF* colors, size_t size, const Optional<Float2>& offset, const float thickness, const bool inner, const CloseRing closeRing)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildDefaultLineString(m_bufferCreator, points, colors, size, offset, thickness, inner, closeRing, getMaxScaling()))
		{
			if (not m_currentCustomVS)
			{
				m_commandManager.pushStandardVS(m_standardVS->spriteID);
			}

			if (not m_currentCustomPS)
			{
				m_commandManager.pushStandardPS(m_standardPS->shapeID);
			}

			m_commandManager.pushDraw(indexCount);
		}
	}

	void CRenderer2D_D3D11::addPolygon(const Array<Float2>& vertices, const Array<TriangleIndex>& indices, const Optional<Float2>& offset, const Float4& color)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildPolygon(m_bufferCreator, vertices, indices, offset, color))
		{
			if (not m_currentCustomVS)
			{
				m_commandManager.pushStandardVS(m_standardVS->spriteID);
			}

			if (not m_currentCustomPS)
			{
				m_commandManager.pushStandardPS(m_standardPS->shapeID);
			}

			m_commandManager.pushDraw(indexCount);
		}
	}

	void CRenderer2D_D3D11::addPolygon(const Vertex2D* vertices, const size_t vertexCount, const TriangleIndex* indices, const size_t num_triangles)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildPolygon(m_bufferCreator, vertices, vertexCount, indices, num_triangles))
		{
			if (not m_currentCustomVS)
			{
				m_commandManager.pushStandardVS(m_standardVS->spriteID);
			}

			if (not m_currentCustomPS)
			{
				m_commandManager.pushStandardPS(m_standardPS->shapeID);
			}

			m_commandManager.pushDraw(indexCount);
		}
	}

	void CRenderer2D_D3D11::addPolygonFrame(const Float2* points, const size_t size, const float thickness, const Float4& color)
	{
		if (const auto indexCount = Vertex2DBuilder::BuildPolygonFrame(m_bufferCreator, points, size, thickness, color, getMaxScaling()))
		{
			if (not m_currentCustomVS)
			{
				m_commandManager.pushStandardVS(m_standardVS->spriteID);
			}

			if (not m_currentCustomPS)
			{
				m_commandManager.pushStandardPS(m_standardPS->shapeID);
			}

			m_commandManager.pushDraw(indexCount);
		}
	}

	void CRenderer2D_D3D11::addNullVertices(const uint32 count)
	{
		if (not m_currentCustomPS)
		{
			m_commandManager.pushStandardPS(m_standardPS->shapeID);
		}

		m_commandManager.pushNullVertices(count);
	}


	Float4 CRenderer2D_D3D11::getColorMul() const
	{
		return m_commandManager.getCurrentColorMul();
	}

	Float4 CRenderer2D_D3D11::getColorAdd() const
	{
		return m_commandManager.getCurrentColorAdd();
	}

	void CRenderer2D_D3D11::setColorMul(const Float4& color)
	{
		m_commandManager.pushColorMul(color);
	}

	void CRenderer2D_D3D11::setColorAdd(const Float4& color)
	{
		m_commandManager.pushColorAdd(color);
	}

	BlendState CRenderer2D_D3D11::getBlendState() const
	{
		return m_commandManager.getCurrentBlendState();
	}

	RasterizerState CRenderer2D_D3D11::getRasterizerState() const
	{
		return m_commandManager.getCurrentRasterizerState();
	}

	SamplerState CRenderer2D_D3D11::getSamplerState(const ShaderStage shaderStage, const uint32 slot) const
	{
		if (shaderStage == ShaderStage::Vertex)
		{
			return m_commandManager.getVSCurrentSamplerState(slot);
		}
		else
		{
			return m_commandManager.getPSCurrentSamplerState(slot);
		}
	}

	void CRenderer2D_D3D11::setBlendState(const BlendState& state)
	{
		m_commandManager.pushBlendState(state);
	}

	void CRenderer2D_D3D11::setRasterizerState(const RasterizerState& state)
	{
		m_commandManager.pushRasterizerState(state);
	}

	void CRenderer2D_D3D11::setSamplerState(const ShaderStage shaderStage, const uint32 slot, const SamplerState& state)
	{
		if (shaderStage == ShaderStage::Vertex)
		{
			m_commandManager.pushVSSamplerState(state, slot);
		}
		else
		{
			m_commandManager.pushPSSamplerState(state, slot);
		}
	}

	Optional<VertexShader> CRenderer2D_D3D11::getCustomVS() const
	{
		return m_currentCustomVS;
	}

	Optional<PixelShader> CRenderer2D_D3D11::getCustomPS() const
	{
		return m_currentCustomPS;
	}

	void CRenderer2D_D3D11::setCustomVS(const Optional<VertexShader>& vs)
	{
		if (vs && (not vs->isEmpty()))
		{
			m_currentCustomVS = vs.value();
			m_commandManager.pushCustomVS(vs.value());
		}
		else
		{
			m_currentCustomVS.reset();
		}
	}

	void CRenderer2D_D3D11::setCustomPS(const Optional<PixelShader>& ps)
	{
		if (ps && (not ps->isEmpty()))
		{
			m_currentCustomPS = ps.value();
			m_commandManager.pushCustomPS(ps.value());
		}
		else
		{
			m_currentCustomPS.reset();
		}
	}

	const Mat3x2& CRenderer2D_D3D11::getLocalTransform() const
	{
		return m_commandManager.getCurrentLocalTransform();
	}

	const Mat3x2& CRenderer2D_D3D11::getCameraTransform() const
	{
		return m_commandManager.getCurrentCameraTransform();
	}

	void CRenderer2D_D3D11::setLocalTransform(const Mat3x2& matrix)
	{
		m_commandManager.pushLocalTransform(matrix);
	}

	void CRenderer2D_D3D11::setCameraTransform(const Mat3x2& matrix)
	{
		m_commandManager.pushCameraTransform(matrix);
	}

	float CRenderer2D_D3D11::getMaxScaling() const noexcept
	{
		return m_commandManager.getCurrentMaxScaling();
	}

	void CRenderer2D_D3D11::flush()
	{
		ScopeGuard cleanUp = [this]()
		{
			m_batches.reset();
			m_commandManager.reset();
			m_currentCustomVS.reset();
			m_currentCustomPS.reset();
		};

		m_commandManager.flush();

		m_context->IASetInputLayout(m_inputLayout.Get());
		pShader->setConstantBufferVS(0, m_vsConstants2D.base());
		pShader->setConstantBufferPS(0, m_psConstants2D.base());

		const Size currentRenderTargetSize = SIV3D_ENGINE(Renderer)->getSceneBufferSize();

		{
			const CD3D11_VIEWPORT viewport{
				0.0f, 0.0f,
				static_cast<float>(currentRenderTargetSize.x), static_cast<float>(currentRenderTargetSize.y) };
			m_context->RSSetViewports(1, &viewport);
		}

		Mat3x2 transform = Mat3x2::Identity();
		Mat3x2 screenMat = Mat3x2::Screen(currentRenderTargetSize);

		pRenderer->getBackBuffer().bindRenderTarget(pRenderer->getBackBuffer().getSceneBuffer().getRTV());;

		BatchInfo2D batchInfo;

		LOG_COMMAND(U"----");

		for (const auto& command : m_commandManager.getCommands())
		{
			switch (command.type)
			{
			case D3D11Renderer2DCommandType::Null:
				{
					LOG_COMMAND(U"Null");
					break;
				}
			case D3D11Renderer2DCommandType::SetBuffers:
				{
					m_batches.setBuffers();

					LOG_COMMAND(U"SetBuffers[{}]"_fmt(command.index));
					break;
				}
			case D3D11Renderer2DCommandType::UpdateBuffers:
				{
					batchInfo = m_batches.updateBuffers(command.index);
					
					LOG_COMMAND(U"UpdateBuffers[{}] BatchInfo(indexCount = {}, startIndexLocation = {}, baseVertexLocation = {})"_fmt(
						command.index, batchInfo.indexCount, batchInfo.startIndexLocation, batchInfo.baseVertexLocation));
					break;
				}
			case D3D11Renderer2DCommandType::Draw:
				{
					m_vsConstants2D._update_if_dirty();
					m_psConstants2D._update_if_dirty();

					const D3D11DrawCommand& draw = m_commandManager.getDraw(command.index);
					const uint32 indexCount = draw.indexCount;
					const uint32 startIndexLocation = batchInfo.startIndexLocation;
					const uint32 baseVertexLocation = batchInfo.baseVertexLocation;

					m_context->DrawIndexed(indexCount, startIndexLocation, baseVertexLocation);
					batchInfo.startIndexLocation += indexCount;

					LOG_COMMAND(U"Draw[{}] indexCount = {}, startIndexLocation = {}"_fmt(command.index, indexCount, startIndexLocation));
					break;
				}
			case D3D11Renderer2DCommandType::DrawNull:
				{
					m_vsConstants2D._update_if_dirty();
					m_psConstants2D._update_if_dirty();

					const uint32 draw = m_commandManager.getNullDraw(command.index);

					// draw null vertex buffer
					{
						m_context->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
						m_context->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
						m_context->IASetInputLayout(nullptr);

						m_context->Draw(draw, 0);

						m_batches.setBuffers();
						m_context->IASetInputLayout(m_inputLayout.Get());
					}

					LOG_COMMAND(U"DrawNull[{}] count = {}"_fmt(command.index, draw));
					break;
				}
			case D3D11Renderer2DCommandType::ColorMul:
				{
					m_vsConstants2D->colorMul = m_commandManager.getColorMul(command.index);
					LOG_COMMAND(U"ColorMul[{}] {}"_fmt(command.index, m_cbSprite0->colorMul));
					break;
				}
			case D3D11Renderer2DCommandType::ColorAdd:
				{
					m_psConstants2D->colorAdd = m_commandManager.getColorAdd(command.index);
					LOG_COMMAND(U"ColorAdd[{}] {}"_fmt(command.index, m_cbSprite1->colorAdd));
					break;
				}
			case D3D11Renderer2DCommandType::BlendState:
				{
					const auto& blendState = m_commandManager.getBlendState(command.index);
					pRenderer->getBlendState().set(blendState);
					LOG_COMMAND(U"BlendState[{}]"_fmt(command.index));
					break;
				}
			case D3D11Renderer2DCommandType::RasterizerState:
				{
					const auto& rasterizerState = m_commandManager.getRasterizerState(command.index);
					pRenderer->getRasterizerState().set(rasterizerState);
					LOG_COMMAND(U"RasterizerState[{}]"_fmt(command.index));
					break;
				}
			case D3D11Renderer2DCommandType::VSSamplerState0:
			case D3D11Renderer2DCommandType::VSSamplerState1:
			case D3D11Renderer2DCommandType::VSSamplerState2:
			case D3D11Renderer2DCommandType::VSSamplerState3:
			case D3D11Renderer2DCommandType::VSSamplerState4:
			case D3D11Renderer2DCommandType::VSSamplerState5:
			case D3D11Renderer2DCommandType::VSSamplerState6:
			case D3D11Renderer2DCommandType::VSSamplerState7:
				{
					const uint32 slot = FromEnum(command.type) - FromEnum(D3D11Renderer2DCommandType::VSSamplerState0);
					const auto& samplerState = m_commandManager.getVSSamplerState(slot, command.index);
					pRenderer->getSamplerState().setVS(slot, samplerState);
					LOG_COMMAND(U"VSSamplerState{}[{}] "_fmt(slot, command.index));
					break;
				}
			case D3D11Renderer2DCommandType::PSSamplerState0:
			case D3D11Renderer2DCommandType::PSSamplerState1:
			case D3D11Renderer2DCommandType::PSSamplerState2:
			case D3D11Renderer2DCommandType::PSSamplerState3:
			case D3D11Renderer2DCommandType::PSSamplerState4:
			case D3D11Renderer2DCommandType::PSSamplerState5:
			case D3D11Renderer2DCommandType::PSSamplerState6:
			case D3D11Renderer2DCommandType::PSSamplerState7:
				{
					const uint32 slot = FromEnum(command.type) - FromEnum(D3D11Renderer2DCommandType::PSSamplerState0);
					const auto& samplerState = m_commandManager.getPSSamplerState(slot, command.index);
					pRenderer->getSamplerState().setPS(slot, samplerState);
					LOG_COMMAND(U"PSSamplerState{}[{}] "_fmt(slot, command.index));
					break;
				}
			case D3D11Renderer2DCommandType::SetVS:
				{
					const auto& vsID = m_commandManager.getVS(command.index);

					if (vsID == VertexShader::IDType::InvalidValue())
					{
						;// [Siv3D ToDo] set null
						LOG_COMMAND(U"SetVS[{}]: null"_fmt(command.index));
					}
					else
					{
						pShader->setVS(vsID);
						LOG_COMMAND(U"SetVS[{}]: {}"_fmt(command.index, vsID.value()));
					}

					break;
				}
			case D3D11Renderer2DCommandType::SetPS:
				{
					const auto& psID = m_commandManager.getPS(command.index);

					if (psID == PixelShader::IDType::InvalidValue())
					{
						;// [Siv3D ToDo] set null
						LOG_COMMAND(U"SetPS[{}]: null"_fmt(command.index));
					}
					else
					{
						pShader->setPS(psID);
						LOG_COMMAND(U"SetPS[{}]: {}"_fmt(command.index, psID.value()));
					}

					break;
				}
			case D3D11Renderer2DCommandType::Transform:
				{
					transform = m_commandManager.getCombinedTransform(command.index);
					const Mat3x2 matrix = (transform * screenMat);
					m_vsConstants2D->transform[0].set(matrix._11, matrix._12, matrix._31, matrix._32);
					m_vsConstants2D->transform[1].set(matrix._21, matrix._22, 0.0f, 1.0f);

					LOG_COMMAND(U"Transform[{}] {}"_fmt(command.index, matrix));
					break;
				}
			}
		}
	}

	void CRenderer2D_D3D11::drawFullScreenTriangle(const TextureFilter textureFilter)
	{
		// view port
		{
			const auto [s, viewRect] = pRenderer->getLetterboxComposition();
			const CD3D11_VIEWPORT viewport{
				static_cast<float>(viewRect.x), static_cast<float>(viewRect.y),
				static_cast<float>(viewRect.w), static_cast<float>(viewRect.h) };
			m_context->RSSetViewports(1, &viewport);
		}

		// render states
		{
			const SamplerState samplerState = (textureFilter == TextureFilter::Linear) ?
				SamplerState::ClampLinear : SamplerState::ClampNearest;
			pRenderer->getSamplerState().setPS(0, samplerState);
			pRenderer->getBlendState().set(BlendState::Opaque);
			pRenderer->getRasterizerState().set(RasterizerState::Default2D);
		}
		
		// shaders
		{
			pShader->setVS(m_standardVS->fullscreen_triangle.id());
			pShader->setPS(m_standardPS->fullscreen_triangle.id());
		}

		// draw null vertex buffer
		{
			m_context->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
			m_context->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
			m_context->IASetInputLayout(nullptr);
			m_context->Draw(3, 0);
		}

		//Siv3DEngine::Get<ISiv3DProfiler>()->reportDrawcalls(1, 1);
	}
}