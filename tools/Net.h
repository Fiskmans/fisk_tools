#ifndef FISK_TOOLS_NET_H
#define FISK_TOOLS_NET_H

#include "tools/Singleton.h"


namespace fisk::tools
{
	class NetHandle;
	class Net : public Singleton<Net>
	{
	public:

		NetHandle Use();

	private:

		friend NetHandle;
		void DecUser()
		{
			if (myUseCount-- == 0)
				Shutdown();
		}

		void StartUp();
		void Shutdown();

		size_t myUseCount = 0;
	};

	class NetHandle
	{
	public:
		NetHandle(Net* aOwner) 
			: myOwner(aOwner)
		{
			if (myOwner)
				myOwner->myUseCount++;
		}

		~NetHandle()
		{
			if (myOwner)
				myOwner->DecUser();
		}

		NetHandle(const NetHandle& aOther)
			: myOwner(aOther.myOwner)
		{
			if (myOwner)
				myOwner->myUseCount++;
		}

		void operator=(const NetHandle& aOther)
		{
			if (myOwner)
				myOwner->DecUser();

			myOwner = aOther.myOwner;
			myOwner->myUseCount++;
		}

	private:
		Net* myOwner = nullptr;
	};

	inline NetHandle Net::Use()
	{
		if (myUseCount == 0)
			StartUp();

		return NetHandle(this);
	}

}

#endif