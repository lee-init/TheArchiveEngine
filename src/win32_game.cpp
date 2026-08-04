// windows.h has many of the functions needed to get running on Windows
#include <windows.h>
#include <stdint.h>

// Static has 3 different meanings. Re-define static 3 different ways
#define INTERNAL static
#define LOCAL_PERSIST static
#define GLOBAL_VARIABLE static

/*
Using static for global variables always initializes it to 0

```static bool Running = 0; == static bool Running;```

These two statements are identical.
*/

struct win32_offscreen_buffer {
  BITMAPINFO Info;
  // void* - A pointer to nothing. Means we're pointing to the memory address of a particular value but we don't know what type it is. Can be cast to a type later
  void* Memory;
  int Width;
  int Height;
  int Stride;
  // NOTE: Pixels are always 32-bits wide. Memory Order BB GG RR XX.
};

// TODO: This is a global variable for now
GLOBAL_VARIABLE bool Running;
GLOBAL_VARIABLE win32_offscreen_buffer GlobalBackbuffer;

struct win32_window_dimensions {
  int Width;
  int Height;
};

win32_window_dimensions Win32GetWindowDimension(HWND Window) {

  win32_window_dimensions Result;

  RECT ClientRect;
  GetClientRect(Window, &ClientRect);
  Result.Width = ClientRect.right - ClientRect.left;
  Result.Height = ClientRect.bottom - ClientRect.top;

  return(Result);
}

typedef uint8_t uint8;
typedef uint32_t uint32;

INTERNAL void RenderWeirdGradient(win32_offscreen_buffer Buffer, int BlueOffset, int GreenOffset) {

  uint8* Row = (uint8*)Buffer.Memory;
  for (int Y = 0; Y < Buffer.Height; ++Y) {
    uint32* Pixel = (uint32*)Row;
    for (int X = 0; X < Buffer.Width; ++X) {
      // Need to point to the memory pointed to by the *Pixel pointer
      uint8 Blue = (X + BlueOffset);
      uint8 Green = (Y + GreenOffset);
      uint8 Red = 155;
      *Pixel++ = ((Red << 16) | (Green << 8) | Blue);

    }
    Row += Buffer.Stride;
  }
}

INTERNAL void Win32ResizeDIBSection(win32_offscreen_buffer* Buffer, int Width, int Height) {

  // TODO: Bulletproof this
  // Maybe don't free memory used first, free after, then free first if that fails.

  if (Buffer->Memory) {
    VirtualFree(Buffer->Memory, 0, MEM_RELEASE);
  }

  int BytesPerPixel = 4;

  Buffer->Width = Width;
  Buffer->Height = Height;

  Buffer->Info.bmiHeader.biSize = sizeof(Buffer->Info.bmiHeader);
  Buffer->Info.bmiHeader.biWidth = Buffer->Width;
  Buffer->Info.bmiHeader.biHeight = -Buffer->Height;
  Buffer->Info.bmiHeader.biPlanes = 1;
  Buffer->Info.bmiHeader.biBitCount = 32;
  Buffer->Info.bmiHeader.biCompression = BI_RGB;

  // This value needs to be 
  int BitmapMemorySize = (Buffer->Width * Buffer->Height) * BytesPerPixel;
  Buffer->Memory = VirtualAlloc(0, BitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);
  Buffer->Stride = Width * BytesPerPixel;

  RenderWeirdGradient(GlobalBackbuffer, 0, 0);
}

INTERNAL void Win32DisplayBufferInWindow(win32_offscreen_buffer Buffer, HDC DeviceContext, int WindowWidth, int WindowHeight) {

  // TODO: Correct the aspect ratio
  // TODO: Play with stretch modes
  StretchDIBits(DeviceContext,
    0, 0, WindowWidth, WindowHeight,
    0, 0, Buffer.Width, Buffer.Height,
    Buffer.Memory,
    &Buffer.Info,
    DIB_RGB_COLORS,
    SRCCOPY);
}

LRESULT CALLBACK Win32MainWindowCallback(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam) {

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
  } break;

  case WM_ACTIVATEAPP:
  {
    OutputDebugStringA("WM_ACTIVATEAPP\n");
  } break;

  case WM_DESTROY:
  {
    // TODO: Handle this as an error - recreate window?
    Running = false;
  } break;

  case WM_PAINT:
  {
    PAINTSTRUCT Painter;
    HDC DeviceContext = BeginPaint(Window, &Painter);
    win32_window_dimensions Dimension = Win32GetWindowDimension(Window);
    Win32DisplayBufferInWindow(GlobalBackbuffer, DeviceContext, Dimension.Width, Dimension.Height);
    EndPaint(Window, &Painter);
  } break;
  default:
  {
    Result = DefWindowProc(Window, Message, WParam, LParam);
  } break;
  }

  return Result;
}

// WinMain is the entry point for C/C++ applications made on Windows
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

  WNDCLASS WindowClass = {};

  Win32ResizeDIBSection(&GlobalBackbuffer, 1280, 720);

  WindowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
  WindowClass.lpfnWndProc = Win32MainWindowCallback;
  WindowClass.hInstance = hInstance;
  // WindowClass.hIcon
  WindowClass.lpszClassName = TEXT("TheArchivesWindowClass");

  if (RegisterClass(&WindowClass)) {
    HWND Window = CreateWindowEx(
      0,
      WindowClass.lpszClassName,
      TEXT("The Archives"),
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

    if (Window) {

      HDC DeviceContext = GetDC(Window);
      int XOffset = 0;
      int YOffset = 0;
      Running = true;

      while (Running) {
        MSG Message;
        while (PeekMessage(&Message, 0, 0, 0, PM_REMOVE)) {
          // Handle quit message
          if (Message.message == WM_QUIT) {
            Running = false;
          }

          TranslateMessage(&Message);
          DispatchMessage(&Message);
        }
        RenderWeirdGradient(GlobalBackbuffer, XOffset, YOffset);
        win32_window_dimensions Dimension = Win32GetWindowDimension(Window);
        Win32DisplayBufferInWindow(GlobalBackbuffer, DeviceContext, Dimension.Width, Dimension.Height);

        ++XOffset;

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



