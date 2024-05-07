#include "FileDialog.h"

#include <Windows.h>
#include <commdlg.h>


std::string FileDialog::OpenFile(const char* filter)
{
    OPENFILENAMEA ofn;
    CHAR sizeFile[264] = { 0 };

    ZeroMemory(&ofn, sizeof(OPENFILENAME));
    ofn.lStructSize = sizeof(OPENFILENAME);   
    ofn.hwndOwner = NULL;   //main window handle? - GL getwindow?
    ofn.lpstrFilter = filter;
    ofn.nFilterIndex = 1;
    ofn.lpstrFile = sizeFile;
    ofn.nMaxFile = sizeof(sizeFile);
    ofn.lpstrTitle = "Open File";
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

    if (GetOpenFileNameA(&ofn) == TRUE)
        return ofn.lpstrFile;

    return std::string();
}

std::string FileDialog::SaveFile(const char* filter)
{
    OPENFILENAMEA ofn;
    CHAR sizeFile[264] = { 0 };

    ZeroMemory(&ofn, sizeof(OPENFILENAME));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = NULL;   //main window handle? - GL getwindow?
    ofn.lpstrFilter = filter;
    ofn.nFilterIndex = 1;
    ofn.lpstrFile = sizeFile;
    ofn.nMaxFile = sizeof(sizeFile);
    ofn.lpstrTitle = "Save File";
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

    if (GetSaveFileNameA(&ofn) == TRUE)
        return ofn.lpstrFile;

    return std::string();
}