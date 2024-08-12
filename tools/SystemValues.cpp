#include "SystemValues.h"

#include <sstream>

namespace fisk::tools
{
	bool SystemValues::Process(DataProcessor& aProcessor)
	{
		return aProcessor.Process(myFloatValues)
			&& aProcessor.Process(myDoubleValues)
			&& aProcessor.Process(myLongDoubleValues)
			&& aProcessor.Process(myCharValues)
			&& aProcessor.Process(myShortValues)
			&& aProcessor.Process(myIntValues)
			&& aProcessor.Process(myLongValues)
			&& aProcessor.Process(myLongLongValues)
			&& aProcessor.Process(mySizeTValues);
	}

	SystemValues::Difference SystemValues::Differences(SystemValues& aOther)
	{
		Difference diff;

		CheckForDifferences(diff, "float", myFloatValues, aOther.myFloatValues);
		CheckForDifferences(diff, "double", myDoubleValues, aOther.myDoubleValues);
		CheckForDifferences(diff, "long_double", myLongDoubleValues, aOther.myLongDoubleValues);
		CheckForDifferences(diff, "char", myCharValues, aOther.myCharValues);
		CheckForDifferences(diff, "short", myShortValues, aOther.myShortValues);
		CheckForDifferences(diff, "int", myIntValues, aOther.myIntValues);
		CheckForDifferences(diff, "long", myLongValues, aOther.myLongValues);
		CheckForDifferences(diff, "long_long", myLongLongValues, aOther.myLongLongValues);
		CheckForDifferences(diff, "size_t", mySizeTValues, aOther.mySizeTValues);

		return diff;
	}

	SystemValues::Difference::operator bool()
	{
		return !myMessage.empty() || !mySubDifferences.empty();
	}

	std::string SystemValues::Difference::ToString()
	{
		std::stringstream ss;
		if (myTag.empty())
		{
			bool first = true;
			for (std::unique_ptr<Difference>& sub : mySubDifferences)
			{
				if (!first)
				{
					ss << "\n";
				}

				first = false;

				ss << sub->ToString();
			}
		}
		else
		{
			ss << "[" << myTag << "]" << myMessage;

			for (std::unique_ptr<Difference>& sub : mySubDifferences)
			{
				ss << "\n  " << sub->ToString();
			}
		}

		return ss.str();
	}

	bool SystemValues::Difference::Contains(std::string aTag)
	{
		std::string::size_type dotPos = aTag.find('.');

		std::string tag;
		std::string subTags;

		if (dotPos != std::string::npos)
		{
			tag = aTag.substr(0, dotPos);
			subTags = aTag.substr(dotPos + 1);
		}
		else
		{
			tag = aTag;
		}

		Difference* diff = nullptr;

		for (std::unique_ptr<Difference>& sub : mySubDifferences)
		{
			if (sub->myTag == tag)
			{
				diff = sub.get();
				break;
			}
		}

		if (diff)
		{
			if (subTags.empty())
				return true;

			return diff->Contains(subTags);
		}

		return false;
	}

}
