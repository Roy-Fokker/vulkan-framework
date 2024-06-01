module;

#include <Windows.h>
#include <cassert>

export module input;

import std;

export namespace win32
{
	enum class input_button : std::uint8_t
	{
		none           = 0x00, // No key was pressed
		left_button    = 0x01, // Left mouse button
		right_button   = 0x02, // Right mouse button
		cancel         = 0x03, // Cancel key
		middle_button  = 0x04, // Middle mouse button
		extra_button_1 = 0x05, // X1 mouse button
		extra_button_2 = 0x06, // X2 mouse button
		//                  0x07 is undefined
		back = 0x08,
		tab  = 0x09,
		//                  0x0A - 0B are reserved
		clear = 0x0c, // The CLEAR key
		enter = 0x0d, // The Enter key
		//                  0x0E - 0F are undefined
		shift        = 0x10, // The Shift key
		control      = 0x11, // The Ctrl key
		alt          = 0x12, // The Alt key
		pause        = 0x13, // The Pause key
		caps_lock    = 0x14, // The Caps Lock key
		kana_mode    = 0x15, // IMI Kana mode
		hanguel_mode = 0x15, // IMI Hanguel mode (Use HangulMode)
		hangul_mode  = 0x15, // IMI Hangul mode
		//                  0x16 is undefined
		junja_mode = 0x17, // IMI Janja mode
		jinal_mode = 0x18, // IMI Final mode
		hanja_mode = 0x19, // IMI Hanja mode
		kanji_mode = 0x19, // IMI Kanji mode
		//                  0x1A is undefined
		escape          = 0x1b, // The ESC key
		imi_convert     = 0x1c, // IMI convert key
		imi_no_convert  = 0x1d, // IMI noconvert key
		imi_accept      = 0x1e, // IMI accept key
		imi_mode_change = 0x1f, // IMI mode change key
		space           = 0x20, // The Space key
		prior           = 0x21, // The Page Up key
		pageUp          = 0x21, // The Page Up key
		next            = 0x22, // The Page Down key
		pageDown        = 0x22, // The Page Down key
		end             = 0x23, // The End key
		home            = 0x24, // The Home key
		left_arrow      = 0x25, // The Left arrow key
		up_arrow        = 0x26, // The Up arrow key
		right_arrow     = 0x27, // The Right arrow key
		down_arrow      = 0x28, // The Down arrow key
		select          = 0x29, // The Select key
		print           = 0x2a, // The Print key
		execute         = 0x2b, // The Execute key
		print_screen    = 0x2c, // The Print Screen key
		snapshot        = 0x2c, // The Print Screen key
		insert          = 0x2d, // The Insert key
		del             = 0x2e, // The Delete key
		help            = 0x2f, // The Help key
		D0              = 0x30, // 0
		D1              = 0x31, // 1
		D2              = 0x32, // 2
		D3              = 0x33, // 3
		D4              = 0x34, // 4
		D5              = 0x35, // 5
		D6              = 0x36, // 6
		D7              = 0x37, // 7
		D8              = 0x38, // 8
		D9              = 0x39, // 9
		//                  0x3A - 40 are undefined
		A         = 0x41, // A
		B         = 0x42, // B
		C         = 0x43, // C
		D         = 0x44, // D
		E         = 0x45, // E
		F         = 0x46, // F
		G         = 0x47, // G
		H         = 0x48, // H
		I         = 0x49, // I
		J         = 0x4a, // J
		K         = 0x4b, // K
		L         = 0x4c, // L
		M         = 0x4d, // M
		N         = 0x4e, // N
		O         = 0x4f, // O
		P         = 0x50, // P
		Q         = 0x51, // Q
		R         = 0x52, // R
		S         = 0x53, // S
		T         = 0x54, // T
		U         = 0x55, // U
		V         = 0x56, // V
		W         = 0x57, // W
		X         = 0x58, // X
		Y         = 0x59, // Y
		Z         = 0x5a, // Z
		left_win  = 0x5b, // Left Windows key
		right_win = 0x5c, // Right Windows key
		apps      = 0x5d, // Apps key
		//                  0x5E is reserved
		sleep     = 0x5f, // The Sleep key
		num_pad_0 = 0x60, // The Numeric keypad 0 key
		num_pad_1 = 0x61, // The Numeric keypad 1 key
		num_pad_2 = 0x62, // The Numeric keypad 2 key
		num_pad_3 = 0x63, // The Numeric keypad 3 key
		num_pad_4 = 0x64, // The Numeric keypad 4 key
		num_pad_5 = 0x65, // The Numeric keypad 5 key
		num_pad_6 = 0x66, // The Numeric keypad 6 key
		num_pad_7 = 0x67, // The Numeric keypad 7 key
		num_pad_8 = 0x68, // The Numeric keypad 8 key
		num_pad_9 = 0x69, // The Numeric keypad 9 key
		multiply  = 0x6a, // The Multiply key
		add       = 0x6b, // The Add key
		separator = 0x6c, // The Separator key
		subtract  = 0x6d, // The Subtract key
		decimal   = 0x6e, // The Decimal key
		divide    = 0x6f, // The Divide key
		F1        = 0x70, // The F1 key
		F2        = 0x71, // The F2 key
		F3        = 0x72, // The F3 key
		F4        = 0x73, // The F4 key
		F5        = 0x74, // The F5 key
		F6        = 0x75, // The F6 key
		F7        = 0x76, // The F7 key
		F8        = 0x77, // The F8 key
		F9        = 0x78, // The F9 key
		F10       = 0x79, // The F10 key
		F11       = 0x7a, // The F11 key
		F12       = 0x7b, // The F12 key
		F13       = 0x7c, // The F13 key
		F14       = 0x7d, // The F14 key
		F15       = 0x7e, // The F15 key
		F16       = 0x7f, // The F16 key
		F17       = 0x80, // The F17 key
		F18       = 0x81, // The F18 key
		F19       = 0x82, // The F19 key
		F20       = 0x83, // The F20 key
		F21       = 0x84, // The F21 key
		F22       = 0x85, // The F22 key
		F23       = 0x86, // The F23 key
		F24       = 0x87, // The F24 key
		//                  0x88 - 8f are unassigned
		num_lock    = 0x90, // The Num Lock key
		scroll_lock = 0x91, // The Scroll Lock key
		//                  0x92 - 96 are OEM specific
		//                  0x97 - 9f are unassigned
		left_shift           = 0xa0, // The Left Shift key
		right_shift          = 0xa1, // The Right Shift key
		left_control         = 0xa2, // The Left Control key
		right_control        = 0xa3, // The Right Control key
		left_alt             = 0xa4, // The Left Alt key
		right_alt            = 0xa5, // The Right Alt key
		browser_back         = 0xa6, // The Browser Back key
		browser_forward      = 0xa7, // The Browser Forward key
		browser_refresh      = 0xa8, // The Browser Refresh key
		browser_stop         = 0xa9, // The Browser Stop key
		browser_search       = 0xaa, // The Browser Search key
		browser_favorites    = 0xab, // The Browser Favorites key
		browser_home         = 0xac, // The Browser Home key
		volume_mute          = 0xad, // The Volume Mute key
		volume_down          = 0xae, // The Volume Down key
		volume_up            = 0xaf, // The Volume Up key
		media_next_track     = 0xb0, // The Next Track key
		media_previous_track = 0xb1, // The Previous Track key
		media_stop           = 0xb2, // The Stop Media key
		media_play_pause     = 0xb3, // The Play/Pause Media key
		launch_mail          = 0xb4, // The Start Mail key
		select_media         = 0xb5, // The Select Media key
		launch_application_1 = 0xb6, // The Launch Application 1 key.
		launch_application_2 = 0xb7, // The Launch Application 2 key.
		//                  0xB8 - B9 are reserved
		oem_semicolon = 0xba,
		oem_1         = 0xba, // Used for miscellaneous characters; it can vary by keyboard.  For the US standard keyboard, the ';:' key
		oem_plus      = 0xbb, // For any country/region, the '+' key
		oem_comma     = 0xbc, // For any country/region, the ',' key
		oem_minus     = 0xbd, // For any country/region, the '-' key
		oem_period    = 0xbe, // For any country/region, the '.' key
		oem_question  = 0xbf,
		oem_2         = 0xbf, // Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the '/?' key
		oem_tilde     = 0xc0,
		oem_3         = 0xc0, // Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the '`~' key
		//                  0xC1 - D7 are reserved
		//                  0xD8 - DA are unassigned
		oem_openBrackets  = 0xdb,
		oem_4             = 0xdb, // Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the '[{' key
		oem_pipe          = 0xdc,
		oem_5             = 0xdc, // Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the '\|' key
		oem_closeBrackets = 0xdd,
		oem_6             = 0xdd, // Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the ']}' key
		oem_quotes        = 0xde,
		oem_7             = 0xde, // Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the 'single-quote/double-quote' key
		oem_8             = 0xdf, // Used for miscellaneous characters; it can vary by keyboard.
		//                  0xE0 is reserved
		//                  0xE1 is OEM specific
		oem_backslash = 0xe2,
		oem_102       = 0xe2, // Either the angle bracket key or the backslash key on the RT 102-key keyboard
		//                  0xE3 - E4 OEM specific
		process_key = 0xe5, // IME Process key
		//                  0xE6 is OEM specific
		packet = 0xe7, // Used to pass Unicode characters as if they were keystrokes. The Packet key value is the low word of a 32-bit virtual-key value used for non-keyboard input methods.
		//                  0xE8 is unassigned
		//                  0xE9 - F5 OEM specific
		attn      = 0xf6, // The Attn key
		cr_sel    = 0xf7, // The CrSel key
		ex_sel    = 0xf8, // The ExSel key
		erase_eof = 0xf9, // The Erase EOF key
		play      = 0xfa, // The Play key
		zoom      = 0xfb, // The Zoom key
		no_name   = 0xfc, // Reserved
		pa_1      = 0xfd, // The PA1 key
		oem_clear = 0xfe, // The Clear key
	};

