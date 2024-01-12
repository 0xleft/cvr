#pragma once
#include <vector>
#include <string>

enum class FileMapLeafColor {
	WHITE,
	RED,
	ORANGE,
	BLACK,
	GREEN
};

class FileMapLeaf {
	FileMapLeaf* parent = nullptr;
	std::vector<FileMapLeaf*> children;
	std::string hash;
	std::string path;
	FileMapLeafColor color = FileMapLeafColor::BLACK;

public:
	FileMapLeaf(std::string hash, std::string path) {
		this->hash = hash;
		this->path = path;
	}

	void setParent(FileMapLeaf* parent) { this->parent = parent; }
	void addChild(FileMapLeaf* child) { this->children.push_back(child); }
	void setColor(FileMapLeafColor color) { this->color = color; }

	std::vector<FileMapLeaf*> getChildren() { return this->children; }
	FileMapLeaf* getParent() { return this->parent; }
	std::string getPath() { return this->path; }
	std::string getHash() { return this->hash; }
	FileMapLeafColor getColor() { return this->color; }

	~FileMapLeaf() {
		for (FileMapLeaf* child : this->children) {
			delete child;
		}
	}

	void compare(FileMapLeaf* base, FileMapLeaf* second, FileMapLeaf* output) {
		if (base->getHash() != second->getHash()) {
			FileMapLeaf* baseChild = new FileMapLeaf(base->getHash(), base->getPath());
			baseChild->setColor(FileMapLeafColor::ORANGE);
			output->addChild(baseChild);
		}

		for (FileMapLeaf* child : second->getChildren()) {
			bool found = false;
			for (FileMapLeaf* baseChild : base->getChildren()) {
				if (child->getPath() == baseChild->getPath()) {
					found = true;
					compare(baseChild, child, output);
					break;
				}
			}

			if (!found) {
				FileMapLeaf* newChild = new FileMapLeaf(child->getHash(), child->getPath());
				newChild->setColor(FileMapLeafColor::GREEN);
				output->addChild(newChild);
			}
		}

		for (FileMapLeaf* child : base->getChildren()) {
			bool found = false;
			for (FileMapLeaf* secondChild : second->getChildren()) {
				if (child->getPath() == secondChild->getPath()) {
					found = true;
					break;
				}
			}

			if (!found) {
				FileMapLeaf* newChild = new FileMapLeaf(child->getHash(), child->getPath());
				newChild->setColor(FileMapLeafColor::RED);
				output->addChild(newChild);
			}
		}
	}
};

class FileMap {
	FileMapLeaf* root;

public:
	void setRoot(FileMapLeaf* root) {
		this->root = root;
	}

	FileMapLeaf* getRoot() { return this->root; }

	~FileMap() {
		delete this->root;
	}
};