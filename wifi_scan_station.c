/*
 * wifi-scan-station example for wifi-scan library
 *
 * Copyright (C) 2016 Bartosz Meglicki <meglickib@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 * This program is distributed "as is" WITHOUT ANY WARRANTY of any
 * kind, whether express or implied; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/*
 *  This example retrieves information only from associated AP (Access Point).
 *  wifi_scan_station function may be called at high frequency without affectining the link
 *  
 *  The signal strength retrieved comes from last received PPDU (physical layer protocol data unit).
 *  You may want to average the value over some reads.
 * 
 *  Program expects wireless interface as argument, e.g:
 *  wifi-scan-station wlan0
 * 
 */

#include "../wifi_scan.h"
#include <stdio.h>  //printf
#include <unistd.h> //sleep
#include <math.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

//convert bssid to printable hardware mac address
const char *bssid_to_string(const uint8_t bssid[BSSID_LENGTH], char bssid_string[BSSID_STRING_LENGTH])
{
	snprintf(bssid_string, BSSID_STRING_LENGTH, "%02x:%02x:%02x:%02x:%02x:%02x",bssid[0],bssid[1],bssid[2],bssid[3],bssid[4],bssid[5]);
	return bssid_string;
}

void Usage(char **argv);

int main(int argc, char **argv)
{
	struct wifi_scan *wifi=NULL;    //this stores all the library information
	struct station_info station;    //this is where we are going to keep information about AP (Access Point) we are connected to
	char mac[BSSID_STRING_LENGTH];  //a placeholder where we convert BSSID to printable hardware mac address
	int status;

	const int tlumienie_1m=20;
	const double indeks_gamma= -5.5;
	const double zanik_mocy= indeks_gamma*10;
	const double tlumienie_swobod_przestrz = (-27.55+20*log10(2400)+20*log10(9));

	if(argc != 2)
	{
		Usage(argv);
		return 0;
	}

	printf("This is just example, this is library - not utility!\n");
	printf("### Close the program with ctrl+c when you're done ###\n\n");
	
	// initialize the library with network interface argv[1] (e.g. wlan0)
	wifi=wifi_scan_init(argv[1]);

	for(int i=0; i<1000; ++i)
	{
		//get information from just the station we are associated with
		//this is quick, you can call it at much faster frequency (e.g. 50 ms)
		status=wifi_scan_station(wifi, &station);
		
		if(status==0)
			printf("No associated station\n");
		else if(status==-1)
			perror("Unable to get station information\n");
		else
		{
			printf("%s "ANSI_COLOR_GREEN"%s"ANSI_COLOR_RESET" signal "ANSI_COLOR_YELLOW"%d"ANSI_COLOR_RESET"dBm signal_avg "ANSI_COLOR_YELLOW"%d"ANSI_COLOR_RESET"dBm\n One-Slope "ANSI_COLOR_RED"%.4f"ANSI_COLOR_RESET" Model-liniowy "ANSI_COLOR_RED"%.4f"ANSI_COLOR_RESET" Swobodnej-przestrzeni "ANSI_COLOR_RED"%.4f"ANSI_COLOR_RESET" Multi-Wall "ANSI_COLOR_RED"%.4f"ANSI_COLOR_RESET"\n\n", bssid_to_string(station.bssid,mac), 
			station.ssid, 
			station.signal_dbm, 
			station.signal_bea, 
			pow(10,(double)(station.signal_dbm+tlumienie_1m)/zanik_mocy), 
			(double)((station.signal_dbm-tlumienie_swobod_przestrz)/-15), 
			pow(10,(double)((station.signal_dbm-27.55+20*log10(2437))/-20)),
			pow(10,(double)(station.signal_dbm+tlumienie_1m-1*8)/zanik_mocy));
		}
		sleep(1);
	}
	//free the library resources
	wifi_scan_close(wifi);

	return 0;
}

void Usage(char **argv)
{
	printf("Usage:\n");
	printf("%s wireless_interface\n\n", argv[0]);
	printf("examples:\n");
	printf("%s wlan0\n", argv[0]);	
}