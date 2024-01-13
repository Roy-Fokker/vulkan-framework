module;

export module window;

import input;

using namespace std::string_view_literals;

export namespace win32
{
	class window
	{
	public:
		struct description
		{
			std::uint16_t width;
			std::uint16_t height;
			std::wstring_view title;
		};

		enum class active_state
		{
			active,
			inactive,
		};

		using keypress_callback = std::function<bool(input::button button, std::uint16_t scan_code, bool isKeyDown, std::uint16_t repeat_count)>;
		using resize_callback   = std::function<bool(std::uint32_t width, std::uint32_t height)>;
		using activate_callback = std::function<bool(active_state is_active, bool minimized)>;

	public:
		window(const description &desc)
		{
			window_impl = std::make_unique<window_implementation>();

			DWORD default_window_style    = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
				  default_window_style_ex = WS_EX_OVERLAPPEDWINDOW;

			auto window_rectangle = RECT{ 0, 0, desc.width, desc.height };

			AdjustWindowRectEx(&window_rectangle, default_window_style, NULL,
			                   default_window_style_ex);

			window_impl->Create(nullptr, window_rectangle, desc.title,
			                    default_window_style, default_window_style_ex);

			/*
			if (window_icon)
			{
			    auto icon = LoadIcon(GetModuleHandle(nullptr), MAKEINTRESOURCE(window_icon));
			    window_impl->SetIcon(icon);
			}
			*/

			window_impl->CenterWindow();
		}

		~window() = default;

		void show()
		{
			window_impl->ShowWindow(SW_SHOWNORMAL);
			window_impl->SetFocus();
		}

		void process_messages()
		{
			auto has_more_messages = BOOL{ TRUE };
			while (has_more_messages)
			{
				MSG msg{};

				// Parameter two here has to be nullptr, putting hWnd here will
				// not retrive WM_QUIT messages, as those are posted to the thread
				// and not the window
				has_more_messages = PeekMessage(&msg, nullptr, NULL, NULL, PM_REMOVE);
				if (msg.message == WM_QUIT)
				{
					return;
				}
				else
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}
		}

		[[nodiscard]] auto handle() const -> HWND
		{
			return window_impl->m_hWnd;
		}

		void set_callback(const keypress_callback &on_keypress)
		{
			window_impl->on_keypress = on_keypress;
		}

		void set_callback(const resize_callback &on_resize)
		{
			window_impl->on_resize = on_resize;
		}

		void set_callback(const activate_callback &on_activate)
		{
			window_impl->on_activate = on_activate;
		}

	private:
		struct window_implementation;
		std::unique_ptr<window_implementation> window_impl;

		struct window_implementation
		{
			static constexpr std::wstring_view CLASSNAME = L"PureWin32Window"sv;

			window_implementation()
			{
				Register();
			}

			~window_implementation()
			{
				Destroy();
				UnRegister();
			}

			void Register()
			{
				auto wc = WNDCLASSEXW{
					.cbSize        = sizeof(WNDCLASSEX),
					.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
					.lpfnWndProc   = window_implementation::window_procedure,
					.hInstance     = GetModuleHandle(nullptr),
					.hbrBackground = static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH)),
					.lpszClassName = CLASSNAME.data()
				};
				RegisterClassEx(&wc);
			}

			void UnRegister()
			{
				HINSTANCE hInstance = GetModuleHandle(nullptr);
				UnregisterClass(CLASSNAME.data(), hInstance);
			}

			void Create(HWND parent_hwnd,
			            RECT window_rectangle,
			            const std::wstring_view &title,
			            DWORD window_style,
			            DWORD window_style_ex)
			{
				auto [x, y, w, h] = window_rectangle;
				w                 = w - x;
				h                 = h - y;
				x                 = (GetSystemMetrics(SM_CXSCREEN) - w) / 2;
				y                 = (GetSystemMetrics(SM_CYSCREEN) - h) / 2;

				m_hWnd = CreateWindowEx(window_style_ex,
				                        CLASSNAME.data(),
				                        title.data(),
				                        window_style,
				                        x, y, w, h,
				                        parent_hwnd,
				                        nullptr,
				                        GetModuleHandle(nullptr),
				                        static_cast<LPVOID>(this));
			}

			void Destroy()
			{
				if (m_hWnd)
				{
					DestroyWindow(m_hWnd);
					m_hWnd = nullptr;
				}
			}

			// void SetIcon(HICON icon) const
			// {
			// 	SendMessageW(m_hWnd, WM_SETICON, ICON_BIG, (LPARAM)icon);
			// 	SendMessageW(m_hWnd, WM_SETICON, ICON_SMALL, (LPARAM)icon);
			// }

			void GetClientRect(RECT *window_rectangle) const
			{
				::GetClientRect(m_hWnd, window_rectangle);
			}

			void CenterWindow() const
			{
				RECT window_rectangle{};

				GetWindowRect(m_hWnd, &window_rectangle);

				auto [x, y, w, h] = window_rectangle;
				w                 = w - x;
				h                 = h - y;
				x                 = (GetSystemMetrics(SM_CXSCREEN) - w) / 2;
				y                 = (GetSystemMetrics(SM_CYSCREEN) - h) / 2;

				MoveWindow(m_hWnd, x, y, w, h, FALSE);
			}

