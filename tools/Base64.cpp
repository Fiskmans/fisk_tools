
#include "tools/Base64.h"

#include <cassert>

namespace fisk::tools
{
    namespace base_64
    {
        const char globEncodeLookup[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

        uint8_t globDecodeLookup[256];

        struct ReverseTableBuilder
        {
            ReverseTableBuilder()
            {
                memset(globDecodeLookup, 0xff, sizeof(globDecodeLookup));

                for (uint8_t i = 0; i < 64; i++)
                {
                    globDecodeLookup[globEncodeLookup[i]] = i;
                }
            }
        } globReverseTableBuilderInstance;


        std::string EncodeBlock(const uint8_t ablock[3])
        {
            uint8_t a = (ablock[0] & 0b11111100) >> 2;

            uint8_t b = ((ablock[0] & 0b00000011) << 4)
                      | ((ablock[1] & 0b11110000) >> 4);

            uint8_t c = ((ablock[1] & 0b00001111) << 2)
                      | ((ablock[2] & 0b11000000) >> 6);

            uint8_t d = (ablock[2] & 0b00111111);

            return {
                globEncodeLookup[a],
                globEncodeLookup[b],
                globEncodeLookup[c],
                globEncodeLookup[d]
            };
        }

        std::string EncodePartialBlock(const uint8_t* aBlock, uint32_t aAmount)
        {
            assert(aAmount > 0);
            assert(aAmount < 3);

            uint8_t block[3]
            {
                aBlock[0],
                aAmount > 1 ? aBlock[1] : 0,
                aAmount > 2 ? aBlock[2] : 0
            };

            std::string out = EncodeBlock(block);

            switch (aAmount)
            {
            case 1:
                out[2] = '=';
            case 2:
                out[3] = '=';
                break;
            }

            return out;
        }

        uint32_t DecodeBlock(std::string_view aData, uint8_t aOutBlock[3])
        {
            assert(aData.length() == 4);

            uint8_t a = globDecodeLookup[aData[0]];
            uint8_t b = globDecodeLookup[aData[1]];

            assert(a != 0xff); // TODO: report error;
            assert(b != 0xff); // TODO: report error;

            aOutBlock[0] = (a << 2)
                         | ((b & 0b110000) >> 4);

            if (aData[2] == '=')
                return 1;

			uint8_t c = globDecodeLookup[aData[2]];
            assert(c != 0xff); // TODO: report error;

			aOutBlock[1] = ((b & 0b001111) << 4)
				         | ((c & 0b111100) >> 2);

			if (aData[3] == '=')
				return 2;

			uint8_t d = globDecodeLookup[aData[3]];
			assert(d != 0xff); // TODO: report error;

			aOutBlock[2] = ((c & 0b000011) << 6)
				         | d;

            return 3;
        }
    }

    std::string Base64::Encode(const std::vector<uint8_t>& aData)
    {
        std::string out;
        out.reserve(aData.size() * 4 / 3 + 3);
        
        size_t at;
        for (at = 0; at + 2 < aData.size(); at += 3)
            out.append(base_64::EncodeBlock(aData.data() + at));

        if (at != aData.size())
            out.append(base_64::EncodePartialBlock(aData.data() + at, aData.size() - at));

        return out;
    }

    std::vector<uint8_t> Base64::Decode(const std::string_view aData)
    {
        if (aData.size() % 4 != 0)
        {
            // TODO: report error;
            assert(false);
        }

        size_t blocks = aData.length() / 4;

        std::vector<uint8_t> out;
        out.reserve(blocks * 3);

        for (size_t i = 0; i < blocks; i++)
        {
            uint8_t block[3]{ 0, 0, 0 };

            uint32_t length = 3;

            length = base_64::DecodeBlock(aData.substr(i * 4, 4), block);

            if (length != 3)
			{
				// TODO: report error;
                assert(i + 1 == blocks);
            }

            out.resize(out.size() + length);

            memcpy(out.data() + (i * 3), block, length);
        }

        return out;
    }
}
