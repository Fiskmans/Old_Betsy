#include "engine/graphics/GBuffer.h"
#include "engine/graphics/GraphicEngine.h"

#include "tools/ImGuiHelpers.h"

#include "common/Macros.h"

namespace engine::graphics
{
	GBuffer::~GBuffer()
	{
		for (ID3D11Texture2D*& i : myTextures)
			SAFE_RELEASE(i);
	
		for (ID3D11RenderTargetView*& i : myRenderTargets)
			SAFE_RELEASE(i);
		
		for (ID3D11ShaderResourceView*& i : myShaderResources)
			SAFE_RELEASE(i);
		
		SAFE_DELETE(myViewport);
	}

	GBuffer::GBuffer(GBuffer&& aOther)
	{
		for(size_t i = 0; i < myTextures.size(); i++)
			std::swap(myTextures[i], aOther.myTextures[i]);
		for(size_t i = 0; i < myRenderTargets.size(); i++)
			std::swap(myRenderTargets[i], aOther.myRenderTargets[i]);
		for(size_t i = 0; i < myShaderResources.size(); i++)
			std::swap(myShaderResources[i], aOther.myShaderResources[i]);

		std::swap(myViewport, aOther.myViewport);
	}

	void GBuffer::operator=(GBuffer&& aOther)
	{
		for (size_t i = 0; i < myTextures.size(); i++)
			std::swap(myTextures[i], aOther.myTextures[i]);
		for (size_t i = 0; i < myRenderTargets.size(); i++)
			std::swap(myRenderTargets[i], aOther.myRenderTargets[i]);
		for (size_t i = 0; i < myShaderResources.size(); i++)
			std::swap(myShaderResources[i], aOther.myShaderResources[i]);

		std::swap(myViewport, aOther.myViewport);
	}

	void GBuffer::ClearTextures(const tools::V4f aClearColor)
	{
		for (auto& i : myRenderTargets)
		{
			GraphicsEngine::GetInstance().GetFrameWork().GetContext()->ClearRenderTargetView(i, aClearColor.Raw());
		}
	}

	void GBuffer::SetAsActiveTarget(const DepthTexture* aDepth) const
	{
		ID3D11DeviceContext* context = GraphicsEngine::GetInstance().GetFrameWork().GetContext();
		context->OMSetRenderTargets(static_cast<int>(GBuffer::Channel::Count), myRenderTargets.data(), aDepth ? aDepth->myDepth : nullptr);
		context->RSSetViewports(1, myViewport);
	}

	void GBuffer::SetAsActiveTarget(const Channel aResource, const DepthTexture* aDepth) const
	{
		ID3D11DeviceContext* context = GraphicsEngine::GetInstance().GetFrameWork().GetContext();
		context->OMSetRenderTargets(1, myRenderTargets.data() + static_cast<int>(aResource), aDepth ? aDepth->myDepth : nullptr);
		context->RSSetViewports(1, myViewport);
	}

	void GBuffer::SetAsResourceOnSlot(const Channel aResource, const unsigned int aSlot)
	{
		GraphicsEngine::GetInstance().GetFrameWork().GetContext()->PSSetShaderResources(aSlot, 1, &myShaderResources[static_cast<int>(aResource)]);
	}

	void GBuffer::SetAllAsResources() const
	{
		GraphicsEngine::GetInstance().GetFrameWork().GetContext()->PSSetShaderResources(0, static_cast<int>(GBuffer::Channel::Count), myShaderResources.data());
	}

	void GBuffer::CopyTo(GBuffer* aOther, ID3D11DeviceContext* aContext)
	{
		for (size_t i = 0; i < static_cast<int>(Channel::Count); i++)
		{
			aContext->CopyResource(aOther->myTextures[i], myTextures[i]);
		}
	}

	void GBuffer::Imgui(ImVec2 aTopLeft, ImVec2 aBottomRight) const
	{
		ImDrawList* drawList = ImGui::GetWindowDrawList();
		ImVec2 size = ImVec2((aBottomRight.x - aTopLeft.x) / 2.f, (aBottomRight.y - aTopLeft.y) / 4.f);
		ImVec2 padding = ImVec2(size.x / 10.f, size.y / 10.f);
		ImVec2 imageSize = ImVec2(size.x - padding.x * 2.f, size.y - padding.y * 2.f);

		drawList->AddRect(aTopLeft, aBottomRight, ImColor(0.3f, 0.3f, 0.3f, 1.f));

		for (size_t x = 0; x < 2; x++)
		{
			for (size_t y = 0; y < 4; y++)
			{
				ImVec2 at = aTopLeft;
				at.x += x * size.x;
				at.y += y * size.y;
				at.x += padding.x;
				at.y += padding.y;
				ImGui::SetCursorScreenPos(at);
				ImGui::Image(myShaderResources[x * 4 + y], imageSize, ImVec2(0,0), ImVec2(1,1), ImVec4(1,1,1,1), tools::GetImColor(ImGuiCol_Border));
			}
		}
	}
}