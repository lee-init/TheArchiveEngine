// windows.h has many of the functions needed to get running on Windows
#include <windows.h>

// WNDPROC MainWindowCallback;

// Static has 3 different meanings. Re-define static 3 different ways
#define INTERNAL static
#define LOCAL_PERSIST static
#define GLOBAL_VARIABLE static

// TODO: This is a global variable for now

/*
  Using static for global variables always initializes it to 0

  ```static bool Running = 0; == static bool Running;```

  These two statements are identical.
*/

GLOBAL_VARIABLE bool Running;

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

  case WM_CLOSE:
  {
    // TODO: Handle this with a message to the user (E.g., "Are you sure you want to quit?")
    Running = false;
    // OutputDebugStringA("WM_CLOSE\n");
  } break;

  case WM_ACTIVATEAPP:
  {
    OutputDebugStringA("WM_ACTIVATEAPP\n");
  } break;

  case WM_DESTROY:
  {
    // TODO: Handle this as an error - recreate window?
    Running = false;
    // OutputDebugStringA("WM_DESTROY\n");
  } break;

  case WM_PAINT:
  {
    PAINTSTRUCT Painter;
    HDC DeviceContext = BeginPaint(Window, &Painter);
    int X = Painter.rcPaint.left;
    int Y = Painter.rcPaint.top;
    LONG Height = Painter.rcPaint.bottom - Painter.rcPaint.top;
    LONG Width = Painter.rcPaint.right - Painter.rcPaint.left;
    // Locally persisted variables keep the value it's been assigned rather than resetting to the default every time.
    LOCAL_PERSIST DWORD Opperation = WHITENESS;
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
      Running = true;
      while (Running) {
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



