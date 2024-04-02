#include "Prefab.h"

//#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

Prefab::Prefab(uint pId, std::string pName) : id(pId), name(pName) {}

Prefab::~Prefab() {}

void Prefab::SerializePrefab(std::string path)
{
	std::string fileNameExt = name + ".prefab";

	fs::path filename = path + "/" + fileNameExt; //Edit depending if the bar is already written

	json prefabJSON;
}
