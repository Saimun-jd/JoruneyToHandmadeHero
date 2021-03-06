#include <windows.h>
#include <stdint.h>

typedef int8_t i8;
typedef int16_t i16;
typedef int64_t i64;
typedef int32_t i32;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

#define global_variable static
#define interval static

struct win32_offscreen_buffer {
    void *Memory;
    BITMAPINFO Info;
    int BytesPerPixel;
    int Width;
    int Height;
};

struct win32_window_dimension {
    int Width;
    int Height;
};

global_variable bool Running = true;
global_variable win32_offscreen_buffer GlobalBackBuffer;

win32_window_dimension Win32GetWindowDimension(HWND);

void RenderWeirdGradeint(win32_offscreen_buffer*, int, int);

void Win32ResizeDIBSection(win32_offscreen_buffer*, int, int);

void Win32UpdateWindow(HDC, int, int, win32_offscreen_buffer*);

LRESULT CALLBACK Win32MainWindowCallback(HWND, UINT, WPARAM, LPARAM);

int CALLBACK WinMain(
    HINSTANCE Instance,
    HINSTANCE PrevInstance,
    LPSTR     CmdLine,
    int       ShowCmd
) {
    WNDCLASS WindowClass = {};

    // TODO: Check if we need these
    WindowClass.style = CS_HREDRAW | CS_VREDRAW;

    WindowClass.lpfnWndProc = Win32MainWindowCallback;
    WindowClass.hInstance = Instance;
    WindowClass.lpszClassName = "HandmadeHeroWindowClass";

    if(RegisterClass(&WindowClass)) {
        HWND Window = CreateWindowEx(
        0,
        "HandmadeHeroWindowClass",
        "Handmade Hero",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        0,
        0,
        Instance,
        0
        );

    if(Window) {
        int XOffset = 0;
        int YOffset = 0;

        Win32ResizeDIBSection(&GlobalBackBuffer, 1280, 720);

        while(Running) {
            MSG Message = {};

            while(PeekMessage(&Message, 0, 0, 0, PM_REMOVE)) {
                TranslateMessage(&Message);
                DispatchMessage(&Message);
            }

            RenderWeirdGradeint(&GlobalBackBuffer, XOffset, YOffset);
            HDC DeviceContext = GetDC(Window);
            win32_window_dimension Dimension = Win32GetWindowDimension(Window);
            Win32UpdateWindow(
                DeviceContext,
                Dimension.Width,
                Dimension.Height,
                &GlobalBackBuffer
            );
            ReleaseDC(Window, DeviceContext);

            XOffset++;
            YOffset += 2;
        }
    } else {
      // TODO: Logging
    }
    } else {
    // TODO: Logging
    }

    return 0;
}

win32_window_dimension Win32GetWindowDimension(HWND Window) {
    win32_window_dimension Result;
    RECT ClientRect;
    GetClientRect(Window, &ClientRect);
    Result.Width = ClientRect.right - ClientRect.left;
    Result.Height = ClientRect.bottom - ClientRect.top;
    return Result;
}

void RenderWeirdGradeint(win32_offscreen_buffer *Buffer, int XOffset, int YOffset) {
  u8 *Row = (u8 *)Buffer->Memory;

    for(int Y = 0; Y < Buffer->Height; Y++) {
        u32 *Pixel = (u32 *)Row;
        for(int X = 0; X < Buffer->Width; X++) {
            u8 Blue = X + XOffset;
            u8 Green = Y + YOffset;
            // 0xXXRRGGBB
            *Pixel++ = (Green << 8) | Blue;
        }
        Row = Row + Buffer->Width * Buffer->BytesPerPixel;
    }
}

void Win32ResizeDIBSection(win32_offscreen_buffer *Buffer, int Width, int Height) {
    if(Buffer->Memory) 
        VirtualFree(Buffer->Memory, 0, MEM_RELEASE);

    Buffer->Width = Width;
    Buffer->Height = Height;
    Buffer->BytesPerPixel = 4;

    Buffer->Info.bmiHeader.biSize = sizeof(Buffer->Info.bmiHeader);
    Buffer->Info.bmiHeader.biWidth = Buffer->Width;
    Buffer->Info.bmiHeader.biHeight = -Buffer->Height;
    Buffer->Info.bmiHeader.biPlanes = 1;
    Buffer->Info.bmiHeader.biBitCount = Buffer->BytesPerPixel * 8;
    Buffer->Info.bmiHeader.biCompression = BI_RGB;

    int BitmapSize = Buffer->Width * Buffer->Height * Buffer->BytesPerPixel;

    Buffer->Memory = VirtualAlloc(0, BitmapSize, MEM_COMMIT, PAGE_READWRITE);
}

void Win32UpdateWindow(
    HDC DeviceContext,
    int WindowWidth,
    int WindowHeight,
    win32_offscreen_buffer *Buffer
) {
    StretchDIBits(
        DeviceContext,
        0, 0, WindowWidth, WindowHeight, // dest
        0, 0, Buffer->Width, Buffer->Height, // src
        Buffer->Memory,
        &Buffer->Info,
        DIB_RGB_COLORS,
        SRCCOPY
    );
}

LRESULT CALLBACK Win32MainWindowCallback(
    HWND Window,
    UINT Message,
    WPARAM WParam,
    LPARAM LParam
) {
    LRESULT Result = 0;
    switch(Message) {
        case WM_ACTIVATEAPP:
        {
            OutputDebugStringA("WM_ACTIVATEAPP\n");
        } break;

        case WM_CLOSE:
        {
        Running = false;
            OutputDebugStringA("WM_CLOSE\n");
        } break;

        case WM_DESTROY:
        {
            OutputDebugStringA("WM_DESTROY\n");
        } break;

        case WM_PAINT:
        {
            PAINTSTRUCT PaintStruct = {};
            HDC DeviceContext = BeginPaint(Window, &PaintStruct);
            win32_window_dimension Dimension = Win32GetWindowDimension(Window);
            Win32UpdateWindow(
                DeviceContext,
                Dimension.Width,
                Dimension.Height,
                &GlobalBackBuffer
            );
            EndPaint(Window, &PaintStruct);
        } break;

        case WM_SIZE:
        {
            OutputDebugStringA("WM_SIZE\n");
        } break;

        default:
        {
            Result = DefWindowProc(Window, Message, WParam, LParam);
        } break;
    }

    return Result;
}