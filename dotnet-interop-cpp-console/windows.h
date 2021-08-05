#pragma once
#include <Windows.h>
#include <dwmapi.h>
#include <tchar.h>
#include <Shlwapi.h>

namespace utils
	{
	namespace windows
		{
		namespace autorun
			{
			inline void add(LPCWSTR name)
				{
				TCHAR szPath[MAX_PATH];
				GetModuleFileName(NULL, szPath, MAX_PATH);
				HKEY key;
				RegOpenKey(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", &key);
				RegSetValueEx(key, name, 0, REG_SZ, (LPBYTE)szPath, sizeof(szPath));
				RegCloseKey(key);
				}
			inline void remove(LPCWSTR name)
				{
				TCHAR szPath[MAX_PATH];
				GetModuleFileName(NULL, szPath, MAX_PATH);
				HKEY key;
				RegOpenKey(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", &key);
				RegDeleteValue(key, name);
				RegCloseKey(key);
				}
			};
		namespace screen
			{
			inline int get_min_x() { return GetSystemMetrics(SM_CXMINTRACK); }
			inline int get_max_x() { return GetSystemMetrics(SM_CXMAXTRACK); }
			inline int get_min_y() { return GetSystemMetrics(SM_CYMINTRACK); }
			inline int get_max_y() { return GetSystemMetrics(SM_CYMAXTRACK); }
			inline int get_width_single_monitor() { return GetSystemMetrics(SM_CXSCREEN); }
			inline int get_height_single_monitor() { return GetSystemMetrics(SM_CYSCREEN); }
			inline int get_width() { return GetSystemMetrics(SM_CXVIRTUALSCREEN); }
			inline int get_height() { return GetSystemMetrics(SM_CYVIRTUALSCREEN); }
			inline int get_taskbar_height()
				{
				RECT rect;
				HWND taskBar = FindWindow(L"Shell_traywnd", NULL);
				if (taskBar && GetWindowRect(taskBar, &rect))
					{
					return rect.bottom - rect.top;
					}
				}
			};

		struct handle_data
			{
			unsigned long process_id;
			HWND best_handle;
			};

		namespace window
			{
			inline void set_transparent(HWND handle)
				{
				MARGINS margins;
				margins.cxLeftWidth = -1;
				SetWindowLong(handle, GWL_STYLE, WS_POPUP | WS_VISIBLE);
				DwmExtendFrameIntoClientArea(handle, &margins);
				}
			inline void set_always_on_top(HWND handle) { SetWindowPos(handle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); }
			inline void set_always_on_bottom(HWND handle) { SetWindowPos(handle, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); }
			inline void set_hidden_from_toolbar(HWND handle) { SetWindowLong(handle, GWL_EXSTYLE, WS_EX_TOOLWINDOW); }
			inline BOOL is_main_window(HWND handle) { return GetWindow(handle, GW_OWNER) == (HWND)0 && IsWindowVisible(handle); }
			inline BOOL CALLBACK enum_windows_callback(HWND handle, LPARAM lParam)
				{
				handle_data& data = *(handle_data*)lParam;
				unsigned long process_id = 0;
				GetWindowThreadProcessId(handle, &process_id);
				if (data.process_id != process_id || !is_main_window(handle))
					{
					return TRUE;
					}
				data.best_handle = handle;
				return FALSE;
				}
			inline HWND find_main_window(unsigned long process_id)
				{
				handle_data data;
				data.process_id = process_id;
				data.best_handle = 0;
				EnumWindows(enum_windows_callback, (LPARAM)&data);
				return data.best_handle;
				}


			inline BOOL CALLBACK _EnumWindowsProc(HWND hwnd, LPARAM lParam)
				{
				HWND p = FindWindowEx(hwnd, NULL, L"SHELLDLL_DefView", NULL);
				HWND* ret = (HWND*)lParam;

				if (p)
					{
					// Gets the WorkerW Window after the current one.
					*ret = FindWindowEx(NULL, hwnd, L"WorkerW", NULL);
					}
				return true;
				}

			inline HWND get_wallpaper_window()
				{
				// Fetch the Progman window
				HWND progman = FindWindow(L"ProgMan", NULL);
				// Send 0x052C to Progman. This message directs Progman to spawn a 
				// WorkerW behind the desktop icons. If it is already there, nothing 
				// happens.
				SendMessageTimeout(progman, 0x052C, 0, 0, SMTO_NORMAL, 1000, nullptr);
				// We enumerate all Windows, until we find one, that has the SHELLDLL_DefView 
				// as a child. 
				// If we found that window, we take its next sibling and assign it to workerw.
				HWND wallpaper_hwnd = nullptr;
				EnumWindows(_EnumWindowsProc, (LPARAM)&wallpaper_hwnd);
				return wallpaper_hwnd;
				}
			}

		//inline bool spawn_prompt(STARTUPINFO* prompt_startupinfo, PROCESS_INFORMATION* prompt_processinfo, HWND* prompt_window)
		//	{
		//	// additional information
		//	STARTUPINFO si;
		//	PROCESS_INFORMATION pi;
		//	pi.hProcess;
		//	// set the size of the structures
		//	ZeroMemory(&si, sizeof(si));
		//	si.cb = sizeof(si);
		//	ZeroMemory(&pi, sizeof(pi));
		//
		//	// start the program up
		//	CreateProcess(L"C:\\Windows\\System32\\cmd.exe",   // the path
		//		L"",            // Command line
		//		NULL,           // Process handle not inheritable
		//		NULL,           // Thread handle not inheritable
		//		FALSE,          // Set handle inheritance to FALSE
		//		0,              // No creation flags
		//		NULL,           // Use parent's environment block
		//		NULL,           // Use parent's starting directory 
		//		&si,            // Pointer to STARTUPINFO structure
		//		&pi             // Pointer to PROCESS_INFORMATION structure (removed extra parentheses)
		//	);
		//	Sleep(1000);
		//	HWND pw = utils::windows::window::find_main_window(pi.dwProcessId);
		//	*prompt_startupinfo = si;
		//	*prompt_processinfo = pi;
		//	*prompt_window = pw;
		//	return false;
		//	}

		inline TCHAR* _GetDirFromPath(TCHAR* dest, size_t destSize)
			{
			if (!dest) return NULL;
			if (MAX_PATH > destSize) return NULL;

			DWORD length = GetModuleFileName(NULL, dest, destSize);
			PathRemoveFileSpec(dest);
			return dest;
			}
		inline void SetExeDirectory()
			{
			TCHAR dir[_MAX_PATH];
			GetModuleFileName(NULL, dir, _MAX_PATH);
			_GetDirFromPath(dir, _MAX_PATH);
			SetCurrentDirectory(dir);
			}

		inline void mouse_set_position(int x, int y)
			{
			long fScreenWidth = GetSystemMetrics(SM_CXSCREEN) - 1;
			long fScreenHeight = GetSystemMetrics(SM_CYSCREEN) - 1;

			// http://msdn.microsoft.com/en-us/library/ms646260(VS.85).aspx
			// If MOUSEEVENTF_ABSOLUTE value is specified, dx and dy contain normalized absolute coordinates between 0 and 65,535.
			// The event procedure maps these coordinates onto the display surface.
			// Coordinate (0,0) maps onto the upper-left corner of the display surface, (65535,65535) maps onto the lower-right corner.
			float fx = x * (65535.0f / fScreenWidth);
			float fy = y * (65535.0f / fScreenHeight);

			INPUT Input = {0};
			Input.type = INPUT_MOUSE;

			Input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;

			Input.mi.dx = (long)fx;
			Input.mi.dy = (long)fy;

			SendInput(1, &Input, sizeof(INPUT));
			}
		inline void mouse_click(int x, int y)
			{
			mouse_set_position(x, y);
			INPUT Input = {0};													// Create our input.

			Input.type = INPUT_MOUSE;									// Let input know we are using the mouse.
			Input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;							// We are setting left mouse button down.
			SendInput(1, &Input, sizeof(INPUT));								// Send the input.

			ZeroMemory(&Input, sizeof(INPUT));									// Fills a block of memory with zeros.
			Input.type = INPUT_MOUSE;									// Let input know we are using the mouse.
			Input.mi.dwFlags = MOUSEEVENTF_LEFTUP;								// We are setting left mouse button up.
			SendInput(1, &Input, sizeof(INPUT));								// Send the input.

			}
		};
	/**/
	}