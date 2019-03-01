#pragma once

#include <boost/circular_buffer.hpp>

template<
	typename _Message,
	typename _MessageConsumer,
	typename _MessageDisposer,
	typename _Timestamp,
	typename _Timestamper,
	typename _TimestampThreshold
>
class message_throttler_interface
{
public:
	message_throttler_interface(
		std::size_t bufferSize,
		_MessageConsumer& messageConsumer, 
		_MessageDisposer& messageDisposer, 
		_Timestamper& timestamper,
		_TimestampThreshold & timestampThreshold) :
		mBuffer(bufferSize),
		mMessageConsumer(messageConsumer),
		mMessageDisposer(messageDisposer),
		mTimestamper(timestamper),
		mTimestampThreshold(timestampThreshold)
	{ }

	message_throttler_interface(const message_throttler_interface&) = delete;
	message_throttler_interface& operator=(const message_throttler_interface&) = delete;

	message_throttler_interface& send(const _Message& message)
	{
		auto now = mTimestamper();

		try_make_space_in_buffer(now);
		try_send(message, now);

		return *this;
	}

private:
	void try_make_space_in_buffer(const _Timestamp& now)
	{
		if (mBuffer.full() && !mTimestampThreshold(now, mBuffer.front()))
			mBuffer.pop_front();
	}

	void try_send(const _Message& message, const _Timestamp& now)
	{
		if (mBuffer.full())
			dispose(message);
		else
			consume(message, now);
	}

	void consume(const _Message& message, const _Timestamp& now)
	{
		mMessageConsumer(message);
		mBuffer.push_back(now);
	}

	void dispose(const _Message& message)
	{
		mMessageDisposer(message);
	}

	_MessageConsumer& mMessageConsumer;
	_MessageDisposer& mMessageDisposer;

	_Timestamper& mTimestamper;
	_TimestampThreshold& mTimestampThreshold;

	boost::circular_buffer<_Timestamp> mBuffer;
};