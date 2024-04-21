#pragma once

#include <string>

enum Filter
{
	//...?
};

class FileDialog
{
public:
	/// <summary>
	///	Opens a windows open file dialog
	/// </summary>
	/// <param name="filter"></param>
	/// <returns></returns>
	static std::string OpenFile(const char* filter);

	/// <summary>
	/// Opens a windows save file dialog
	/// </summary>
	/// <param name="filter"></param>
	/// <returns></returns>
	static std::string SaveFile(const char* filter);
};