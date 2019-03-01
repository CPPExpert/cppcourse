#pragma once

#include <memory>
#include <map>

#include "message_throttler_interface.hpp"

template<
	typename _ClientId,
	typename _Message,
	typename _MessageConsumer,
	typename _MessageDisposer,
	typename _Timestamp,
	typename _Timestamper,
	typename _TimestampThreshold
>
class message_throttler
{
public:
	using _MessageThrottlerInterface = message_throttler_interface<_Message, _MessageConsumer, _MessageDisposer, _Timestamp, _Timestamper, _TimestampThreshold>;

	message_throttler(
		std::size_t bufferSize,
		const _MessageConsumer& messageConsumer,
		const _MessageDisposer& messageDisposer,
		const _Timestamper& timestamper,
		const _TimestampThreshold& timestampThreshold) :
		mBufferSize(bufferSize),
		mMessageConsumer(messageConsumer),
		mMessageDisposer(messageDisposer),
		mTimestamper(timestamper),
		mTimestampThreshold(timestampThreshold)
	{ }

	_MessageThrottlerInterface& from(const _ClientId& clientId)
	{
		auto& client = mClients[clientId];

		if (client == nullptr)
			client = std::make_unique<_MessageThrottlerInterface>(mBufferSize, mMessageConsumer, mMessageDisposer, mTimestamper, mTimestampThreshold);

		return *client;
	}

private:
	std::size_t mBufferSize;

	_MessageConsumer mMessageConsumer;
	_MessageDisposer mMessageDisposer;

	_Timestamper mTimestamper;
	_TimestampThreshold mTimestampThreshold;

	std::map<_ClientId, std::unique_ptr<_MessageThrottlerInterface>>  mClients;
};