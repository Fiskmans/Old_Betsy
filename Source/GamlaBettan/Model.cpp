#include "pch.h"
#include "Model.h"
#include <d3d11.h>
#include "ModelLoader.h"
#include <algorithm>
#include "ShaderCompiler.h"
#include "ShaderFlags.h"
#include "TextureLoader.h"

#if USEIMGUI
#include <imgui.h>
#include <ImGuiHelpers.h>
#endif // !_RETAIL


#if USEFILEWATHCER
Tools::FileWatcher Model::ourFileWatcher;
#endif

Model::Model() : myModelData()
{
	myLoader = nullptr;
}

Model::~Model()
{
#if  USEFILEWATHCER
	ourFileWatcher.UnRegister(myPSFileHandle);
	ourFileWatcher.UnRegister(myVSFileHandle);
#endif //  USEFILEWATHCER

	for (auto& i : myModelData.myLodLevels)
	{
		delete i;
	}

	SAFE_RELEASE(myModelData.myInputLayout);

	if (myLoader)
	{
		if (myModelData.myTextures[0]) { myModelData.myTextures[0]->Release(); }
		if (myModelData.myTextures[1]) { myModelData.myTextures[1]->Release(); }
		if (myModelData.myTextures[2]) { myModelData.myTextures[2]->Release(); }
	}

}

void Model::Init(const CModelData& data, ModelLoader* aLoader, const std::string& aPixelShaderFileName, const std::string& aVertexShaderFileName, const std::string& aFilePath, const std::string& aFriendlyName)
{
	myLoader = aLoader;
	myModelData = data;
	myFilePath = aFilePath;
#if USEFILEWATHCER
	if (aPixelShaderFileName != "")
	{
		myPSFileHandle = ourFileWatcher.RegisterCallback(aPixelShaderFileName, std::bind(&Model::ReloadPixelShader, this, std::placeholders::_1), false);
	}
	if (aVertexShaderFileName != "")
	{
		myVSFileHandle = ourFileWatcher.RegisterCallback(aVertexShaderFileName, std::bind(&Model::ReloadVertexShader, this, std::placeholders::_1), false);
	}
#endif
	myFriendlyName = aFriendlyName;
	myIsLoaded = true;
}

void Model::ResetAndRelease()
{
	myIsLoaded = false;
	for (auto& i : myModelData.myLodLevels)
	{
		if (i)
		{
			i->myIndexBuffer->Release();
			for (size_t j = 0; j < i->myVertexBufferCount; j++)
			{
				i->myVertexBuffer[j]->Release();
			}
			SAFE_DELETE(i->myVertexBuffer);
			SAFE_DELETE(i);
		}
	}
	myModelData.myInputLayout->Release();
	for (auto& i : myModelData.myTextures)
	{
		i->Release();
		i = nullptr;
	}
	myBoneData.clear();
	myBoneNameLookup.clear();
}

Model::CModelData* Model::GetModelData()
{
	return &myModelData;
}

void Model::FlushChanges()
{
#if USEFILEWATHCER
	ourFileWatcher.FlushChanges();
#endif
}

std::string Model::GetFriendlyName()
{
	return myFriendlyName;
}