	[[nodiscard]] auto to_string(input_button button_) -> std::string_view
	{
		using enum input_button;
		const static auto names = std::unordered_map<input_button, std::string_view>{
			{ none, "none" },                                 // No key was pressed
			{ left_button, "left_button" },                   // Left mouse button
			{ right_button, "right_button" },                 // Right mouse button
			{ cancel, "cancel" },                             // Cancel key
			{ middle_button, "middle_button" },               // Middle mouse button
			{ extra_button_1, "extra_button_1" },             // X1 mouse button
			{ extra_button_2, "extra_button_2" },             // X2 mouse button
			{ back, "back" },                                 // Backspace key
			{ tab, "tab" },                                   // Tab key
			{ clear, "clear" },                               // The CLEAR key
			{ enter, "enter" },                               // The Enter key
			{ shift, "shift" },                               // The Shift key
			{ control, "control" },                           // The Ctrl key
			{ alt, "alt" },                                   // The Alt key
			{ pause, "pause" },                               // The Pause key
			{ caps_lock, "caps_lock" },                       // The Caps Lock key
			{ kana_mode, "kana_mode" },                       // IMI Kana mode
			{ hanguel_mode, "hanguel_mode" },                 // IMI Hanguel mode (Use HangulMode)
			{ hangul_mode, "hangul_mode" },                   // IMI Hangul mode
			{ junja_mode, "junja_mode" },                     // IMI Janja mode
			{ jinal_mode, "jinal_mode" },                     // IMI Final mode
			{ hanja_mode, "hanja_mode" },                     // IMI Hanja mode
			{ kanji_mode, "kanji_mode" },                     // IMI Kanji mode
			{ escape, "escape" },                             // The ESC key
			{ imi_convert, "imi_convert" },                   // IMI convert key
			{ imi_no_convert, "imi_no_convert" },             // IMI noconvert key
			{ imi_accept, "imi_accept" },                     // IMI accept key
			{ imi_mode_change, "imi_mode_change" },           // IMI mode change key
			{ space, "space" },                               // The Space key
			{ prior, "prior" },                               // The Page Up key
			{ pageUp, "pageUp" },                             // The Page Up key
			{ next, "next" },                                 // The Page Down key
			{ pageDown, "pageDown" },                         // The Page Down key
			{ end, "end" },                                   // The End key
			{ home, "home" },                                 // The Home key
			{ left_arrow, "left_arrow" },                     // The Left arrow key
			{ up_arrow, "up_arrow" },                         // The Up arrow key
			{ right_arrow, "right_arrow" },                   // The Right arrow key
			{ down_arrow, "down_arrow" },                     // The Down arrow key
			{ select, "select" },                             // The Select key
			{ print, "print" },                               // The Print key
			{ execute, "execute" },                           // The Execute key
			{ print_screen, "print_screen" },                 // The Print Screen key
			{ snapshot, "snapshot" },                         // The Print Screen key
			{ insert, "insert" },                             // The Insert key
			{ del, "del" },                                   // The Delete key
			{ help, "help" },                                 // The Help key
			{ D0, "D0" },                                     // 0
			{ D1, "D1" },                                     // 1
			{ D2, "D2" },                                     // 2
			{ D3, "D3" },                                     // 3
			{ D4, "D4" },                                     // 4
			{ D5, "D5" },                                     // 5
			{ D6, "D6" },                                     // 6
			{ D7, "D7" },                                     // 7
			{ D8, "D8" },                                     // 8
			{ D9, "D9" },                                     // 9
			{ A, "A" },                                       // A
			{ B, "B" },                                       // B
			{ C, "C" },                                       // C
			{ D, "D" },                                       // D
			{ E, "E" },                                       // E
			{ F, "F" },                                       // F
			{ G, "G" },                                       // G
			{ H, "H" },                                       // H
			{ I, "I" },                                       // I
			{ J, "J" },                                       // J
			{ K, "K" },                                       // K
			{ L, "L" },                                       // L
			{ M, "M" },                                       // M
			{ N, "N" },                                       // N
			{ O, "O" },                                       // O
			{ P, "P" },                                       // P
			{ Q, "Q" },                                       // Q
			{ R, "R" },                                       // R
			{ S, "S" },                                       // S
			{ T, "T" },                                       // T
			{ U, "U" },                                       // U
			{ V, "V" },                                       // V
			{ W, "W" },                                       // W
			{ X, "X" },                                       // X
			{ Y, "Y" },                                       // Y
			{ Z, "Z" },                                       // Z
			{ left_win, "left_win" },                         // Left Windows key
			{ right_win, "right_win" },                       // Right Windows key
			{ apps, "apps" },                                 // Apps key
			{ sleep, "sleep" },                               // The Sleep key
			{ num_pad_0, "num_pad_0" },                       // The Numeric keypad 0 key
			{ num_pad_1, "num_pad_1" },                       // The Numeric keypad 1 key
			{ num_pad_2, "num_pad_2" },                       // The Numeric keypad 2 key
			{ num_pad_3, "num_pad_3" },                       // The Numeric keypad 3 key
			{ num_pad_4, "num_pad_4" },                       // The Numeric keypad 4 key
			{ num_pad_5, "num_pad_5" },                       // The Numeric keypad 5 key
			{ num_pad_6, "num_pad_6" },                       // The Numeric keypad 6 key
			{ num_pad_7, "num_pad_7" },                       // The Numeric keypad 7 key
			{ num_pad_8, "num_pad_8" },                       // The Numeric keypad 8 key
			{ num_pad_9, "num_pad_9" },                       // The Numeric keypad 9 key
			{ multiply, "multiply" },                         // The Multiply key
			{ add, "add" },                                   // The Add key
			{ separator, "separator" },                       // The Separator key
			{ subtract, "subtract" },                         // The Subtract key
			{ decimal, "decimal" },                           // The Decimal key
			{ divide, "divide" },                             // The Divide key
			{ F1, "F1" },                                     // The F1 key
			{ F2, "F2" },                                     // The F2 key
			{ F3, "F3" },                                     // The F3 key
			{ F4, "F4" },                                     // The F4 key
			{ F5, "F5" },                                     // The F5 key
			{ F6, "F6" },                                     // The F6 key
			{ F7, "F7" },                                     // The F7 key
			{ F8, "F8" },                                     // The F8 key
			{ F9, "F9" },                                     // The F9 key
			{ F10, "F10" },                                   // The F10 key
			{ F11, "F11" },                                   // The F11 key
			{ F12, "F12" },                                   // The F12 key
			{ F13, "F13" },                                   // The F13 key
			{ F14, "F14" },                                   // The F14 key
			{ F15, "F15" },                                   // The F15 key
			{ F16, "F16" },                                   // The F16 key
			{ F17, "F17" },                                   // The F17 key
			{ F18, "F18" },                                   // The F18 key
			{ F19, "F19" },                                   // The F19 key
			{ F20, "F20" },                                   // The F20 key
			{ F21, "F21" },                                   // The F21 key
			{ F22, "F22" },                                   // The F22 key
			{ F23, "F23" },                                   // The F23 key
			{ F24, "F24" },                                   // The F24 key
			{ num_lock, "num_lock" },                         // The Num Lock key
			{ scroll_lock, "scroll_lock" },                   // The Scroll Lock key
			{ left_shift, "left_shift" },                     // The Left Shift key
			{ right_shift, "right_shift" },                   // The Right Shift key
			{ left_control, "left_control" },                 // The Left Control key
			{ right_control, "right_control" },               // The Right Control key
			{ left_alt, "left_alt" },                         // The Left Alt key
			{ right_alt, "right_alt" },                       // The Right Alt key
			{ browser_back, "browser_back" },                 // The Browser Back key
			{ browser_forward, "browser_forward" },           // The Browser Forward key
			{ browser_refresh, "browser_refresh" },           // The Browser Refresh key
			{ browser_stop, "browser_stop" },                 // The Browser Stop key
			{ browser_search, "browser_search" },             // The Browser Search key
			{ browser_favorites, "browser_favorites" },       // The Browser Favorites key
			{ browser_home, "browser_home" },                 // The Browser Home key
			{ volume_mute, "volume_mute" },                   // The Volume Mute key
			{ volume_down, "volume_down" },                   // The Volume Down key
			{ volume_up, "volume_up" },                       // The Volume Up key
			{ media_next_track, "media_next_track" },         // The Next Track key
			{ media_previous_track, "media_previous_track" }, // The Previous Track key
			{ media_stop, "media_stop" },                     // The Stop Media key
			{ media_play_pause, "media_play_pause" },         // The Play/Pause Media key
			{ launch_mail, "launch_mail" },                   // The Start Mail key
			{ select_media, "select_media" },                 // The Select Media key
			{ launch_application_1, "launch_application_1" }, // The Launch Application 1 key.
			{ launch_application_2, "launch_application_2" }, // The Launch Application 2 key.
			{ oem_semicolon, "semicolon/colon" },             // OEM Semicolon
			{ oem_1, "oem_1" },                               // Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the ';:' key
			{ oem_plus, "plus/equal" },                       // For any country/region, the '+=' key
			{ oem_comma, "comma" },                           // For any country/region, the ',<' key
			{ oem_minus, "minus" },                           // For any country/region, the '-_' key
			{ oem_period, "period" },                         // For any country/region, the '.>' key
			{ oem_question, "question/foreslash" },           // OEM Question
			{ oem_2, "oem_2" },                               // Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the '/?' key
			{ oem_tilde, "tilde" },                           // OEM Tilda
			{ oem_3, "oem_3" },                               // Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the '`~' key
			{ oem_openBrackets, "openBrackets" },             // OEM Open Brackets
			{ oem_4, "oem_4" },                               // Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the '[{' key
			{ oem_pipe, "pipe/backslash" },                   // OEM Pipe
			{ oem_5, "oem_5" },                               // Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the '\|' key
			{ oem_closeBrackets, "closeBrackets" },           // OEM Close Brackets
			{ oem_6, "oem_6" },                               // Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the ']}' key
			{ oem_quotes, "quotes" },                         // OEM Quotes
			{ oem_7, "oem_7" },                               // Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the 'single-quote/double-quote' key
			{ oem_8, "oem_8" },                               // Used for miscellaneous characters; it can vary by keyboard.
			{ oem_backslash, "backslash" },                   // OEM Backslash
			{ oem_102, "oem_102" },                           // Either the angle bracket key or the backslash key on the RT 102-key keyboard
			{ process_key, "process_key" },                   // IME Process key
			{ packet, "packet" },                             // Used to pass Unicode characters as if they were keystrokes. The Packet key value is the low word of a 32-bit virtual-key value used for non-keyboard input methods.
			{ attn, "attn" },                                 // The Attn key
			{ cr_sel, "cr_sel" },                             // The CrSel key
			{ ex_sel, "ex_sel" },                             // The ExSel key
			{ erase_eof, "erase_eof" },                       // The Erase EOF key
			{ play, "play" },                                 // The Play key
			{ zoom, "zoom" },                                 // The Zoom key
			{ no_name, "no_name" },                           // Reserved
			{ pa_1, "pa_1" },                                 // The PA1 key
			{ oem_clear, "oem_clear" },                       // The Clear key
		};

		return names.at(button_);
	}

