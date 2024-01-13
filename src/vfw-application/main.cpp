#include <fmt/core.h>

auto main() -> int
{
	std::println("Hello World! -std");

	fmt::println("Hello World! -fmt");

	::MessageBox(nullptr, L"Hello World", L"Test Windows.h", MB_OK);
}