Model::LodLevel* Model::GetOptimalLodLevel(float aDistanceFromCameraSqr)
{
#if USEIMGUI
	if (myForceLodLevel)
	{
		if (myModelData.myLodLevels[myForcedLodLevel])
		{
			return myModelData.myLodLevels[myForcedLodLevel];
		}
	}
#endif // !_RETAIL


	int wanted = static_cast<int>(aDistanceFromCameraSqr / (myLodLevelDistance * myLodLevelDistance));
	wanted = CLAMP(0, NUMBEROFLODLEVELS - 1, wanted);//clamp
	if (myModelData.myLodLevels[wanted])
	{
		return myModelData.myLodLevels[wanted];
	}

	for (size_t i = 1; i < NUMBEROFLODLEVELS + 1; i++)
	{
		int below = wanted - static_cast<int>(i);
		int above = wanted + static_cast<int>(i);

		if (below >= 0 && myModelData.myLodLevels[below])
		{
			return myModelData.myLodLevels[below];
		}
		if (above < NUMBEROFLODLEVELS && myModelData.myLodLevels[above])
		{
			return myModelData.myLodLevels[above];
		}
	}

	return nullptr;
}
void Model::ApplyLodLevel(LodLevel* aLodLevel, size_t aLevel, float aSize)
{
	myGraphicSize = MAX(aSize, myGraphicSize);
	if (aLevel < NUMBEROFLODLEVELS)
	{
#if USEIMGUI
		if (myModelData.myLodLevels[aLevel])
		{
			SYSWARNING("Overwriting lod level " + std::to_string(aLevel) + " on model ", myFilePath);
			delete myModelData.myLodLevels[aLevel];
		}
#endif // !_RETAIL
		myModelData.myLodLevels[aLevel] = aLodLevel;
	}
	else
	{
		SYSERROR("Trying to add a lodlevel to a model beyond the capacity (" + std::to_string(aLevel) + ")", myFilePath);
	}
}

