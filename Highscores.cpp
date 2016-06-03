#include "Highscores.h"
#include "Utils.h"
#include "Player.h"

//Highscore stuff by nootropic design to add high scores
//Some changes from seth so it uses less memory

extern Player player;

//for highscore data:
char initials[3];     //Initials used in high score

//reworked it to not need this
//char text_buffer[16];      //General string buffer

void enterInitials()
{
	char index = 0;

	arduboy.clear();

	initials[0] = ' ';
	initials[1] = ' ';
	initials[2] = ' ';

	while (true)
	{
		arduboy.display();
		arduboy.clear();

		text.setCursor(16,0);
		text.print("HIGH SCORE");
		//sprintf(text_buffer, "%u", player.m_score);
		//don't wanna use sprintf!
		text.setCursor(88, 0);
		//text.print(text_buffer);
		text.print(player.m_score);
		text.setCursor(56, 20);
		text.print(initials[0]);
		text.setCursor(64, 20);
		text.print(initials[1]);
		text.setCursor(72, 20);
		text.print(initials[2]);
		for(byte i = 0; i < 3; i++)
		{
			arduboy.drawLine(56 + (i*8), 27, 56 + (i*8) + 6, 27, 1);
		}
		arduboy.drawLine(56, 28, 88, 28, 0);
		arduboy.drawLine(56 + (index*8), 28, 56 + (index*8) + 6, 28, 1);
		delay(150);

		if (arduboy.pressed(LEFT_BUTTON) || arduboy.pressed(B_BUTTON))
		{
			index--;
			if (index < 0)
			{
				index = 0;
			} else
			{
				tunes.tone(1046, 250);
			}
		}

		if (arduboy.pressed(RIGHT_BUTTON))
		{
			index++;
			if (index > 2)
			{
				index = 2;
			}  else {
				tunes.tone(1046, 250);
			}
		}

		if (arduboy.pressed(DOWN_BUTTON))
		{
			initials[index]++;
			tunes.tone(523, 250);
			// A-Z 0-9 :-? !-/ ' '
			if (initials[index] == '0')
			{
				initials[index] = ' ';
			}
			if (initials[index] == '!')
			{
				initials[index] = 'A';
			}
			if (initials[index] == '[')
			{
				initials[index] = '0';
			}
			if (initials[index] == '@')
			{
				initials[index] = '!';
			}
		}

		if (arduboy.pressed(UP_BUTTON))
		{
			initials[index]--;
			tunes.tone(523, 250);
			if (initials[index] == ' ') {
				initials[index] = '?';
			}
			if (initials[index] == '/') {
				initials[index] = 'Z';
			}
			if (initials[index] == 31) {
				initials[index] = '/';
			}
			if (initials[index] == '@') {
				initials[index] = ' ';
			}
		}

		if (arduboy.pressed(A_BUTTON))
		{
			if (index < 2)
			{
				index++;
				tunes.tone(1046, 250);
			} else {
				tunes.tone(1046, 250);
				return;
			}
		}
	}

}


bool enterHighScore(byte file)
{
	// Each block of EEPROM has 10 high scores, and each high score entry
	// is 5 bytes long:  3 bytes for initials and two bytes for score.
	int address = file * 10 * 5;
	byte hi, lo;
	char tmpInitials[3];
	unsigned int tmpScore = 0;
	
	// High score processing
	for(byte i = 0; i < 10; i++)
	{
		hi = EEPROM.read(address + (5*i));
		lo = EEPROM.read(address + (5*i) + 1);
		if ((hi == 0xFF) && (lo == 0xFF))
		{
			// The values are uninitialized, so treat this entry
			// as a score of 0.
			tmpScore = 0;
		} else
		{
			tmpScore = (hi << 8) | lo;
		}
		if (player.m_score > tmpScore)
		{
			enterInitials();
			for(byte j=i;j<10;j++)
			{
				hi = EEPROM.read(address + (5*j));
				lo = EEPROM.read(address + (5*j) + 1);

				if ((hi == 0xFF) && (lo == 0xFF))
				{
					tmpScore = 0;
				}
				else
				{
					tmpScore = (hi << 8) | lo;
				}

				tmpInitials[0] = (char)EEPROM.read(address + (5*j) + 2);
				tmpInitials[1] = (char)EEPROM.read(address + (5*j) + 3);
				tmpInitials[2] = (char)EEPROM.read(address + (5*j) + 4);

				// write score and initials to current slot
				EEPROM.write(address + (5*j), ((player.m_score >> 8) & 0xFF));
				EEPROM.write(address + (5*j) + 1, (player.m_score & 0xFF));
				EEPROM.write(address + (5*j) + 2, initials[0]);
				EEPROM.write(address + (5*j) + 3, initials[1]);
				EEPROM.write(address + (5*j) + 4, initials[2]);

				// tmpScore and tmpInitials now hold what we want to
				//write in the next slot.
				player.m_score = tmpScore;
				initials[0] = tmpInitials[0];
				initials[1] = tmpInitials[1];
				initials[2] = tmpInitials[2];
			}

			player.m_score = 0;
			initials[0] = ' ';
			initials[1] = ' ';
			initials[2] = ' ';

			return true;
		}
	}

	return false;
}

//Function by nootropic design to display highscores
void displayHighScores(byte file)
{
	byte y = 10;
	byte x = 24;
	// Each block of EEPROM has 10 high scores, and each high score entry
	// is 5 bytes long:  3 bytes for initials and two bytes for score.
	int address = file*10*5;
	byte hi, lo;
	arduboy.clear();
	text.setCursor(32, 0);
	text.print("HIGH SCORES");

	uint32_t score;

	for(int i = 0; i < 10; i++)
	{
		//sprintf(text_buffer, "%2d", i+1);
		text.setCursor(x,y+(i*8));
		//text.print(text_buffer);
		
		text.print(i+1);
		
		hi = EEPROM.read(address + (5*i));
		lo = EEPROM.read(address + (5*i) + 1);

		if ((hi == 0xFF) && (lo == 0xFF))
		{
			score = 0;
		}
		else
		{
			score = (hi << 8) | lo;
		}

		initials[0] = (char)EEPROM.read(address + (5*i) + 2);
		initials[1] = (char)EEPROM.read(address + (5*i) + 3);
		initials[2] = (char)EEPROM.read(address + (5*i) + 4);

		if (score > 0)
		{
			//sprintf(text_buffer, "%c%c%c %u", initials[0], initials[1], initials[2], score);
			text.setCursor(x + 24, y + (i*8));
			text.print(initials[0]);
			text.print(initials[1]);
			text.print(initials[2]);
			text.print(' ');
			text.print(score);

			//text.print(text_buffer);
		}
	}
	if (arduboy.pressed(A_BUTTON))
	{
		g_mode = MODE_MENU;
		tunes.tone(523, 20);
		delay(200);

		return;
	}

	arduboy.display();
}