	enum class input_axis : std::uint8_t
	{
		none,
		x,
		y,
		rx,
		ry,
	};

	[[nodiscard]] auto to_string(input_axis axis_) -> std::string_view
	{
		using enum input_axis;
		const static auto names = std::unordered_map<input_axis, std::string_view>{
			{ none, "none" },
			{ x, "x" },
			{ y, "y" },
			{ rx, "rx" },
			{ ry, "ry" },
		};

		return names.at(axis_);
	}

	enum class input_device
	{
		keyboard,
		mouse,
	};

		class input final
	{
	public:
		input() = delete;
		input(const input &src)             = delete;
		input &operator=(const input &src)  = delete;
		input(input &&src)                  = delete;
		input &operator=(input &&src)       = delete;

		input(HWND hWnd, const std::vector<input_device> &devices);
		~input() = default;

		void process_messages();

		// Which Direction to go in, returns 1, -1, or 0
		auto which_button_is_down(input_button positive_button, input_button negative_button) const -> std::int8_t
		{
			return (is_button_down(positive_button) ? 1 : 0) | 
			       (is_button_down(negative_button) ? -1 : 0);
		}

		auto is_button_down(input_button button) const -> bool
		{
			return buttons_down.at(static_cast<uint8_t>(button));
		}

		auto get_axis_value(input_axis axis, bool absolute = false) const -> std::int32_t
		{
			if (absolute)
			{
				return axis_values_absolute.at(static_cast<std::size_t>(axis));
			}

			return axis_values_relative.at(static_cast<std::size_t>(axis));
		}