#if USEIMGUI
void Model::ImGuiNode(std::map<std::string, std::vector<std::string>>& aFiles)
{
	ImGui::Text(((std::string("Is loaded: ") + (myIsLoaded ? "true" : "false")).c_str()));
	ImGui::Text("Renders: ");
	ImGui::SameLine();
	ImGui::PushID("should render");
	ImGui::Checkbox("", &myShouldRender);
	ImGui::PopID();
	int facecount = 0;
	for (auto& i : myModelData.myLodLevels)
	{
		if (i)
		{
			facecount += i->myNumberOfIndexes / 3;
		}
	}
	ImGui::Text("Loaded LodLevels");
	for (size_t i = 0; i < NUMBEROFLODLEVELS; i++)
	{
		if (i != 0)
		{
			ImGui::SameLine();
		}
		bool loaded = !!myModelData.myLodLevels[i];
		ImGui::Checkbox("", &loaded);
	}
	ImGui::Checkbox("Force LodLevel", &myForceLodLevel);
	ImGui::SliderInt("Level", &myForcedLodLevel, 0, NUMBEROFLODLEVELS - 1);
	if (myForceLodLevel && myModelData.myLodLevels[myForcedLodLevel])
	{
		ImGui::Text("Faces on Lod: %d", myModelData.myLodLevels[myForcedLodLevel]->myNumberOfIndexes / 3);
	}
	else
	{
		ImGui::Text("Faces on Lod:");
	}

	size_t flags = myModelData.myshaderTypeFlags;
	if (flags & ShaderFlags::HasVertexColors)
	{
		ImGui::Text("Has VertexColors");
	}
	if (flags & ShaderFlags::HasUvSets)
	{
		ImGui::Text("Has Uv sets");
	}
	if (flags & ShaderFlags::HasBones)
	{
		ImGui::Text("Has bones");
		ImGui::DragFloat("Blending", &myTemporaryBlendValue, 0.01f, 0.0f, 1.0f);
	}
	if (myModelData.myForceForward)
	{
		ImGui::Text("Draws using forward renderer");
	}
	if (myModelData.myIsEffect)
	{
		ImGui::Text("Is an effect");
	}


	ImGui::Text(("Tricount: " + std::to_string(facecount)).c_str());
	ImGui::PushID("PS");
	if (ImGui::Button("Change"))
	{
		ImGui::OpenPopup("PS-Popup");
	}
	ImGui::SameLine();
#if USEFILEWATHCER
	ImGui::Text(("PixelShader: " + myPSFileHandle).c_str());
#endif


	if (ImGui::BeginPopup("PS-Popup"))
	{
		ImGui::Text("Found files");
		ImGui::Separator();
		for (auto& i : aFiles[".hlsl"])
		{
			if (ImGui::Selectable(i.c_str()))
			{
				ID3D11Device* device;
				myModelData.myPixelShader->operator ID3D11PixelShader* ()->GetDevice(&device);
				PixelShader* shader = GetPixelShader(device, i, myModelData.myshaderTypeFlags);

				if (shader)
				{
					myModelData.myPixelShader = shader;
#if USEFILEWATHCER
					ourFileWatcher.UnRegister(myPSFileHandle);
					myPSFileHandle = ourFileWatcher.RegisterCallback(i, std::bind(&Model::ReloadPixelShader, this, std::placeholders::_1));
#endif
				}
			}
		}
		ImGui::EndPopup();
	}

	ImGui::PopID();

	ImGui::PushID("VS");
	if (ImGui::Button("Change"))
	{
		ImGui::OpenPopup("VS-Popup");
	}
	ImGui::SameLine();
#if USEFILEWATHCER
	ImGui::Text(("VertexShader: " + myVSFileHandle).c_str());
#endif
	if (ImGui::BeginPopup("VS-Popup"))
	{
		ImGui::Text("Found files");
		ImGui::Separator();
		for (auto& i : aFiles[".hlsl"])
		{
			if (ImGui::Selectable(i.c_str()))
			{
				ID3D11Device* device;
				myModelData.myVertexShader->operator ID3D11VertexShader* ()->GetDevice(&device);
				std::vector<char> _;
				VertexShader* shader = GetVertexShader(device, i, _, myModelData.myshaderTypeFlags);
				if (ReloadVertexShader(i))
				{
					myModelData.myVertexShader = shader;
#if USEFILEWATHCER
					ourFileWatcher.UnRegister(myVSFileHandle);
					myVSFileHandle = ourFileWatcher.RegisterCallback(i, std::bind(&Model::ReloadVertexShader, this, std::placeholders::_1));
#endif
				}
			}
		}
		ImGui::EndPopup();
	}
	ImGui::PopID();

	float width = ImGui::GetWindowSize().x / 3;
	ImVec2 imagesize = ImVec2(width, width);
	ImGui::Columns(3);
	static const char* imageNames[3] = { "Albedo:","Normal:","Material:" };
	for (size_t i = 0; i < 3; i++)
	{
		ImGui::PushID(imageNames[i]);
		ImGui::Text(imageNames[i]);

		Tools::ZoomableImGuiImage(myModelData.myTextures[i], imagesize);

		
		if (ImGui::Button("Change"))
		{
			ImGui::OpenPopup("ImageSelector");
		}
		if (ImGui::BeginPopup("ImageSelector"))
		{
			ImGui::Text("Images");
			ImGui::Separator();
			for (auto& f : aFiles[".dds"])
			{
				if (ImGui::Selectable(f.c_str()))
				{
					ID3D11Device* device;
					myModelData.myTextures[i]->operator ID3D11ShaderResourceView *()->GetDevice(&device);
					Texture* image = LoadTexture(device, f);
					if (IsErrorTexture(image))
					{
						SYSWARNING("Could not load image: " , f);
					}
					myModelData.myTextures[i]->Release();
					myModelData.myTextures[i] = image;
				}
			}
			ImGui::EndPopup();
		}
		ImGui::NextColumn();
		ImGui::PopID();
	}
	ImGui::Separator();
}
#endif // !_RETAIL

float Model::GetGraphicSize()
{
	return myGraphicSize;
}

bool Model::ReloadPixelShader(const std::string& aFilePath)
{
	SYSINFO("Reloading: " + aFilePath);
	ID3D11Device* device;
	myModelData.myPixelShader->operator ID3D11PixelShader* ()->GetDevice(&device);
	::ReloadPixelShader(device, aFilePath, myModelData.myshaderTypeFlags);
	return true;
}

bool Model::ReloadVertexShader(const std::string& aFilePath)
{
	SYSINFO("Reloading: " + aFilePath);
	ID3D11Device* device;
	myModelData.myVertexShader->operator ID3D11VertexShader* ()->GetDevice(&device);
	::ReloadVertexShader(device, aFilePath, myModelData.myshaderTypeFlags);
	return true;
}
