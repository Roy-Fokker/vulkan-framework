module;

export module clock;

export namespace timer
{
	using hrc = std::chrono::high_resolution_clock;
	using ns  = std::chrono::duration<double, std::nano>;
	using us  = std::chrono::duration<double, std::micro>;
	using ms  = std::chrono::duration<double, std::milli>;
	using s   = std::chrono::duration<double, std::ratio<1>>;

	class clock
	{
	public:
		void tick()
		{
			auto timepoint_now = hrc::now();

			delta_time         = timepoint_now - timepoint_previous;
			timepoint_previous = timepoint_now;

			total_time += delta_time;
		}

		void reset()
		{
			timepoint_previous = hrc::now();
			delta_time         = {};
			total_time         = {};
		}

		template <typename duration_type>
		[[nodiscard]] auto get_delta() const -> double
		{
			return std::chrono::duration_cast<duration_type>(delta_time).count();
		}

		template <typename duration_type>
		[[nodiscard]] auto get_total() const -> double
		{
			return std::chrono::duration_cast<duration_type>(total_time).count();
		}

	private:
		hrc::time_point timepoint_previous = hrc::now();

		hrc::duration delta_time{};
		hrc::duration total_time{};
	};
}