	private:
		void process_input(std::uint32_t count)
		{
			// Reset relative positions for all axis
			axis_values_relative.fill(0);

			for (auto &&[i, raw_data] : input_buffer | std::views::enumerate)
			{
				if (i >= count)
					break;

				switch (raw_data.header.dwType)
				{
				case RIM_TYPEKEYBOARD:
					process_keyboard_input(raw_data.data.keyboard);
					break;
				case RIM_TYPEMOUSE:
					process_mouse_input(raw_data.data.mouse);
					break;
				default:
					// Not sure what causes this to happen.
					// assert(false);
					break;
				}
			}
		}

		void process_keyboard_input(const RAWKEYBOARD &data);
		void process_mouse_input(const RAWMOUSE &data);

	private:
		HWND hWnd;

		alignas(8) std::array<RAWINPUT, 128> input_buffer{};

		std::array<bool, 256> buttons_down{};
		std::array<std::int32_t, 5> axis_values_relative{};
		std::array<std::int32_t, 5> axis_values_absolute{};
	};
}


namespace
{
	using namespace win32;

	constexpr auto use_buffered_raw_input = false;
	constexpr auto page_id                = 0x01;
	constexpr auto usage_id               = std::array<std::uint8_t, 2>{
        0x06, // keyboard
        0x02, // mouse
	};
	constexpr std::array raw_device_desc{
		RAWINPUTDEVICE{ page_id, usage_id.at(static_cast<std::uint8_t>(input_device::keyboard)) },
		RAWINPUTDEVICE{ page_id, usage_id.at(static_cast<std::uint8_t>(input_device::mouse)) },
	};

