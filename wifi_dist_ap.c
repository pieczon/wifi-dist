#include "wifi_scan.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include <termios.h>
#include <sys/time.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[93m"
#define ANSI_COLOR_ORANGE  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

struct wifi_scan *wifi = NULL;
struct station_info station;    
char mac[BSSID_STRING_LENGTH], key, inputs[10];
int conn_stat, sg, st, ss, sf, tlumienie_1m, l_scian, sig_freq;
float indeks_gamma, zanik_mocy, tlumienie_swobod_przestrz;

static struct termios g_old_kbd_mode;

sem_t watek1, watek2;
pid_t pid;

static int keyPressed(void) //did somebody press somthing??? 
{ 
    struct timeval timeout; 
    fd_set read_handles; 
    int status; 
    
    FD_ZERO(&read_handles); //check stdin (fd 0) for activity 
    FD_SET(0, &read_handles); 
    timeout.tv_sec = timeout.tv_usec = 0; 
    status = select(0 + 1, &read_handles, NULL, NULL, &timeout); 
    return status; 
} 

static void old_attr(void) //put the things as they were befor leave..!!!
{ 
    tcsetattr(0, TCSANOW, &g_old_kbd_mode); 
} 

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

bool isNumber(char *string, int size)
{
    for (int i=0; i<size; i++)
	{
		if(string[i] != '\0')
		{
			if(isdigit(string[i]) == false)
			{
				return false;
			}
		}
	}
    return true;
}

bool isValueInArray(int val, int *arr, int size)
{
    int i;
    for (i=0; i < size; i++) 
	{
        if (arr[i] == val)
            return true;
    }
    return false;
}

bool isFloat(char *string, int size)
{
    for (int i=0; i<size; i++)
	{
		if( (string[i] != '\0') && (string[i] != '.') )
		{
			if(isdigit(string[i]) == false)
			{
				return false;
			}
		}
	}
    return true;
}

float mod_OneSlope(int sta_signal, int supp_1m, float absorption, int dbi_gain)
{
	float pomiar = pow(10,(float)((sta_signal - dbi_gain + supp_1m)/absorption));
	return pomiar;
} 

float mod_ModelLiniowy(int sta_signal, float supp_freespace, int dbi_gain)
{
	float pomiar = (float)((sta_signal - dbi_gain - supp_freespace)/-15);
	return pomiar;
}

float mod_SwobodnejPrzestrzeni(int sta_signal, int freq, int dbi_gain)
{
	float pomiar = pow(10,(float)((sta_signal -27.55 + 20*log10(freq))/-20));
	return pomiar;
}

float mod_MultiWall(int sta_signal, int supp_1m, float absorption, int wall_no, int dbi_gain)
{
	float pomiar = pow(10,(float)((sta_signal + supp_1m - wall_no*8)/absorption));
	return pomiar;
}

void *pomiary(void *arg)
{
	for(;;)
	{	
		sem_wait(&watek1);

		conn_stat = wifi_scan_station(wifi, &station);
		if(conn_stat == 0)
		{
			printf("Brak połączenia Wi-Fi\n");
		}
		else if(conn_stat == -1)
		{
			perror("Nie można uzyskać informacji o połączeniu\n");
		}
		else
		{
			printf("%s "ANSI_COLOR_GREEN"%s"ANSI_COLOR_RESET" signal_strength[dBm]"ANSI_COLOR_ORANGE"%d"ANSI_COLOR_RESET" signal_average[dBm]"ANSI_COLOR_ORANGE"%d"ANSI_COLOR_RESET" bss_freq[MHz]"ANSI_COLOR_ORANGE"%u"ANSI_COLOR_RESET"\n One-Slope "ANSI_COLOR_RED"%.2f"ANSI_COLOR_RESET" Model-liniowy "ANSI_COLOR_RED"%.2f"ANSI_COLOR_RESET" Swobodnej-przestrzeni "ANSI_COLOR_RED"%.2f"ANSI_COLOR_RESET" Multi-Wall "ANSI_COLOR_RED"%.2f"ANSI_COLOR_RESET"\n\n", bssid_to_string(station.bssid,mac), 
			station.ssid, 
			station.signal_dbm, 
			station.signal_avg, 
			station.bss_freq, 
			mod_OneSlope(station.signal_dbm, tlumienie_1m, zanik_mocy, 20), 
			mod_ModelLiniowy(station.signal_dbm, tlumienie_swobod_przestrz, 20), 
			mod_SwobodnejPrzestrzeni(station.signal_dbm, sig_freq, 20), 
			mod_MultiWall(station.signal_dbm, tlumienie_1m, zanik_mocy, l_scian, 20));
		}
		sleep(1);
		sem_post(&watek2);
	}
}

