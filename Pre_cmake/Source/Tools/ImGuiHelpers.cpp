#include <pch.h>
#include "ImGuiHelpers.h"
#include <math.h>
#include "TimeHelper.h"
#include "FullscreenRenderer.h"
#include "Texture.h"
#include "TextureFactory.h"
#include "Sprite.h"
#include "GamlaBettan\Camera.h"
#include "GamlaBettan\PathFinder.h"

namespace Tools
{
	namespace ImguiHelperGlobals
	{
		ID3D11Device* globalDevice;
		ID3D11DeviceContext* globalContext;
		std::vector<Texture> globalTextureStack;
		size_t globalTextureUsage;
		FullscreenRenderer* globalRenderer;
		
		void ResetCounter()
		{
			globalTextureUsage = 0;
		}

		void Setup(ID3D11Device* aDevice, ID3D11DeviceContext* aContext, FullscreenRenderer* aRenderer)
		{
			ResetCounter();
			globalDevice = aDevice;
			globalContext = aContext;
			globalRenderer = aRenderer;
		}
	}
	thread_local bool dummy = false;


	void ZoomableImGuiImage(void* aTexture, ImVec2 aSize)
	{
		ImVec2 drawPos = ImGui::GetCursorScreenPos();
		ImGui::Image(aTexture, aSize);
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem))
		{
			ImVec2 iopos = ImVec2(ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y);
			ImGui::SetNextWindowPos(iopos);
			ImGui::BeginTooltip();
			static float region_sz = 32.0f;
			static float zoom = 8.0f;
			float delta = ImGui::GetIO().MouseWheel;
			if (ImGui::GetIO().KeyShift)
			{
				zoom *= pow(1.1f, delta);
			}
			else
			{
				region_sz *= pow(1.1f, -delta);
				zoom *= pow(1.1f, delta);
			}
			if (region_sz > 200.f)
			{
				region_sz *= pow(1.1f, delta);
				zoom *= pow(1.1f, -delta);
			}

			float region_x = iopos.x - drawPos.x - region_sz * 0.5f; if (region_x < 0.0f) region_x = 0.0f; else if (region_x > aSize.x - region_sz) region_x = aSize.x - region_sz;
			float region_y = iopos.y - drawPos.y - region_sz * 0.5f; if (region_y < 0.0f) region_y = 0.0f; else if (region_y > aSize.y - region_sz) region_y = aSize.y - region_sz;
			ImVec2 uv0 = ImVec2((region_x) / aSize.x, (region_y) / aSize.y);
			ImVec2 uv1 = ImVec2((region_x + region_sz) / aSize.x, (region_y + region_sz) / aSize.y);
			ImGui::Image(aTexture, ImVec2(region_sz * zoom, region_sz * zoom), uv0, uv1, ImVec4(1.0f, 1.0f, 1.0f, 1.0f), ImVec4(1.0f, 1.0f, 1.0f, 0.5f));
			ImGui::EndTooltip();
		}

	}

	Texture GetNextTexture()
	{
		if (ImguiHelperGlobals::globalTextureUsage == ImguiHelperGlobals::globalTextureStack.size())
		{
			ImguiHelperGlobals::globalTextureStack.push_back(
				TextureFactory::GetInstance().CreateTexture(
					V2ui(960,540),
					DXGI_FORMAT_R8G8B8A8_UNORM,
					"Imgui debug texture"));
		}
		return ImguiHelperGlobals::globalTextureStack[ImguiHelperGlobals::globalTextureUsage++];
	}

	ID3D11ShaderResourceView* CopyTexture(void* aTexture)
	{
		Texture texture = GetNextTexture();
		ID3D11ShaderResourceView* source = (ID3D11ShaderResourceView*)aTexture;

		struct BACKUP_DX11_STATE
		{
			UINT                        ScissorRectsCount, ViewportsCount;
			D3D11_RECT                  ScissorRects[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
			D3D11_VIEWPORT              Viewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
			ID3D11RasterizerState*		RS;
			ID3D11BlendState*			BlendState;
			FLOAT                       BlendFactor[4];
			UINT                        SampleMask;
			UINT                        StencilRef;
			ID3D11DepthStencilState*	DepthStencilState;
			ID3D11ShaderResourceView*	PSShaderResource;
			ID3D11SamplerState*			PSSampler;
			ID3D11PixelShader*			PS;
			ID3D11VertexShader*			VS;
			ID3D11GeometryShader*		GS;
			UINT                        PSInstancesCount, VSInstancesCount, GSInstancesCount;
			ID3D11ClassInstance*		PSInstances[256], * VSInstances[256], * GSInstances[256];   // 256 is max according to PSSetShader documentation
			D3D11_PRIMITIVE_TOPOLOGY    PrimitiveTopology;
			ID3D11Buffer*				IndexBuffer, * VertexBuffer, * VSConstantBuffer;
			UINT                        IndexBufferOffset, VertexBufferStride, VertexBufferOffset;
			DXGI_FORMAT                 IndexBufferFormat;
			ID3D11InputLayout*			InputLayout;
		};
		BACKUP_DX11_STATE old;
		old.ScissorRectsCount = old.ViewportsCount = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
		ImguiHelperGlobals::globalContext->RSGetScissorRects(&old.ScissorRectsCount, old.ScissorRects);
		ImguiHelperGlobals::globalContext->RSGetViewports(&old.ViewportsCount, old.Viewports);
		ImguiHelperGlobals::globalContext->RSGetState(&old.RS);
		ImguiHelperGlobals::globalContext->OMGetBlendState(&old.BlendState, old.BlendFactor, &old.SampleMask);
		ImguiHelperGlobals::globalContext->OMGetDepthStencilState(&old.DepthStencilState, &old.StencilRef);
		ImguiHelperGlobals::globalContext->PSGetShaderResources(0, 1, &old.PSShaderResource);
		ImguiHelperGlobals::globalContext->PSGetSamplers(0, 1, &old.PSSampler);
		old.PSInstancesCount = old.VSInstancesCount = old.GSInstancesCount = 256;
		ImguiHelperGlobals::globalContext->PSGetShader(&old.PS, old.PSInstances, &old.PSInstancesCount);
		ImguiHelperGlobals::globalContext->VSGetShader(&old.VS, old.VSInstances, &old.VSInstancesCount);
		ImguiHelperGlobals::globalContext->VSGetConstantBuffers(0, 1, &old.VSConstantBuffer);
		ImguiHelperGlobals::globalContext->GSGetShader(&old.GS, old.GSInstances, &old.GSInstancesCount);

		ImguiHelperGlobals::globalContext->IAGetPrimitiveTopology(&old.PrimitiveTopology);
		ImguiHelperGlobals::globalContext->IAGetIndexBuffer(&old.IndexBuffer, &old.IndexBufferFormat, &old.IndexBufferOffset);
		ImguiHelperGlobals::globalContext->IAGetVertexBuffers(0, 1, &old.VertexBuffer, &old.VertexBufferStride, &old.VertexBufferOffset);
		ImguiHelperGlobals::globalContext->IAGetInputLayout(&old.InputLayout);

		texture.SetAsActiveTarget();
		ImguiHelperGlobals::globalContext->PSSetShaderResources(0, 1, &source);
		ImguiHelperGlobals::globalRenderer->Render(FullscreenRenderer::Shader::COPY);

		// Restore modified DX state
		ImguiHelperGlobals::globalContext->RSSetScissorRects(old.ScissorRectsCount, old.ScissorRects);
		ImguiHelperGlobals::globalContext->RSSetViewports(old.ViewportsCount, old.Viewports);
		ImguiHelperGlobals::globalContext->RSSetState(old.RS); if (old.RS) old.RS->Release();
		ImguiHelperGlobals::globalContext->OMSetBlendState(old.BlendState, old.BlendFactor, old.SampleMask); if (old.BlendState) old.BlendState->Release();
		ImguiHelperGlobals::globalContext->OMSetDepthStencilState(old.DepthStencilState, old.StencilRef); if (old.DepthStencilState) old.DepthStencilState->Release();
		ImguiHelperGlobals::globalContext->PSSetShaderResources(0, 1, &old.PSShaderResource); if (old.PSShaderResource) old.PSShaderResource->Release();
		ImguiHelperGlobals::globalContext->PSSetSamplers(0, 1, &old.PSSampler); if (old.PSSampler) old.PSSampler->Release();
		ImguiHelperGlobals::globalContext->PSSetShader(old.PS, old.PSInstances, old.PSInstancesCount); if (old.PS) old.PS->Release();
		for (UINT i = 0; i < old.PSInstancesCount; i++) if (old.PSInstances[i]) old.PSInstances[i]->Release();
		ImguiHelperGlobals::globalContext->VSSetShader(old.VS, old.VSInstances, old.VSInstancesCount); if (old.VS) old.VS->Release();
		ImguiHelperGlobals::globalContext->VSSetConstantBuffers(0, 1, &old.VSConstantBuffer); if (old.VSConstantBuffer) old.VSConstantBuffer->Release();
		ImguiHelperGlobals::globalContext->GSSetShader(old.GS, old.GSInstances, old.GSInstancesCount); if (old.GS) old.GS->Release();
		for (UINT i = 0; i < old.VSInstancesCount; i++) if (old.VSInstances[i]) old.VSInstances[i]->Release();
		ImguiHelperGlobals::globalContext->IASetPrimitiveTopology(old.PrimitiveTopology);
		ImguiHelperGlobals::globalContext->IASetIndexBuffer(old.IndexBuffer, old.IndexBufferFormat, old.IndexBufferOffset); if (old.IndexBuffer) old.IndexBuffer->Release();
		ImguiHelperGlobals::globalContext->IASetVertexBuffers(0, 1, &old.VertexBuffer, &old.VertexBufferStride, &old.VertexBufferOffset); if (old.VertexBuffer) old.VertexBuffer->Release();
		ImguiHelperGlobals::globalContext->IASetInputLayout(old.InputLayout); if (old.InputLayout) old.InputLayout->Release();

		return texture.GetResourceView();
	}

	void ZoomableImGuiSnapshot(void* aTexture, ImVec2 aSize)
	{
		ZoomableImGuiImage(CopyTexture(aTexture), aSize);
	}

	bool EditPosition(const char* aName, float* aData, bool& hovered)
	{
		bool changed = false;
		hovered = false;
		changed |= ImGui::InputFloat3(aName, aData);
		hovered |= ImGui::IsItemHovered();
		return changed;
	}

	ImVec4 GetColor(int aDepth)
	{
		static std::vector<ImVec4> colors = {
			ImVec4(0.3f,1.0f,0.4f,1.0f),
			ImVec4(1.0f,0.6f,0.6f,1.0f),
			ImVec4(0.5f,0.7f,0.5f,1.0f),
			ImVec4(0.5f,0.5f,0.8f,1.0f),
			ImVec4(0.9f,0.9f,0.5f,1.0f),
			ImVec4(0.8f,0.2f,0.5f,1.0f),
			ImVec4(0.6f,0.9f,0.1f,1.0f)
		};
		return colors[aDepth % colors.size()];
	}
	ImVec4 DefaultColor(int /*aDepth*/)
	{
		return ImGui::GetStyleColorVec4(ImGuiCol_Text);
	}

	const char* GetCovarage(float aProcent)
	{
		if (aProcent > 0.95)
		{
			return "All";
		}
		if (aProcent > 0.85)
		{
			return "Good";
		}
		if (aProcent > 0.70)
		{
			return "Lacking";
		}
		if (aProcent > 0.40)
		{
			return "Half";
		}
		if (aProcent > 0.3)
		{
			return "Insignificant";
		}
		if (aProcent > 0.1)
		{
			return "Miniscule";
		}
		return "None";
	}
	std::string PadOrTrimTo(const std::string& aString, int aLength)
	{
		if (aString.length() > aLength)
		{
			return aString.substr(0, aLength);
		}
		return aString + std::string(aLength - aString.length(), ' ');
	}

	Tools::TimeTree*& HoveredTimeTree()
	{
		static Tools::TimeTree* val = nullptr;
		return val;
	}

	void DrawTimeTree_internal(Tools::TimeTree* aTree, int aDepth, std::function<ImVec4(int)> aColorGetter, const char* aFormat, std::function<float(TimeTree*)> aArgumentGetter)
	{
		ImGui::PushStyleColor(ImGuiCol_Text, aColorGetter(aDepth));
		if (ImGui::TreeNode(aTree->myName, aFormat, PadOrTrimTo(aTree->myName, 20).c_str(),(int)aTree->myCallCount,  aArgumentGetter(aTree), GetCovarage(aTree->myCovarage)))
		{
			if (ImGui::IsItemHovered())
			{
				HoveredTimeTree() = aTree;
			}
			for (auto& i : aTree->myChildren)
			{
				DrawTimeTree_internal(i, aDepth + 1, aColorGetter, aFormat, aArgumentGetter);
			}
			if (aTree->myChildren.empty())
			{
				ImGui::BulletText("No futher breakdown available");
			}
			ImGui::TreePop();
		}
		ImGui::PopStyleColor(1);
	}

	void DrawTimeTree(Tools::TimeTree* aTree)
	{

		ImGui::PushID(aTree->myName);
		static bool colorize = true;
		ImGui::Checkbox("Colorize", &colorize);
		static int mode = 0;

		const char* names[4] = { "time","Percent of parent","percent of performance","percent of hovered" };
		ImGui::SameLine();
		if (ImGui::BeginCombo("Value Mode", names[mode]))
		{
			for (int i = 0; i < 4; i++)
			{
				bool selected = mode == i;
				if (ImGui::Selectable(names[i], &selected))
				{
					mode = i;
				}
			}
			ImGui::EndCombo();
		}


		const char* format = nullptr;
		std::function<float(TimeTree*)> valueGetter;
		switch (mode)
		{
		case 0:
			format = "%s: [%3d] %5.2fms covarage: %s";
			valueGetter = [](TimeTree* aNode) -> float { return aNode->myTime * 1000.f; };
			break;
		case 1:
			format = "%s: [%3d] %5.1f%% covarage: %s";
			valueGetter = [](TimeTree* aNode) -> float { return (aNode->myParent ? aNode->myTime / aNode->myParent->myTime : 1.f) * 100.f; };
			break;
		case 2:
			format = "%s: [%3d] %5.1f%% covarage: %s";
			valueGetter = [](TimeTree* aNode) -> float
			{
				std::function< float(TimeTree*)> GetRootTime = [&GetRootTime](TimeTree* aNode) -> float
				{
					if (aNode->myParent)
					{
						return GetRootTime(aNode->myParent);
					}
					return aNode->myTime;
				};
				return aNode->myTime / GetRootTime(aNode) * 100.f;
			};
			break;
		case 3:
			format = "%s: [%3d] %5.1f%% covarage: %s";
			valueGetter = [](TimeTree* aNode) -> float { return (HoveredTimeTree() ? aNode->myTime / HoveredTimeTree()->myTime : 1.f) * 100.f; };
			break;
		default:
			break;
		}

		ImGui::BeginChild(aTree->myName);
		if (colorize)
		{
			DrawTimeTree_internal(aTree, 0, &GetColor, format, valueGetter);
		}
		else
		{
			DrawTimeTree_internal(aTree, 0, &DefaultColor, format, valueGetter);
		}
		ImGui::EndChild();
		ImGui::PopID();
	}

}