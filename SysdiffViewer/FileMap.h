#pragma once
#include <vector>
#include <string>

class FileMapLeaf {
	FileMapLeaf* parent = nullptr;
	std::vector<FileMapLeaf*> children;
	std::string hash;
	std::string path;

public:
	FileMapLeaf(std::string hash, std::string path) {
		this->hash = hash;
		this->path = path;
	}

	void setParent(FileMapLeaf* parent) { this->parent = parent; }
	void addChild(FileMapLeaf* child) { this->children.push_back(child); }

	std::vector<FileMapLeaf*> getChildren() { return this->children; }
	FileMapLeaf* getParent() { return this->parent; }
	std::string getPath() { return this->path; }

	~FileMapLeaf() {
		for (FileMapLeaf* child : this->children) {
			delete child;
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