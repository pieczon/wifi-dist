#include "wifi_scan.h"
#include <stdio.h>
#include <unistd.h>
#include <math.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

const char* bssid_to_string(const uint8_t bssid[BSSID_LENGTH], char bssid_string[BSSID_STRING_LENGTH])
{
	snprintf(bssid_string, BSSID_STRING_LENGTH, "%02x:%02x:%02x:%02x:%02x:%02x",bssid[0],bssid[1],bssid[2],bssid[3],bssid[4],bssid[5]);
	return bssid_string;
}

void Usage(char **argv)
{
	printf("Usage:\n");
	printf("%s wireless_interface\n\n", argv[0]);
	printf("examples:\n");
	printf("%s wlan0\n", argv[0]);	
}

float pomiar_OneSlope(int *sta_signal, int tlumienie_1m, double zanik_mocy)
{
	float pomiar = 0;
	pomiar = pow(10,(double)((station.signal_dbm-20)+tlumienie_1m)/zanik_mocy);
	return pomiar;
} 

float pomiar_ModelLiniowy(int *sta_signal, double tlumienie_swobod_przestrz)
{
	(double)(((station.signal_dbm-20)-tlumienie_swobod_przestrz)/-15);
}

float pomiar_SwobodnejPrzestrzeni(int *sta_signal)
{
	pow(10,(double)(((station.signal_dbm-20)-27.55+20*log10(2437))/-20));
}

float pomiar_MultiWall(int *sta_signal)
{
	pow(10,(double)((station.signal_dbm-20)+tlumienie_1m-1*8)/zanik_mocy);
}

int main(int argc, char **argv)
{
	struct wifi_scan *wifi = NULL;    //this stores all the library information
	struct station_info station;    //this is where we are going to keep information about AP (Access Point) we are connected to
	char mac[BSSID_STRING_LENGTH];  //a placeholder where we convert BSSID to printable hardware mac address
	int status;

	int tlumienie_1m = 0;
	printf("Podaj wartość tlumienia w odległości 1 [m]: ");
	scanf("%d", &tlumienie_1m);

	double indeks_gamma = 0;
	printf("Podaj wartość indeksu gamma: ");
	scanf("%lf", &indeks_gamma);
	
	const double zanik_mocy = indeks_gamma*10;
	const double tlumienie_swobod_przestrz = (-27.55+20*log10(2437)+20*log10(9));

	if(argc != 2)
	{
		Usage(argv);
		return 0;
	}

	printf("Program oblicza szacowaną odległość [m] podłączonego urządzenia w sieci wi-fi od routera/AP\n");
	printf("Do zakończenia pomiarów naciśnij ESC\n\n");
	
	// initialize the library with network interface argv[1] (e.g. wlan0)
	wifi=wifi_scan_init(argv[1]);

	do
	{
		status = wifi_scan_station(wifi, &station);
		
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
			pow(10,(double)((station.signal_dbm-20)+tlumienie_1m)/zanik_mocy), 
			(double)(((station.signal_dbm-20)-tlumienie_swobod_przestrz)/-15), 
			pow(10,(double)(((station.signal_dbm-20)-27.55+20*log10(2437))/-20)),
			pow(10,(double)((station.signal_dbm-20)+tlumienie_1m-1*8)/zanik_mocy));
		}
		sleep(1);
		
	}while(getchar() != '\027');

	wifi_scan_close(wifi);

	return 0;
}