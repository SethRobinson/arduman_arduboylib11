#ifndef ConvertToArduboy11_h__
#define ConvertToArduboy11_h__
#include <Arduboy.h>

extern Arduboy arduboy;

#define text arduboy

class ArduboyPlaytune
{
public:
	ArduboyPlaytune()
	{

	}
	
	void playScore(const byte*score)
	{
		arduboy.tunes.playScore(score);
	}

	void tone(unsigned int frequency, unsigned long duration)
	{
		arduboy.tunes.tone(frequency, duration);
	}

	  void initChannel(byte pin)
	  {
		  arduboy.tunes.initChannel(pin);
	  }
};
#endif // ConvertToArduboy11_h__
