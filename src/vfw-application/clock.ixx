module;

export module clock;

export namespace app_clock
{
	using ns = std::chrono::duration<double, std::nano>;
	using us = std::chrono::duration<double, std::micro>;
	using ms = std::chrono::duration<double, std::milli>;
	using s  = std::chrono::duration<double, std::ratio<1>>;

	class timer
	{
		using hrc = std::chrono::high_resolution_clock;

	public:
		void tick()
		{
			auto timepoint_now = hrc::now();

			delta_time = timepoint_now - timepoint_prev;
			total_time += delta_time;
			timepoint_prev = timepoint_now;
		}

		void reset()
		{
			timepoint_prev = hrc::now();
			delta_time     = hrc::duration{};
			total_time     = hrc::duration{};
		}

		template <typename T>
		auto get_delta() const -> double
		{
			return std::chrono::duration_cast<T>(delta_time).count();
		}

		template <typename T>
		auto get_total() const -> double
		{
			return std::chrono::duration_cast<T>(total_time).count();
		}

	private:
		hrc::time_point timepoint_prev = hrc::now();

		hrc::duration delta_time{};
		hrc::duration total_time{};
	};
}