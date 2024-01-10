#pragma once

#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h>

#include <ImGuiFileDialog.h>

#include <string>
#include <format>
#include <json.hpp>
#include "FileMap.h"

class SysdiffViewer {

private:
	nlohmann::json base = NULL;
	nlohmann::json second = NULL;
	FileMap* fileMap = new FileMap();

public:
	void setBase(nlohmann::json base) { this->base = base; }
	void setSecond(nlohmann::json second) { this->second = second; }
	void clearFileMap() { delete this->fileMap; this->fileMap = new FileMap(); }

	nlohmann::json getBase() { return this->base; }
	nlohmann::json getSecond() { return this->second; }

	void displayMap() {
		ImGui::BeginChild("Diff viewer");

		ImGui::EndChild();
	}

	void loadMap() {
		if (this->base == NULL || this->second == NULL) { return; }

		try {
			
		}
		catch (...) { return; }
	}

	~SysdiffViewer() {
		delete this->fileMap;
	}
};