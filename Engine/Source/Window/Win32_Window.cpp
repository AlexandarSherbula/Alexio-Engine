#include "aio_pch.hpp"
#include "Win32_Window.hpp"

#include "Input/Input.hpp"

#include <imgui.h>

#if defined (AIO_WINDOWS)
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#include "Renderer/DX11/DX11_Context.hpp"

namespace aio
{
	Win32_Window::Win32_Window(const WindowSpecifications& windowSpec)
	{
		mSpecs.title = windowSpec.title;
		mSpecs.width = mProjectionSize.x = windowSpec.width;
		mSpecs.height = mProjectionSize.y = windowSpec.height;
		mSpecs.vSync = windowSpec.vSync;
		mSpecs.eventCallback = windowSpec.eventCallback;
		mSpecs.isFullScreen = windowSpec.isFullScreen;
		mSpecs.startMaximized = windowSpec.startMaximized;
		
		mWindowClass = "Win32 Class";
		m_hInstance = GetModuleHandle(nullptr);

		WNDCLASSEX wc = {};
		wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		wc.lpfnWndProc = WndProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = m_hInstance;
		wc.hIcon = NULL;
		wc.hIconSm = NULL;
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = NULL;
		wc.lpszMenuName = NULL;
		wc.lpszClassName = mWindowClass;
		wc.cbSize = sizeof(WNDCLASSEX);

		AIO_ASSERT(RegisterClassEx(&wc), "Failed to register window class.");

		mSpecs.width  = mProjectionSize.x * mSpecs.pixelWidth;
		mSpecs.height = mProjectionSize.y * mSpecs.pixelHeight;

		mPosition = { GetSystemMetrics(SM_CXSCREEN) / 2.0f - mSpecs.width / 2.0f, GetSystemMetrics(SM_CYSCREEN) / 2.0f - mSpecs.height / 2.0f };

		mWindowRect.left = mPosition.x;
		mWindowRect.top = mPosition.y;
		mWindowRect.right = mWindowRect.left + mSpecs.width;
		mWindowRect.bottom = mWindowRect.top + mSpecs.height;
		AIO_ASSERT(AdjustWindowRect(&mWindowRect, WS_OVERLAPPEDWINDOW, FALSE), "Failed to adjust window");

		mHandle = CreateWindowEx(WS_EX_APPWINDOW, mWindowClass, mSpecs.title, WS_OVERLAPPEDWINDOW,
			mWindowRect.left, mWindowRect.top, mWindowRect.right - mWindowRect.left, mWindowRect.bottom - mWindowRect.top,
			NULL, NULL, m_hInstance, &mSpecs);

		AIO_ASSERT(mHandle, "Failed to create a Window: {0}", ResultInfo(GetLastError()));

		if (mSpecs.isFullScreen)
		{
			SetWindowLong(mHandle, GWL_STYLE, WS_POPUP);
			MONITORINFO mi = { sizeof(mi) };
			if (GetMonitorInfo(MonitorFromWindow(mHandle,
				MONITOR_DEFAULTTOPRIMARY), &mi))
			{
				GetWindowRect(mHandle, &mWindowRect);
				SetWindowPos(mHandle, HWND_TOP,
					mi.rcMonitor.left, mi.rcMonitor.top,
					mi.rcMonitor.right - mi.rcMonitor.left,
					mi.rcMonitor.bottom - mi.rcMonitor.top,
					SWP_SHOWWINDOW);
			}
		}

		AIO_ASSERT(mHandle, "Failed to create a Window: {0}", ResultInfo(GetLastError()));

		BOOL dark = TRUE;
		DwmSetWindowAttribute(mHandle, DWMWA_USE_IMMERSIVE_DARK_MODE, &dark, sizeof(dark));

		MARGINS margins = { 0 };
		DwmExtendFrameIntoClientArea(mHandle, &margins);

		int32_t cmdShow = mSpecs.startMaximized ? SW_MAXIMIZE : SW_SHOW;

 		ShowWindow(mHandle, cmdShow);
		SetForegroundWindow(mHandle);
		UpdateWindow(mHandle);

		mGraphicsContext = CreateRef<DX11_Context>(mHandle, mSpecs.width, mSpecs.height);
		mGraphicsContext->SetVSync(mSpecs.vSync);
	}

	Win32_Window::~Win32_Window()
	{
		DestroyWindow(mHandle);
		UnregisterClass(mWindowClass, m_hInstance);
	}

	void Win32_Window::PollEvents()
	{
		MSG msg;
		ZeroMemory(&msg, sizeof(MSG));

		while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		SDL_Event sdl_event;
		while (SDL_PollEvent(&sdl_event))
		{
			Input::ProcessGamePadEvents(sdl_event);
		}
	}

	void Win32_Window::SwapBuffers()
	{
		mGraphicsContext->SwapChain();
	}

