#include <cstdint>

class VideoCycleCounter
{
public:
	void increment()
	{
		counter++;
		// LCDStatus->hblankFlag = counter > 960;
		if (counter == 1232)
		{
			counter = 0;
			//  memoryLayout[4][6]++;
			//if (memoryLayout[4][6] == 228)
			// 	memoryLayout[4][6] = 0;
		}
		// LCDStatus->hblankFlag = counter > 960;
	}
private:
	uint16_t counter = 0;
};