	auto translate_to_button(std::uint16_t vKey, std::uint16_t sCode, std::uint16_t flags) -> input_button
	{
		// return if the Key is out side of our enumeration range
		if (vKey > static_cast<std::uint16_t>(input_button::oem_clear) ||
		    vKey == static_cast<std::uint16_t>(input_button::none))
		{
			return input_button::none;
		}

		// figure out which key was press, in cases where there are duplicates (e.g numpad)
		const bool isE0 = ((flags & RI_KEY_E0) != 0);
		const bool isE1 = ((flags & RI_KEY_E1) != 0);

		switch (static_cast<input_button>(vKey))
		{
		case input_button::pause:
			sCode = (isE1) ? 0x45 : static_cast<std::uint16_t>( MapVirtualKey(vKey, MAPVK_VK_TO_VSC));
			// TODO: What happens here????
			break;
		case input_button::shift:
			vKey = static_cast<std::uint16_t>(MapVirtualKey(sCode, MAPVK_VSC_TO_VK_EX));
			break;
		case input_button::control:
			return ((isE0) ? input_button::right_control : input_button::left_control);

		case input_button::alt:
			return ((isE0) ? input_button::right_alt : input_button::left_alt);

		case input_button::enter:
			return ((isE0) ? input_button::separator : input_button::enter);

		case input_button::insert:
			return ((!isE0) ? input_button::num_pad_0 : input_button::insert);

		case input_button::del:
			return ((!isE0) ? input_button::decimal : input_button::del);

		case input_button::home:
			return ((!isE0) ? input_button::num_pad_7 : input_button::home);

		case input_button::end:
			return ((!isE0) ? input_button::num_pad_1 : input_button::end);

		case input_button::prior:
			return ((!isE0) ? input_button::num_pad_9 : input_button::prior);

		case input_button::next:
			return ((!isE0) ? input_button::num_pad_3 : input_button::next);

		case input_button::left_arrow:
			return ((!isE0) ? input_button::num_pad_4 : input_button::left_arrow);

		case input_button::right_arrow:
			return ((!isE0) ? input_button::num_pad_6 : input_button::right_arrow);

		case input_button::up_arrow:
			return ((!isE0) ? input_button::num_pad_8 : input_button::up_arrow);

		case input_button::down_arrow:
			return ((!isE0) ? input_button::num_pad_2 : input_button::down_arrow);

		case input_button::clear:
			return ((!isE0) ? input_button::num_pad_5 : input_button::clear);
		}

		return static_cast<input_button>(vKey);
	}