	void Win32_Window::SetFullScreen()
	{
		mSpecs.isFullScreen = !mSpecs.isFullScreen;

		static RECT windowRect = {};

		DWORD style = mSpecs.isFullScreen ? WS_POPUP : WS_OVERLAPPEDWINDOW;
		SetWindowLong(mHandle, GWL_STYLE, style);

		if (mSpecs.isFullScreen)
		{
			MONITORINFO mi = { sizeof(mi) };
			if (GetMonitorInfo(MonitorFromWindow(mHandle,
				MONITOR_DEFAULTTOPRIMARY), &mi))
			{
				GetWindowRect(mHandle, &windowRect);
				SetWindowPos(mHandle, HWND_TOP,
					mi.rcMonitor.left, mi.rcMonitor.top,
					mi.rcMonitor.right - mi.rcMonitor.left,
					mi.rcMonitor.bottom - mi.rcMonitor.top,
					SWP_SHOWWINDOW);
			}
		}
		else
		{
			SetWindowPos(mHandle, NULL,
				mWindowRect.left, mWindowRect.top, mWindowRect.right - mWindowRect.left, mWindowRect.bottom - mWindowRect.top,
				SWP_SHOWWINDOW);
		}
	}

	void Win32_Window::PixelResize(uint32_t pixelSize)
	{
		if (!mSpecs.isFullScreen)
		{
			mSpecs.pixelWidth = pixelSize;
			mSpecs.pixelHeight = pixelSize;

			mSpecs.width = mProjectionSize.x * mSpecs.pixelWidth;
			mSpecs.height = mProjectionSize.y * mSpecs.pixelHeight;

			mPosition = { GetSystemMetrics(SM_CXSCREEN) / 2.0f - mSpecs.width / 2.0f, GetSystemMetrics(SM_CYSCREEN) / 2.0f - mSpecs.height / 2.0f };

			mWindowRect.left = mPosition.x;
			mWindowRect.top = mPosition.y;
			mWindowRect.right = mWindowRect.left + mSpecs.width;
			mWindowRect.bottom = mWindowRect.top + mSpecs.height;

			SetWindowPos(mHandle, NULL,
				mWindowRect.left, mWindowRect.top, mWindowRect.right - mWindowRect.left, mWindowRect.bottom - mWindowRect.top,
				SWP_SHOWWINDOW);
		}
	}

