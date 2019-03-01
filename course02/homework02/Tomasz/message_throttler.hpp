#pragma once

#include<memory>

template<
	typename _ClientId,
	typename _Message,
	typename _MessageConsumer,
	typename _MessageDisposer,
	typename _Timestamp,
	typename _Timestamper,
	typename _TimestampThreshold,
	typename _MessageThrottlerInterface,
	typename _Map
>
class message_throttler
{
public:
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

	_Map mClients;
};