// windows.h has many of the functions needed to get running on Windows
#include <windows.h>

// WinMain is the entry point for C/C++ applications made on Windows
int WINAPI WinMain(
  HINSTANCE hInstance,
  HINSTANCE hPrevInstance,
  LPSTR     lpCmdLine,
  int       nCmdShow
) {

  // GetModuleHandle(0);

  MessageBox(
    0,
    (LPCTSTR)"Welcome to the Archive Engine",
    (LPCTSTR)"The Archive Engine",
    MB_OK | MB_ICONINFORMATION
  );

  return 0;
};