	auto translate_to_button(std::uint16_t btnFlags) -> input_button
	{
		auto btn = input_button::none;

		// Which button was pressed?
		if ((btnFlags & RI_MOUSE_BUTTON_1_DOWN) or (btnFlags & RI_MOUSE_BUTTON_1_UP))
		{
			btn = input_button::left_button; // MK_LBUTTON;
		}
		else if ((btnFlags & RI_MOUSE_BUTTON_2_DOWN) or (btnFlags & RI_MOUSE_BUTTON_2_UP))
		{
			btn = input_button::right_button; // MK_RBUTTON;
		}
		else if ((btnFlags & RI_MOUSE_BUTTON_3_DOWN) or (btnFlags & RI_MOUSE_BUTTON_3_UP))
		{
			btn = input_button::middle_button; // MK_MBUTTON;
		}
		else if ((btnFlags & RI_MOUSE_BUTTON_4_DOWN) or (btnFlags & RI_MOUSE_BUTTON_4_UP))
		{
			btn = input_button::extra_button_1; // MK_XBUTTON1;
		}
		else if ((btnFlags & RI_MOUSE_BUTTON_5_DOWN) or (btnFlags & RI_MOUSE_BUTTON_5_UP))
		{
			btn = input_button::extra_button_2; // MK_XBUTTON2;
		}

		return btn;
	}
}