void *koniec(void *arg)
{
    for(;;)
    {
        sem_wait(&watek2);
		if(keyPressed())
		{ 
			key = getchar();

			if(key == 27)
			{ 
				printf("Naciśnięto ESC\n");
				printf("baju :)\n"); 
				exit(1); 
			} 
		}
		sem_post(&watek1);
    }
}

int main(int argc, char **argv)
{
	printf(ANSI_COLOR_ORANGE"\nProgram oblicza szacunkową odległość połączonego urządzenia w sieci Wi-Fi od Routera/AP.\nPomiar prezentowany jest w metrach "ANSI_COLOR_RED"[m]"ANSI_COLOR_ORANGE", z dokładnością do 2 miejsc po przecinku.\n\n"ANSI_COLOR_RESET);

	printf(ANSI_COLOR_GREEN"Podaj szacowaną wartość tłumienia w odległości 1"ANSI_COLOR_RED"[m]"ANSI_COLOR_GREEN" od Routera/AP [1 - 80]: "ANSI_COLOR_RESET);
	while((tlumienie_1m < 1) || (tlumienie_1m > 80))
	{
		if(inputs[0])
		{
			printf(ANSI_COLOR_RED"Błędna liczba, wpisz ponownie: "ANSI_COLOR_RESET);
		}
		memset(inputs, 0, 10*sizeof(char));
		st = scanf("%s", inputs);
		int size = sizeof(inputs)/sizeof(char);
		while(isNumber(inputs, size) == false)
		{	
			printf(ANSI_COLOR_RED"Nie podano liczby, wpisz ponownie: "ANSI_COLOR_RESET);
			memset(inputs, 0, 10*sizeof(char));
			st = scanf("%s", inputs);
			size = sizeof(inputs)/sizeof(char);
		}
		tlumienie_1m = atoi(inputs);
	}
	
	printf(ANSI_COLOR_GREEN"Podaj wartość indeksu gamma [3.5 - 6]: "ANSI_COLOR_RESET);
	memset(inputs, 0, 10*sizeof(char));
	while((indeks_gamma < 3.5) || (indeks_gamma > 6))
	{
		if(inputs[0])
		{
			printf(ANSI_COLOR_RED"Błędna liczba, wpisz ponownie: "ANSI_COLOR_RESET);
		}
		memset(inputs, 0, 10*sizeof(char));
		sg = scanf("%s", inputs);
		int size = sizeof(inputs)/sizeof(char);
		while(isFloat(inputs, size) == false)
		{	
			printf(ANSI_COLOR_RED"Nie podano liczby, wpisz ponownie: "ANSI_COLOR_RESET);
			memset(inputs, 0, 10*sizeof(char));
			sg = scanf("%s", inputs);
			size = sizeof(inputs)/sizeof(char);
		}
		indeks_gamma = atof(inputs);
	}

	printf(ANSI_COLOR_GREEN"Podaj liczbę ścian/stropów na drodze sygnału: "ANSI_COLOR_RESET);
	memset(inputs, 0, 10*sizeof(char));
	while(l_scian <= 0)
	{
		if(inputs[0])
		{
			printf(ANSI_COLOR_RED"Błędna liczba, wpisz ponownie: "ANSI_COLOR_RESET);
		}
		memset(inputs, 0, 10*sizeof(char));
		ss = scanf("%s", inputs);
		int size = sizeof(inputs)/sizeof(char);
		while(isNumber(inputs, size) == false)
		{	
			printf(ANSI_COLOR_RED"Nie podano liczby, wpisz ponownie: "ANSI_COLOR_RESET);
			memset(inputs, 0, 10*sizeof(char));
			ss = scanf("%s", inputs);
			size = sizeof(inputs)/sizeof(char);
		}
		l_scian = atoi(inputs);
	}

	printf(ANSI_COLOR_GREEN"Podaj częstotliwość nadawanego sygnału [2.4GHz: 2412-2472MHz | 5GHz: 5180-5700MHz]: "ANSI_COLOR_RESET);
	memset(inputs, 0, 10*sizeof(char));
	while(((sig_freq < 2412) || (sig_freq > 2472)) && ((sig_freq < 5180) || (sig_freq > 5700)))
	{
		if(inputs[0])
		{
			printf(ANSI_COLOR_RED"Błędna liczba, wpisz ponownie: "ANSI_COLOR_RESET);
		}
		memset(inputs, 0, 10*sizeof(char));
		sf = scanf("%s", inputs);
		int size = sizeof(inputs)/sizeof(char);
		while(isNumber(inputs, size) == false)
		{	
			printf(ANSI_COLOR_RED"Nie podano liczby, wpisz ponownie: "ANSI_COLOR_RESET);
			memset(inputs, 0, 10*sizeof(char));
			sf = scanf("%s", inputs);
			size = sizeof(inputs)/sizeof(char);
		}
		sig_freq = atoi(inputs);
	}

	printf("\n\nUstawione tlumienie: "ANSI_COLOR_YELLOW"%d"ANSI_COLOR_RESET"\n", tlumienie_1m);
	printf("Ustawiony indeks gamma: "ANSI_COLOR_YELLOW"%.2f"ANSI_COLOR_RESET"\n", indeks_gamma);
	printf("Ustawiona liczba ścian/stropów: "ANSI_COLOR_YELLOW"%d"ANSI_COLOR_RESET"\n", l_scian);
	printf("Ustawiona częstotliwość sygnału: "ANSI_COLOR_YELLOW"%d"ANSI_COLOR_RESET"\n", sig_freq);

	printf(ANSI_COLOR_CYAN"\nParametry poprawne, rozpoczynam obliczenia...(zakończenie pomiarów klawisz "ANSI_COLOR_YELLOW"ESC"ANSI_COLOR_RESET")\n\n"ANSI_COLOR_RESET);

	zanik_mocy = indeks_gamma*(-10);
	tlumienie_swobod_przestrz = (-27.55+20*log10(2462)+20*log10(9));
	if(argc != 2)
	{
		Usage(argv);
		return 0;
	}
	wifi = wifi_scan_init(argv[1]);	//inicjalizacja funkcji biblioteki na interfejsie sieciowym lapka wlp4s0
	
	struct termios new_kbd_mode; 
	tcgetattr(0, &g_old_kbd_mode); //stdin w trybie niebuforowanym
	memcpy(&new_kbd_mode, &g_old_kbd_mode, sizeof(struct termios)); 
	
	new_kbd_mode.c_lflag &= ~(ICANON | ECHO); 
	new_kbd_mode.c_cc[VTIME] = 0; 
	new_kbd_mode.c_cc[VMIN] = 1; 
	tcsetattr(0, TCSANOW, &new_kbd_mode); 
	atexit(old_attr); //przywrocenie trybu buforowanego stdin

	sem_init(&watek1,0,1);
	sem_init(&watek2,0,0);
	pthread_t pomiary_thread, koniec_thread;
	pthread_create(&pomiary_thread, NULL, pomiary, NULL);
	pthread_create(&koniec_thread, NULL, koniec, NULL);
	pthread_join(pomiary_thread, NULL);
	pthread_join(koniec_thread, NULL);

	wifi_scan_close(wifi);
	return 0;
}