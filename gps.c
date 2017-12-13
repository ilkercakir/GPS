/*
 * gps.c
 * 
 * Copyright 2017 pc <pc@pc-ubuntu>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <termios.h>
#include <fcntl.h>

void set_baud_rate(char* dev, speed_t baud)
{
	int fd;
	struct termios settings;

	fd = open(dev, O_RDWR | O_NONBLOCK);

	tcgetattr(fd, &settings);
	cfsetospeed(&settings, baud); // baud rate
	cfsetispeed(&settings, baud); // baud rate
	tcsetattr(fd, TCSANOW, &settings); // apply the settings
	tcflush(fd, TCOFLUSH);
	tcflush(fd, TCIFLUSH);

	close(fd);
}

int main(int argc, char **argv)
{
	char c;
	int pos, numtokens, row=200;
	char str[512];
	char *tokens[50];
	char *dev;
	FILE *f;

	if (argc > 1)
	{
		dev = argv[1];
	}
	else
	{
		dev = "/dev/ttyACM0";
	}

	set_baud_rate(dev, B4800);
	
	//FILE *f = fopen("/dev/ttyUSB0", "r");
	f = fopen(dev, "r");

	while(row)
	{
		c = '\0';
		numtokens = 0;
		tokens[numtokens++] = str;
		for(pos=0;c!=0x0A;pos++)
		{
			fread(&c, 1, 1, f);
			if (c==',')
			{
				c = '\0';
				tokens[numtokens++] = str + pos + 1;
			}
			str[pos] = c;
		}
		str[--pos]='\0';

		if (!strcmp(tokens[0], "$GPGGA"))
		{
			if (strcmp(tokens[1], "")) // Time token not empty
			{
				if (strcmp(tokens[2], "") && strcmp(tokens[4], "")) // Lattitude & Longitude tokens not empty
				{
					//printf("Time %s Lattitude %s %s Longitude %s %s Height %s\n", tokens[1], tokens[2], tokens[3], tokens[4], tokens[5], tokens[9]);
					int timevalue = atoi(tokens[1]);
					int hh = timevalue/10000;
					timevalue-=hh*10000;
					int mm = timevalue/100;
					timevalue-=mm*100;
					int ss = timevalue;

					float lattitude = atof(tokens[2])/100.0;
					float longitude = atof(tokens[4])/100.0;
					int lat_deg = (long)(lattitude);
					float lat_min = (lattitude-(float)lat_deg)/60.0*100.0;

					int lon_deg = (long)(longitude);
					float lon_min = (longitude-(float)lon_deg)/60.0*100.0;

					float height = atof(tokens[9]);
					printf("%02d:%02d:%02d %2.5f %s %2.5f %s, height %5.2f\n", hh, mm, ss, (float)lat_deg+lat_min, tokens[3], (float)lon_deg+lon_min, tokens[5], height);
					row--;
				}
			}
		}
	}

	fclose(f);
	return 0;
}