	LRESULT WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam))
			return true;

		switch (uMsg)
		{
			// WINDOW EVENTS
			case WM_CREATE:
			{
				CREATESTRUCT* createStruct = reinterpret_cast<CREATESTRUCT*>(lParam);
				WindowSpecifications* data = reinterpret_cast<WindowSpecifications*>(createStruct->lpCreateParams);
				SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(data));
				return 0;
			}
			case WM_SIZE:
			{
				UINT width = LOWORD(lParam);
				UINT height = HIWORD(lParam);

				WindowSpecifications* data = (WindowSpecifications*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
				data->width = width;
				data->height = height;

				WindowResizeEvent event(width, height);
				data->eventCallback(event);
				return 0;
			}
			case WM_CLOSE:
			{
				WindowSpecifications* data = (WindowSpecifications*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

				WindowCloseEvent event;
				data->eventCallback(event);
				return 0;
			}

			//// KEY EVENTS
			case WM_SYSKEYDOWN:
			{
				WindowSpecifications* data = (WindowSpecifications*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

				int32_t keycode;
				int32_t isRight = (lParam >> 24) & 0x01;

				if (wParam == 18)
					keycode = (isRight) ? KeyCode::R_ALT : KeyCode::L_ALT;
				else
					keycode = Keyboard::MapKeys[wParam];

				if ((GetKeyState(VK_MENU) & 0x8000) && wParam == VK_F4)
					break; 

				KeyPressedEvent event(wParam, LOWORD(lParam));
				data->eventCallback(event);
				Input::GetKeyboard()->SetNewState(keycode, true);
				return 0;
			}
			case WM_SYSKEYUP:
			{
				WindowSpecifications* data = (WindowSpecifications*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

				int32_t keycode;
				int32_t isRight = (lParam >> 24) & 0x01;

				if (wParam == 18)
					 keycode = (isRight) ? KeyCode::R_ALT : KeyCode::L_ALT; 
				else
					keycode = Keyboard::MapKeys[wParam];
				KeyPressedEvent event(wParam, LOWORD(lParam));
				data->eventCallback(event);
				Input::GetKeyboard()->SetNewState(keycode, false);
				return 0;
			}
			case WM_KEYDOWN:
			{
				WindowSpecifications* data = (WindowSpecifications*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

				KeyPressedEvent event(wParam, LOWORD(lParam));
				data->eventCallback(event);

				int32_t keycode;
				bool isRight;

				switch (wParam)
				{
				case 16:
				{
					isRight = (lParam >> 20) & 0x01;
					keycode = (isRight) ? KeyCode::R_SHIFT : KeyCode::L_SHIFT; break;
				}
				case 17:
				{
					isRight = (lParam >> 24) & 0x01;
					keycode = (isRight) ? KeyCode::R_CTRL : KeyCode::L_CTRL; break;
				}
				default:
					keycode = Keyboard::MapKeys[wParam];
				}

				Input::GetKeyboard()->SetNewState(keycode, true);
				return 0;
			}
			case WM_KEYUP:
			{
				WindowSpecifications* data = (WindowSpecifications*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

				KeyReleasedEvent event(wParam);
				data->eventCallback(event);


				int32_t keycode;
				int32_t isRight;

				switch (wParam)
				{
				case 16:
				{
					isRight = (lParam >> 20) & 0x01;
					keycode = (isRight) ? KeyCode::R_SHIFT : KeyCode::L_SHIFT; break;
				}
				case 17:
				{
					isRight = (lParam >> 24) & 0x01;
					keycode = (isRight) ? KeyCode::R_CTRL : KeyCode::L_CTRL; break;
				}
				default:
					keycode = Keyboard::MapKeys[wParam];
				}

				Input::GetKeyboard()->SetNewState(keycode, false);
				return 0;
			}

			// MOUSE PRESS EVENTS
			case WM_LBUTTONDOWN:
			{
				WindowSpecifications* data = (WindowSpecifications*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

				MouseButtonPressedEvent event(MK_LBUTTON);
				data->eventCallback(event);

				Input::GetMouse()->SetNewState(L_BUTTON, true);
				return 0;
			}
			case WM_RBUTTONDOWN:
			{
				WindowSpecifications* data = (WindowSpecifications*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

				MouseButtonPressedEvent event(MK_RBUTTON);
				data->eventCallback(event);

				Input::GetMouse()->SetNewState(R_BUTTON, true);
				return 0;
			}
			case WM_MBUTTONDOWN:
			{
				WindowSpecifications* data = (WindowSpecifications*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

				MouseButtonPressedEvent event(MK_MBUTTON);
				data->eventCallback(event);

				Input::GetMouse()->SetNewState(M_BUTTON, true);
				return 0;
			}
			case WM_XBUTTONDOWN:
			{
				WindowSpecifications* data = (WindowSpecifications*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

				int32_t mouseButton = (wParam & MK_XBUTTON1) | (wParam & MK_XBUTTON2);
				MouseButtonPressedEvent event(mouseButton);
				data->eventCallback(event);
				if (mouseButton == 32)
					mouseButton = X_BUTTON1;
				else if (mouseButton == 64)
					mouseButton = X_BUTTON2;

				Input::GetMouse()->SetNewState(mouseButton, true);
				return 0;
			}

			// MOUSE RELEASE EVENTS
			case WM_LBUTTONUP:
			{
				WindowSpecifications* data = (WindowSpecifications*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

				MouseButtonReleasedEvent event(MK_LBUTTON);
				data->eventCallback(event);

				Input::GetMouse()->SetNewState(L_BUTTON, false);
				return 0;
			}
			case WM_RBUTTONUP:
			{
				WindowSpecifications* data = (WindowSpecifications*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

				MouseButtonReleasedEvent event(MK_RBUTTON);
				data->eventCallback(event);

				Input::GetMouse()->SetNewState(R_BUTTON, false);
				return 0;
			}
			case WM_MBUTTONUP:
			{
				WindowSpecifications* data = (WindowSpecifications*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

				MouseButtonReleasedEvent event(MK_MBUTTON);
				data->eventCallback(event);

				Input::GetMouse()->SetNewState(M_BUTTON, false);
				return 0;
			}
			case WM_XBUTTONUP:
			{
				WindowSpecifications* data = (WindowSpecifications*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

				UINT xbuttoncode = wParam;
				if (wParam == 131072)		xbuttoncode = (wParam | 0x0040) & 0x0040;
				else if (wParam == 65536)	xbuttoncode = (wParam | 0x0020) & 0x0020;
				MouseButtonReleasedEvent event(xbuttoncode);
				data->eventCallback(event);
				if (xbuttoncode == 32)
					xbuttoncode = X_BUTTON1;
				else if (xbuttoncode == 64)
					xbuttoncode = X_BUTTON2;

				Input::GetMouse()->SetNewState(xbuttoncode, false);
				return 0;
			}

			// MOUSE SCROLL EVENTS
			case WM_MOUSEWHEEL:
			{
				WindowSpecifications* data = (WindowSpecifications*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

				MouseScrolledEvent event(0.0f, GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA);
				data->eventCallback(event);
				return 0;
			}
			case WM_MOUSEHWHEEL: // I have no mouse to test the x-axis scrolling so i don't know if this works as intended
			{
				WindowSpecifications* data = (WindowSpecifications*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

				// the sign is inverted so it matches expected behavior (scroll right = positive)
				MouseScrolledEvent event(-GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA, 0.0f);
				data->eventCallback(event);
				return 0;
			}

			// MOUSE MOVE EVENTS
			case WM_MOUSEMOVE:
			{
				WindowSpecifications* data = (WindowSpecifications*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

				MouseMovedEvent event((float)LOWORD(lParam), (float)HIWORD(lParam));
				data->eventCallback(event);
				return 0;
			}
		}

		
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}

	std::string ResultInfo(HRESULT hr)
	{
		_com_error error(hr);
		return error.ErrorMessage();
	}
}
#endif