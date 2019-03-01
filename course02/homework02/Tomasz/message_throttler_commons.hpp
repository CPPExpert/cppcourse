#pragma once

#include <map>
#include <chrono>

#include "message_throttler.hpp"
#include "message_throttler_interface.hpp"

template<typename _Message>
struct message_swallower
{
	void operator()(const _Message& message)
	{ }
};

struct chrono_timestamp_threshold
{
	chrono_timestamp_threshold(std::chrono::milliseconds milliseconds) :
		milliseconds(milliseconds)
	{ }

	bool operator()(std::chrono::milliseconds now, std::chrono::milliseconds timestamp)
	{
		return (now - timestamp) < milliseconds;
	}

	std::chrono::milliseconds milliseconds;
};

struct chrono_timestamper
{
	std::chrono::milliseconds operator()()
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
	}
};

template<
	typename _ClientId,
	typename _Message,
	typename _Timestamp = std::chrono::milliseconds,
	typename _MessageConsumer = message_swallower<_Message>,
	typename _MessageDisposer = message_swallower<_Message>,
	typename _Timestamper = chrono_timestamper,
	typename _TimestampThreshold = chrono_timestamp_threshold
>
message_throttler<_ClientId, _Message, _MessageConsumer, _MessageDisposer, _Timestamp, _Timestamper, _TimestampThreshold> make_message_throttler(
	std::size_t bufferSize,
	_MessageConsumer messageConsumer = {},
	_MessageDisposer messageDisposer = {},
	_TimestampThreshold timestampThreshold = { std::chrono::milliseconds {1000} },
	_Timestamper timestamper = {}
)
{
	return message_throttler<_ClientId, _Message, _MessageConsumer, _MessageDisposer, _Timestamp, _Timestamper, _TimestampThreshold>(
		bufferSize,
		messageConsumer,
		messageDisposer,
		timestamper,
		timestampThreshold);
}