			void ResizeClient(std::uint32_t width, std::uint32_t height) const
			{
				RECT window_rectangle{};

				GetWindowRect(m_hWnd, &window_rectangle);

				auto [x, y, w, h] = window_rectangle;
				w                 = width;
				h                 = height;
				x                 = (GetSystemMetrics(SM_CXSCREEN) - w) / 2;
				y                 = (GetSystemMetrics(SM_CYSCREEN) - h) / 2;

				MoveWindow(m_hWnd, x, y, w, h, FALSE);
			}

			void ShowWindow(int cmdShow) const
			{
				::ShowWindow(m_hWnd, cmdShow);
			}

			void SetFocus() const
			{
				::SetFocus(m_hWnd);
			}

			void SetWindowPos(HWND hWndInsertAfter, RECT *rect, UINT uFlags) const
			{
				auto [x, y, w, h] = *rect;

				::SetWindowPos(m_hWnd, hWndInsertAfter, x, y, w, h, uFlags);
			}

			void ModifyStyle(DWORD removeStyle, DWORD addStyle, UINT uFlags) const
			{
				auto style = static_cast<DWORD>(::GetWindowLongPtrW(m_hWnd, GWL_STYLE));
				style &= ~(removeStyle);
				style |= addStyle;
				::SetWindowLongPtr(m_hWnd, GWL_STYLE, style);

				::SetWindowPos(m_hWnd, NULL, 0, 0, 0, 0, uFlags);
			}

			void ModifyStyleEx(DWORD removeStyleEx, DWORD addStyleEx, UINT uFlags) const
			{
				auto styleEx = static_cast<DWORD>(::GetWindowLongPtr(m_hWnd, GWL_EXSTYLE));
				styleEx &= ~(removeStyleEx);
				styleEx |= addStyleEx;
				::SetWindowLongPtr(m_hWnd, GWL_EXSTYLE, styleEx);

				::SetWindowPos(m_hWnd, NULL, 0, 0, 0, 0, uFlags);
			}

			static LRESULT CALLBACK window_procedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
			{
				if (msg == WM_NCCREATE)
				{
					auto windowPtr = reinterpret_cast<std::uint64_t>(reinterpret_cast<LPCREATESTRUCTW>(lParam)->lpCreateParams);
					SetWindowLongPtr(hWnd,
					                 GWLP_USERDATA,
					                 windowPtr);
				}

				auto wnd = reinterpret_cast<window_implementation *>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
				if (wnd)
				{
					return wnd->handle_messages(hWnd, msg, wParam, lParam);
				}

				return DefWindowProc(hWnd, msg, wParam, lParam);
			}

			LRESULT CALLBACK handle_messages(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
			{
				switch (msg)
				{
				case WM_DESTROY:
				{
					Destroy();
					PostQuitMessage(NULL);
					return 0;
				}
				case WM_SIZE:
				{
					if (not on_resize)
					{
						break;
					}

					auto width  = LOWORD(lParam);
					auto height = HIWORD(lParam);

					return on_resize(width, height);

					break;
				}
				case WM_ACTIVATE:
				{
					if (not on_activate)
					{
						break;
					}

					auto isActive    = (LOWORD(wParam) | WA_INACTIVE)
					                     ? active_state::active
					                     : active_state::inactive;
					auto isMinimized = (HIWORD(wParam));

					return on_activate(isActive, isMinimized);

					break;
				}
				case WM_KEYDOWN:
				case WM_KEYUP:
				case WM_SYSKEYDOWN:
				case WM_SYSKEYUP:
				{
					if (not on_keypress)
					{
						break;
					}

					auto vkCode = LOWORD(wParam); // virtual-key code

					auto keyFlags = HIWORD(lParam);

					auto scanCode      = (std::uint16_t)LOBYTE(keyFlags);         // scan code
					auto isExtendedKey = (keyFlags & KF_EXTENDED) == KF_EXTENDED; // extended-key flag, 1 if scancode has 0xE0 prefix

					if (isExtendedKey)
						scanCode = MAKEWORD(scanCode, 0xE0);

					[[maybe_unused]] auto wasKeyDown = (keyFlags & KF_REPEAT) == KF_REPEAT; // previous key-state flag, 1 on autorepeat
					auto repeatCount                 = LOWORD(lParam);                      // repeat count, > 0 if several keydown messages was combined into one message
					auto isKeyReleased               = (keyFlags & KF_UP) == KF_UP;         // transition-state flag, 1 on keyup

					// if we want to distinguish these keys:
					switch (vkCode)
					{
					case VK_SHIFT:   // converts to VK_LSHIFT or VK_RSHIFT
					case VK_CONTROL: // converts to VK_LCONTROL or VK_RCONTROL
					case VK_MENU:    // converts to VK_LMENU or VK_RMENU
						vkCode = LOWORD(MapVirtualKey(scanCode, MAPVK_VSC_TO_VK_EX));
						break;
					case VK_CAPITAL:
					case VK_SCROLL:
					case VK_NUMLOCK:
						isKeyReleased = not(GetKeyState(vkCode) & 0x0001);
					}

					auto button = static_cast<input::button>(vkCode);

					return on_keypress(button, scanCode, !isKeyReleased, repeatCount);

					break;
				}
				}

				return DefWindowProc(hWnd, msg, wParam, lParam);
			}

			HWND m_hWnd = nullptr;

			window::keypress_callback on_keypress = nullptr;
			window::activate_callback on_activate = nullptr;
			window::resize_callback on_resize     = nullptr;
		};
	};
}