using namespace win32;

input::input(HWND hWnd, const std::vector<input_device> &devices)
	: hWnd(hWnd)
{
	auto rid = std::vector<RAWINPUTDEVICE>{};

	for (auto device : devices)
	{
		rid.push_back(raw_device_desc.at(static_cast<int>(device)));
		rid.back().hwndTarget = hWnd;
	}

	auto result = ::RegisterRawInputDevices(rid.data(),
	                                        static_cast<uint32_t>(rid.size()),
	                                        sizeof(RAWINPUTDEVICE));
	assert(result == TRUE);
}

void input::process_messages()
{
	auto input_msg_count = 0u;

	if constexpr (use_buffered_raw_input)
	{
		// Use Buffered Raw Input
		// This means no using WM_INPUT in windows message proc.

		auto raw_input_buffer_size = static_cast<std::uint32_t>(input_buffer.size() * sizeof(RAWINPUT));
		input_msg_count            = GetRawInputBuffer(&input_buffer.at(0),
		                                               &raw_input_buffer_size,
		                                               sizeof(RAWINPUTHEADER));

		assert(input_msg_count >= 0); // if asserted issues with getting data
	}
	else
	{
		// Use Unbuffered Raw Input
		// use PeekMessage restricted to WM_INPUT to get all the RAWINPUT data

		auto msg               = MSG{};
		auto has_more_messages = BOOL{ TRUE };

		while (has_more_messages == TRUE && input_msg_count < input_buffer.size())
		{
			has_more_messages = PeekMessage(&msg, hWnd, WM_INPUT, WM_INPUT, PM_NOYIELD | PM_REMOVE);

			if (has_more_messages == FALSE)
			{
				continue;
			}

			auto raw_input_size = static_cast<std::uint32_t>(sizeof(RAWINPUT));
			auto bytes_copied   = GetRawInputData(reinterpret_cast<HRAWINPUT>(msg.lParam),
			                                      RID_INPUT,
			                                      &input_buffer[input_msg_count],
			                                      &raw_input_size,
			                                      sizeof(RAWINPUTHEADER));
			assert(bytes_copied >= 0); // if asserted issues with copying data

			input_msg_count++;
		}
	}

	process_input(input_msg_count);
}

