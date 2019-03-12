#pragma once

namespace xtest {
namespace application {

	struct DirectxSettings
	{
		enum class Buffering
		{
			single_buffer = 1,
			double_buffer,
			triple_buffer
		};

		Buffering buffering = Buffering::double_buffer;
		bool showFrameStats = false;
	};

} // application
} // xtest

