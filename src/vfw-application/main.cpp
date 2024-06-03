import std;

import clock;
import input;
import window;
import application;

auto main() -> int
{
	using namespace std::string_view_literals;
	using namespace win32;

	std::println("Running in: {}", std::filesystem::current_path().generic_string());

	auto app = app_base::application();

	return app.run();
}