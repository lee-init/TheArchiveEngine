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

// TODO: This is a global variable for now
GLOBAL_VARIABLE bool Running;
GLOBAL_VARIABLE BITMAPINFO BitmapInfo;
// void* - A pointer to nothing. Means we're pointing to the memory address of a particular value but we don't know what type it is. 
// Can be cast to a type later
GLOBAL_VARIABLE void* BitmapMemory;
GLOBAL_VARIABLE int BitmapWidth;
GLOBAL_VARIABLE int BitmapHeight;
GLOBAL_VARIABLE int BytesPerPixel = 4;

typedef uint8_t uint8;
typedef uint32_t uint32;

INTERNAL void RenderWeirdGradient(int BlueOffset, int GreenOffset) {

  int Width = BitmapWidth;
  int Height = BitmapHeight;

  int Stride = Width * BytesPerPixel;
  uint8* Row = (uint8*)BitmapMemory;
  for (int Y = 0; Y < BitmapHeight; ++Y) {
    uint32* Pixel = (uint32*)Row;
    for (int X = 0; X < BitmapWidth; ++X) {
      // Need to point to the memory pointed to by the *Pixel pointer

      uint8 Blue = (X + BlueOffset);
      uint8 Green = (Y + GreenOffset);
      uint8 Red = 155;
      *Pixel++ = ((Red << 16) | (Green << 8) | Blue);

    }
    Row += Stride;

  }
}

INTERNAL void Win32ResizeDIBSection(int Width, int Height) {

  // TODO: Bulletproof this
  // Maybe don't free memory used first, free after, then free first if that fails.

  if (BitmapMemory) {
    VirtualFree(BitmapMemory, 0, MEM_RELEASE);
  }

  BitmapWidth = Width;
  BitmapHeight = Height;

  // The number of bites requred by the structure - we use sizeof() to find the size of the stdructure itself
  BitmapInfo.bmiHeader.biSize = sizeof(BitmapInfo.bmiHeader);
  // Width of the bitmap in pixels
  BitmapInfo.bmiHeader.biWidth = BitmapWidth;
  // Height of the bitmap in pixels
  BitmapInfo.bmiHeader.biHeight = -BitmapHeight;
  // Doesn't matter what this is, it has to have a value of 1
  BitmapInfo.bmiHeader.biPlanes = 1;
  // bits per pizel (bpp) - values are 1, 4, 8, 16, 24, 32. 32 is true colour with optional alpha (4.29 billion colours). Setting 32 will set DWORD aligned which is... something idk
  BitmapInfo.bmiHeader.biBitCount = 32;
  // We don't want to compress at all, uncompressed means we can write to and blt to is as fast as possible. BI_RGB is for uncompressed RGB.
  BitmapInfo.bmiHeader.biCompression = BI_RGB;

  // This value needs to be 
  int BitmapMemorySize = (BitmapWidth * BitmapHeight) * BytesPerPixel;
  BitmapMemory = VirtualAlloc(0, BitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);

  RenderWeirdGradient(0, 0);

}

INTERNAL void Win32UpdateWindow(HDC DeviceContext, RECT* ClientRect, int X, int Y, int Width, int Height) {

  int WindowWidth = ClientRect->right - ClientRect->left;
  int WindowHeight = ClientRect->bottom - ClientRect->top;

  StretchDIBits(DeviceContext,
    // X, Y, Width, Height,
    // X, Y, Width, Height,
    // Dirty Rectangle - Fill whole window
    0, 0, WindowWidth, WindowHeight,
    0, 0, BitmapWidth, BitmapHeight,
    BitmapMemory,
    &BitmapInfo,
    DIB_RGB_COLORS,
    SRCCOPY);

}

LRESULT CALLBACK Win32MainWindowCallback(
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
    RECT ClientRect;
    GetClientRect(Window, &ClientRect);
    int Width = ClientRect.right - ClientRect.left;
    int Height = ClientRect.bottom - ClientRect.top;
    Win32ResizeDIBSection(Width, Height);
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
    int X = Painter.rcPaint.left;
    int Y = Painter.rcPaint.top;
    int Width = Painter.rcPaint.right - Painter.rcPaint.left;
    int Height = Painter.rcPaint.bottom - Painter.rcPaint.top;

    RECT ClientRect;
    GetClientRect(Window, &ClientRect);

    Win32UpdateWindow(DeviceContext, &ClientRect, X, Y, Width, Height);
    // Locally persisted variables keep the value it's been assigned rather than resetting to the default every time.
    LOCAL_PERSIST DWORD Opperation = WHITENESS;

    // Remember to remove PatBlt. This is overwriting the pixels with Whiteness
    // PatBlt(DeviceContext, X, Y, Width, Height, Opperation);
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
  WindowClass.lpfnWndProc = Win32MainWindowCallback;
  WindowClass.hInstance = hInstance;
  // WindowClass.hIcon
  WindowClass.lpszClassName = "TheArchivesWindowClass";

  if (RegisterClass(&WindowClass)) {
    HWND Window = CreateWindowEx(
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

    if (Window) {
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

        RenderWeirdGradient(XOffset, YOffset);

        HDC DeviceContext = GetDC(Window);
        RECT ClientRect;
        GetClientRect(Window, &ClientRect);
        int WindowWidth = ClientRect.right - ClientRect.left;
        int WindowHeight = ClientRect.bottom - ClientRect.top;
        Win32UpdateWindow(DeviceContext, &ClientRect, 0, 0, WindowWidth, WindowHeight);
        ReleaseDC(Window, DeviceContext);

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