void input::process_keyboard_input(const RAWKEYBOARD &data)
{
	auto vKey   = data.VKey;
	auto sCode  = data.MakeCode;
	auto flags  = data.Flags;
	auto kState = false;

	if (!(flags & RI_KEY_BREAK))
	{
		kState = true;
	}

	auto button = translate_to_button(vKey, sCode, flags);
	vKey        = static_cast<std::uint16_t>(button);

	// TODO: Figure out what new key states [up, down, pressed]

	// Is this key a toggle key? if so change toggle state
	if (button == input_button::caps_lock or button == input_button::num_lock or button == input_button::scroll_lock)
	{
		kState = !buttons_down[vKey];
	}

	// Update the Keyboard state array
	buttons_down[vKey] = kState;

	// Update the Keyboard state where there are duplicate
	// i.e Shift, Ctrl, and Alt
	switch (button)
	{
	case input_button::left_shift:
	case input_button::right_shift:
		buttons_down[static_cast<std::uint16_t>(input_button::shift)] = kState;
		break;
	case input_button::left_control:
	case input_button::right_control:
		buttons_down[static_cast<std::uint16_t>(input_button::control)] = kState;
		break;
	case input_button::left_alt:
	case input_button::right_alt:
		buttons_down[static_cast<std::uint16_t>(input_button::alt)] = kState;
		break;
	}
}

void input::process_mouse_input(const RAWMOUSE &data)
{
	auto btnFlags = data.usButtonFlags;

	auto button = translate_to_button(btnFlags);
	auto vBtn   = static_cast<std::uint16_t>(button);

	// What is the button state?
	bool btnState{ false };
	if ((btnFlags & RI_MOUSE_BUTTON_1_DOWN) || (btnFlags & RI_MOUSE_BUTTON_2_DOWN) || (btnFlags & RI_MOUSE_BUTTON_3_DOWN) || (btnFlags & RI_MOUSE_BUTTON_4_DOWN) || (btnFlags & RI_MOUSE_BUTTON_5_DOWN))
	{
		btnState = true;
	}
	// TODO: Figure out what new key states [up, down, pressed]
	buttons_down[vBtn] = btnState;

	std::int32_t xPos = data.lLastX;
	std::int32_t yPos = data.lLastY;
	std::int16_t rWheel = data.usButtonData;

	if (data.usFlags & MOUSE_MOVE_ABSOLUTE)
	{
		// Update Axis values
		axis_values_relative[static_cast<std::int8_t>(input_axis::x)] = xPos;
		axis_values_relative[static_cast<std::int8_t>(input_axis::y)] = yPos;
		// Update Absolute values for axis
		axis_values_absolute[static_cast<std::int8_t>(input_axis::x)] = xPos;
		axis_values_absolute[static_cast<std::int8_t>(input_axis::y)] = yPos;
	}
	else
	{
		// Update Axis values
		axis_values_relative[static_cast<std::int8_t>(input_axis::x)] += xPos;
		axis_values_relative[static_cast<std::int8_t>(input_axis::y)] += yPos;
		// Update Absolute values for axis
		axis_values_absolute[static_cast<std::int8_t>(input_axis::x)] += xPos;
		axis_values_absolute[static_cast<std::int8_t>(input_axis::y)] += yPos;
	}

	// Vertical wheel data
	if (btnFlags & RI_MOUSE_WHEEL)
	{
		axis_values_relative[static_cast<std::int8_t>(input_axis::rx)] += rWheel;
		axis_values_absolute[static_cast<std::int8_t>(input_axis::rx)] += rWheel;
	}

	// Horizontal wheel data
	if (btnFlags & RI_MOUSE_HWHEEL)
	{
		axis_values_relative[static_cast<std::int8_t>(input_axis::ry)] += rWheel;
		axis_values_absolute[static_cast<std::int8_t>(input_axis::ry)] += rWheel;
	}
}