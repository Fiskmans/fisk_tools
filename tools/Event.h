#ifndef TOOLS_EVENT_H
#define TOOLS_EVENT_H

#include <functional>
#include <unordered_map>
#include <cassert>

#include "tools/Logger.h"

namespace fisk::tools
{
	class EventRegistration;

	template <typename... Args>
	class Event
	{
	public:
		[[nodiscard]] std::unique_ptr<EventRegistration> Register(std::function<void(Args...)> aCallback);

		void Fire(Args... aArgs) const;

	private:
		using EventID				  = size_t;
		static constexpr EventID NullEventId = 0;

		inline EventID NextID()
		{
			return ++myCurrentID;
		}

		void UnRegister(EventID aEventId);

		EventID myCurrentID = NullEventId;
		std::unordered_map<EventID, std::function<void(Args...)>> myCallbacks;
	};

	
	template <typename... Args>
	class SingleFireEvent
	{
	public:
		void Register(std::function<void(Args...)> aCallback)
		{
			myCallback = aCallback;
		}

		void Fire(Args... aArgs)
		{
			if (myCallback)
				myCallback(std::forward<Args>(aArgs)...);

			myCallback = nullptr;
		}

	private:
		std::function<void(Args...)> myCallback;
	};

	class EventRegistration
	{
	public:
		~EventRegistration();

	private:
		template <typename...>
		friend class Event;
		std::function<void()> myDeregistraion;
	};

	using EventReg = std::unique_ptr<EventRegistration>;

	template <typename... Args>
	inline std::unique_ptr<EventRegistration> Event<Args...>::Register(std::function<void(Args...)> aCallback)
	{
		EventID id = NextID();
		myCallbacks.emplace(id, aCallback);

		std::unique_ptr<EventRegistration> out = std::make_unique<EventRegistration>();
		out->myDeregistraion				   = [id, this]() { UnRegister(id); };

		return std::move(out);
	}

	template <typename... Args>
	inline void Event<Args...>::UnRegister(EventID aEventId)
	{
		assert(aEventId != NullEventId);

		typename decltype(myCallbacks)::iterator it = myCallbacks.find(aEventId);
		assert(it != myCallbacks.end());

		myCallbacks.erase(it);
	}

	template <typename... Args>
	inline void Event<Args...>::Fire(Args... aArgs) const
	{
		std::vector<std::function<void(Args...)>> stackCopy;

		stackCopy.reserve(myCallbacks.size());

		for (const std::pair<EventID, std::function<void(Args...)>>& it : myCallbacks)
			stackCopy.push_back(it.second);

		for (const std::function<void(Args...)>& callback : stackCopy)
			callback(std::forward<Args>(aArgs)...);
	}

} // namespace fisk::tools

#endif