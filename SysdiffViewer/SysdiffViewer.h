#pragma once

// #pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

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
	FileMap* fileMapBase = new FileMap();
	FileMap* fileMapSecond = new FileMap();
	FileMap* fileMapDiff = new FileMap();

public:
	void setBase(nlohmann::json base) { this->base = base; }
	void setSecond(nlohmann::json second) { this->second = second; }
	void clearFileMap() { 
		delete this->fileMapBase; 
		this->fileMapBase = new FileMap(); 
		delete this->fileMapSecond; 
		this->fileMapSecond = new FileMap();
		delete this->fileMapDiff;
		this->fileMapDiff = new FileMap();
	}

	nlohmann::json getBase() { return this->base; }
	nlohmann::json getSecond() { return this->second; }

	void displayChildren(FileMapLeaf* parent) {
		for (FileMapLeaf* child : parent->getChildren()) {
			ImVec4 color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
			switch (child->getColor()) {
				case FileMapLeafColor::BLACK:
					color = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
					break;
				case FileMapLeafColor::RED:
					color = ImVec4(0.5f, 0.0f, 0.0f, 1.0f);
					break;
				case FileMapLeafColor::ORANGE:
					color = ImVec4(0.5f, 0.5f, 0.0f, 1.0f);
					break;
				case FileMapLeafColor::GREEN:
					color = ImVec4(0.0f, 0.5f, 0.0f, 1.0f);
					break;
			}

			
			ImGui::PushStyleColor(ImGuiCol_Text, color);
			
			if (ImGui::TreeNode(child->getPath().c_str())) {
				displayChildren(child);
				ImGui::TreePop();
			}
			if (ImGui::IsItemHovered()) {
				//ImGui::BeginTooltip();
				//ImGui::Text(std::format("Hash: {}", child->getHash()).c_str());
				//ImGui::EndTooltip();
			}
			
			ImGui::PopStyleColor();
		}
	}

	void displayDiff() {
		ImGui::BeginChild("Diff");
		if (this->fileMapDiff->getRoot() == NULL) { ImGui::EndChild(); return; }
		displayChildren(this->fileMapDiff->getRoot());
		ImGui::EndChild();
	}

	void displayBaseMap() {
		ImGui::BeginChild("Base map");
		displayChildren(this->fileMapBase->getRoot());
		ImGui::EndChild();
	}

	void displaySecondMap() {
		ImGui::BeginChild("Second map");
		displayChildren(this->fileMapSecond->getRoot());
		ImGui::EndChild();
	}

	void loadDiffSection(nlohmann::json baseSection, nlohmann::json secondSection, FileMapLeaf* parent) {
		for (nlohmann::json secondChild : secondSection) {
			try {
				nlohmann::json baseChild = baseSection[secondChild["path"]];

				if (baseChild["hash"] == secondChild["hash"]) {
					FileMapLeaf* leaf = new FileMapLeaf(secondChild["hash"], secondChild["path"]);
					leaf->setParent(parent);
					parent->addChild(leaf);

					loadDiffSection(baseChild["children"], secondChild["children"], leaf);
				}
				else {
					FileMapLeaf* leaf = new FileMapLeaf(secondChild["hash"], secondChild["path"]);
					leaf->setParent(parent);
					leaf->setColor(FileMapLeafColor::ORANGE);
					parent->addChild(leaf);

					loadDiffSection(baseChild["children"], secondChild["children"], leaf);
				}
			}
			catch (...) {
				FileMapLeaf* leaf = new FileMapLeaf(secondChild["hash"], secondChild["path"]);
				leaf->setParent(parent);
				leaf->setColor(FileMapLeafColor::GREEN);
				parent->addChild(leaf);
			}
		}

		for (nlohmann::json baseChild : baseSection) {
			try {
				nlohmann::json secondChild = secondSection[baseChild["path"]];
			}
			catch (...) { 
				try {
					FileMapLeaf* leaf = new FileMapLeaf(baseChild["hash"], baseChild["path"]);
					leaf->setParent(parent);
					leaf->setColor(FileMapLeafColor::RED);
					parent->addChild(leaf);
				} catch (...) { continue;
				}
			}
		}
	}

	void loadDiff() {
		if (this->base == NULL || this->second == NULL) { return; }
		this->fileMapDiff->~FileMap();
		this->fileMapDiff = new FileMap();

		FileMapLeaf* root = new FileMapLeaf(this->base["hash"], this->base["path"]);

		loadDiffSection(this->base["children"], this->second["children"], root);

		this->fileMapDiff->setRoot(root);
	}

	void loadSection(nlohmann::json section, FileMapLeaf* parent) {
		for (nlohmann::json::iterator it = section.begin(); it != section.end(); ++it) {
			try {
				nlohmann::json child = *it;

				FileMapLeaf* leaf = new FileMapLeaf(child["hash"], child["path"]);
				leaf->setParent(parent);
				parent->addChild(leaf);

				loadSection(child["children"], leaf);
			} catch (...) { continue; }
		}
	}

	void loadBaseMap() {
		if (this->base == NULL) { return; }

		try {
			FileMapLeaf* root = new FileMapLeaf(this->base["hash"], this->base["path"]);
			this->fileMapBase->setRoot(root);

			loadSection(this->base["children"], root);
		}
		catch (...) { this->base = NULL; return; }
	}

	void loadSecondMap() {
		if (this->second == NULL) { return; }

		try {
			FileMapLeaf* root = new FileMapLeaf(this->second["hash"], this->second["path"]);
			this->fileMapSecond->setRoot(root);

			loadSection(this->second["children"], root);
		}
		catch (...) { this->second = NULL; return; }
	}

	~SysdiffViewer() {
		delete this->fileMapBase;
		delete this->fileMapSecond;
	}
};