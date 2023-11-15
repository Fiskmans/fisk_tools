#ifndef TOOLS_EVENT_H
#define TOOLS_EVENT_H

#include <cassert>
#include <functional>
#include <memory>
#include <unordered_map>
#include <optional>

#include "tools/Logger.h"

namespace fisk::tools
{

	class EventRegistration
	{
	public:
		EventRegistration()							= default;
		EventRegistration(const EventRegistration&) = delete;
		void operator=(const EventRegistration&)	= delete;
		~EventRegistration();

	private:
		template <typename...>
		friend class Event;
		template <class T, typename...>
		friend class ShortCircutableEvent;
		std::function<void()> myDeregistraion;
	};

	template <typename... Args>
	class Event
	{
	public:
		Event()						 = default;
		Event(const Event&)			 = delete;
		void operator=(const Event&) = delete;
		~Event();

		[[nodiscard]] std::unique_ptr<EventRegistration> Register(std::function<void(Args...)> aCallback);

		void Fire(Args... aArgs) const;

	private:
		using EventID						 = size_t;
		static constexpr EventID NullEventId = 0;

		inline EventID NextID()
		{
			return ++myCurrentID;
		}

		void UnRegister(EventID aEventId);

		EventID myCurrentID = NullEventId;

		struct Registration
		{
			Registration(EventRegistration* aRegistration, std::function<void(Args...)> aCallback)
				: myRegistration(aRegistration)
				, myCallback(aCallback)
			{
			}

			EventRegistration* myRegistration;
			std::function<void(Args...)> myCallback;
		};

		std::unordered_map<EventID, Registration> myCallbacks;
	};

	template <class ReturnType, typename... Args>
	class ShortCircutableEvent
	{
	public:
		ShortCircutableEvent()							  = default;
		ShortCircutableEvent(const ShortCircutableEvent&) = delete;
		void operator=(const ShortCircutableEvent&)		  = delete;
		~ShortCircutableEvent();

		using Callback = std::function<std::optional<ReturnType>(Args...)>;

		[[nodiscard]] std::unique_ptr<EventRegistration> Register(Callback aCallback);

		std::optional<ReturnType> Fire(Args... aArgs) const;

	private:
		using EventID						 = size_t;
		static constexpr EventID NullEventId = 0;

		inline EventID NextID()
		{
			return ++myCurrentID;
		}

		void UnRegister(EventID aEventId);

		EventID myCurrentID = NullEventId;

		struct Registration
		{
			Registration(EventRegistration* aRegistration, Callback aCallback, EventID aEventID)
				: myRegistration(aRegistration)
				, myCallback(aCallback)
				, myEventID(aEventID)
			{
			}

			EventRegistration* myRegistration;
			Callback myCallback;
			EventID myEventID;
		};

		std::vector<Registration> myCallbacks;
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

	using EventReg = std::unique_ptr<EventRegistration>;

	template <typename... Args>
	inline Event<Args...>::~Event()
	{
		std::vector<EventRegistration*> stackCopy;

		stackCopy.reserve(myCallbacks.size());

		for (const std::pair<EventID, Registration>& it : myCallbacks)
			stackCopy.push_back(it.second.myRegistration);

		for (EventRegistration* registration : stackCopy)
		{
			registration->myDeregistraion();
			registration->myDeregistraion = nullptr;
		}
	}

	template <typename... Args>
	inline std::unique_ptr<EventRegistration> Event<Args...>::Register(std::function<void(Args...)> aCallback)
	{
		EventID id = NextID();

		std::unique_ptr<EventRegistration> out = std::make_unique<EventRegistration>();
		out->myDeregistraion				   = [id, this]() { UnRegister(id); };

		myCallbacks.emplace(id, Registration(out.get(), aCallback));

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

		for (const std::pair<EventID, Registration>& it : myCallbacks)
			stackCopy.push_back(it.second.myCallback);

		for (const std::function<void(Args...)>& callback : stackCopy)
			callback(std::forward<Args>(aArgs)...);
	}

	template <class ReturnType, typename... Args>
	inline ShortCircutableEvent<ReturnType, Args...>::~ShortCircutableEvent()
	{
		std::vector<EventRegistration*> stackCopy;

		stackCopy.reserve(myCallbacks.size());

		for (Registration& it : myCallbacks)
			stackCopy.push_back(it.myRegistration);

		for (EventRegistration* registration : stackCopy)
		{
			registration->myDeregistraion();
			registration->myDeregistraion = nullptr;
		}
	}

	template <class ReturnType, typename... Args>
	inline std::unique_ptr<EventRegistration> ShortCircutableEvent<ReturnType, Args...>::Register(Callback aCallback)
	{
		EventID id = NextID();

		std::unique_ptr<EventRegistration> out = std::make_unique<EventRegistration>();
		out->myDeregistraion				   = [id, this]() { UnRegister(id); };

		myCallbacks.push_back(Registration(out.get(), aCallback, id));

		return std::move(out);
	}

	template <class ReturnType, typename... Args>
	inline std::optional<ReturnType> ShortCircutableEvent<ReturnType, Args...>::Fire(Args... aArgs) const
	{
		for (Registration reg : myCallbacks)
		{
			std::optional<ReturnType> res = reg.myCallback(std::forward<Args>(aArgs)...);

			if (res)
				return res;
		}

		return {};
	}

	template <class ReturnType, typename... Args>
	inline void ShortCircutableEvent<ReturnType, Args...>::UnRegister(EventID aEventId)
	{
		assert(aEventId != NullEventId);

		typename decltype(myCallbacks)::iterator it = 
			std::find_if(myCallbacks.begin(), 
				myCallbacks.end(), 
				[aEventId](const Registration& aReg) {
					return aReg.myEventID == aEventId; });

		assert(it != myCallbacks.end());

		myCallbacks.erase(it);
	}

} // namespace fisk::tools

#endif