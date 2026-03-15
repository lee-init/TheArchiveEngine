// windows.h has many of the functions needed to get running on Windows
#include <windows.h>

// WNDPROC MainWindowCallback;

LRESULT CALLBACK MainWindowCallback(
  HWND Window,
  UINT Message,
  WPARAM WParam,
  LPARAM LParam
) {

  // Assuming the message is handled
  LRESULT Result = 0;

  switch (Message)
  {
  case WM_SIZE:
  {
    OutputDebugStringA("WM_SIZE\n");
  } break;

  case WM_DESTROY:
  {
    OutputDebugStringA("WM_DESTROY\n");
  } break;

  case WM_CLOSE:
  {
    OutputDebugStringA("WM_CLOSE\n");
  } break;

  case WM_ACTIVATEAPP:
  {
    OutputDebugStringA("WM_ACTIVATEAPP\n");
  } break;

  case WM_PAINT:
  {
    PAINTSTRUCT Painter;
    HDC DeviceContext = BeginPaint(Window, &Painter);
    int X = Painter.rcPaint.left;
    int Y = Painter.rcPaint.top;
    LONG Height = Painter.rcPaint.bottom - Painter.rcPaint.top;
    LONG Width = Painter.rcPaint.right - Painter.rcPaint.left;
    static DWORD Opperation = WHITENESS;
    PatBlt(DeviceContext, X, Y, Width, Height, Opperation);
    if (Opperation == WHITENESS) {
      Opperation = BLACKNESS;
    }
    else {
      Opperation = WHITENESS;
    }
    EndPaint(Window, &Painter);
  } break;
  default:
  {
    // OutputDebugStringA("default\n");
    Result = DefWindowProc(Window, Message, WParam, LParam);
  } break;
  }

  return Result;

}

// WinMain is the entry point for C/C++ applications made on Windows
int WINAPI WinMain(
  HINSTANCE hInstance,
  HINSTANCE hPrevInstance,
  LPSTR     lpCmdLine,
  int       nCmdShow
) {

  WNDCLASS WindowClass = {};

  // TODO: check if HREDRAW/VREDRAW still matter
  WindowClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
  WindowClass.lpfnWndProc = MainWindowCallback;
  WindowClass.hInstance = hInstance;
  // WindowClass.hIcon
  WindowClass.lpszClassName = "TheArchivesWindowClass";

  if (RegisterClass(&WindowClass)) {
    HWND WindowHandle = CreateWindowEx(
      0,
      WindowClass.lpszClassName,
      "The Archives",
      WS_OVERLAPPEDWINDOW | WS_VISIBLE,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      0,
      0,
      hInstance,
      0
    );

    if (WindowHandle) {
      // ;; means blank feilds. It's an infinite loop
      for (;;) {
        MSG Message;
        BOOL MessageResult = GetMessage(&Message, 0, 0, 0);
        if (MessageResult > 0) {
          TranslateMessage(&Message);
          DispatchMessage(&Message);
        }
        else {
          break;
        }
      }
    }
    else {
      // TODO: Logging Error Handle
    }

  }
  else {
    // TODO: Logging Error Handle
  }

  return 0;
};



