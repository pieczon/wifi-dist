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

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

struct wifi_scan *wifi = NULL;
struct station_info station;    
char mac[BSSID_STRING_LENGTH], key;
int conn_stat, tlumienie_1m;
double indeks_gamma, zanik_mocy, tlumienie_swobod_przestrz;

static struct termios g_old_kbd_mode;

sem_t watek1, watek2;
pid_t pid;

static int keypressed(void) //did somebody press somthing??? 
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

float mod_OneSlope(int sta_signal, int supp_1m, double absorption, int dbi_gain)
{
	float pomiar = pow(10,(double)((sta_signal - dbi_gain) + supp_1m)/absorption);
	return pomiar;
}//pow(10,(double)((station.signal_dbm-20)+tlumienie_1m)/zanik_mocy)

float mod_ModelLiniowy(int sta_signal, double supp_freespace, int dbi_gain)
{
	float pomiar = (double)(((sta_signal - dbi_gain) - supp_freespace)/-15);
	return pomiar;
}//(double)(((station.signal_dbm-20)-tlumienie_swobod_przestrz)/-15)

float mod_SwobodnejPrzestrzeni(int sta_signal, int dbi_gain)
{
	float pomiar = pow(10,(double)(((sta_signal - dbi_gain)-27.55 + 20*log10(2437))/-20));
	return pomiar;
}//pow(10,(double)(((station.signal_dbm-20)-27.55+20*log10(2437))/-20))

float mod_MultiWall(int sta_signal, int dbi_gain, int supp_1m, double absorption)
{
	float pomiar = pow(10,(double)((sta_signal - dbi_gain) + supp_1m-1 * 8)/absorption);
	return pomiar;
}//pow(10,(double)((station.signal_dbm-20)+tlumienie_1m-1*8)/zanik_mocy))

void *pomiary(void *arg)
{
	for(;;)
		{	
			sem_wait(&watek1);
			//printf("\n-----jestem 1 wątek!-----\n");

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
				printf("%s "ANSI_COLOR_GREEN"%s"ANSI_COLOR_RESET" signal "ANSI_COLOR_YELLOW"%d"ANSI_COLOR_RESET"dBm signal_avg "ANSI_COLOR_YELLOW"%d"ANSI_COLOR_RESET"dBm\n One-Slope "ANSI_COLOR_RED"%.2f"ANSI_COLOR_RESET" Model-liniowy "ANSI_COLOR_RED"%.2f"ANSI_COLOR_RESET" Swobodnej-przestrzeni "ANSI_COLOR_RED"%.2f"ANSI_COLOR_RESET" Multi-Wall "ANSI_COLOR_RED"%.2f"ANSI_COLOR_RESET"\n\n", bssid_to_string(station.bssid,mac), 
				station.ssid, 
				station.signal_dbm, 
				station.signal_bea, 
				mod_OneSlope(station.signal_dbm, tlumienie_1m, zanik_mocy, 20),
				mod_ModelLiniowy(station.signal_dbm, tlumienie_swobod_przestrz, 20), 
				mod_SwobodnejPrzestrzeni(station.signal_dbm, 20),
				mod_MultiWall(station.signal_dbm, 20, tlumienie_1m, tlumienie_swobod_przestrz));
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
		//printf("\n-----jestem 2 wątek!-----\n");

		//printf("Press 'q' for exit \n"); 
		if(keypressed())
		{ 
			key = getchar(); //getting the pressed key... 
			printf("Wciśnięto klawisz - %c\n", key); 
			if(key == 27)
			{ 
				printf("Naciśnięto ESC\n");
				printf("baju :)\n"); 
				exit(1); 
			} 
		}

		// pid = fork();
		// if(pid == -1)
		// {
		// 	perror("fork error\n");	
		// }
		// if(pid == 0)
		// {
		// 	printf("child pid = %d\n",getpid());
		// 	printf("child parent pid = %d\n", getppid());
			
		// }
		// else if(pid > 0)
		// {
		// 	printf("parent pid = %d\n",getpid());
		// 	printf("parent parent pid = %d\n",getppid());
		// }
		sem_post(&watek1);
    }
}

int main(int argc, char **argv)
{
	printf(ANSI_COLOR_YELLOW"\nProgram oblicza szacunkową odległość połączonego urządzenia w sieci Wi-Fi od Routera/AP, w metrach [m] z dokładnością do 2 miejsc po przecinku\n"ANSI_COLOR_RESET);
	printf(ANSI_COLOR_YELLOW"Zakończenie pomiarów = Q\n\n"ANSI_COLOR_RESET);

	printf(ANSI_COLOR_GREEN"Podaj wartość tlumienia w odległości 1 [m]: "ANSI_COLOR_RESET);
	scanf("%d", &tlumienie_1m); //zabezpieczyc przed wpisaniem dowolnych znakow
	printf(ANSI_COLOR_GREEN"Podaj wartość indeksu gamma: "ANSI_COLOR_RESET);
	scanf("%lf", &indeks_gamma); //zabezpieczyc przed wpisaniem dowolnych znakow
	printf("\n\n");

	zanik_mocy = indeks_gamma*10;
	tlumienie_swobod_przestrz = (-27.55+20*log10(2437)+20*log10(9)); //to przejrzec, sprawdzic co znaczy kazda liczba jeszcze raz, przekalkulowac nowa zmienna dbi_gain

	if(argc != 2)
	{
		Usage(argv);
		return 0;
	}
	wifi = wifi_scan_init(argv[1]);	// initialize the library with network interface argv[1] (e.g. wlan0)
	
	struct termios new_kbd_mode; 
	tcgetattr(0, &g_old_kbd_mode); //put keyboard (stdin, actually) in raw, unbuffered mode
	memcpy(&new_kbd_mode, &g_old_kbd_mode, sizeof(struct termios)); 
	
	new_kbd_mode.c_lflag &= ~(ICANON | ECHO); 
	new_kbd_mode.c_cc[VTIME] = 0; 
	new_kbd_mode.c_cc[VMIN] = 1; 
	tcsetattr(0, TCSANOW, &new_kbd_mode); 
	atexit(old_attr); 	

	sem_init(&watek1,0,1); //-----------tu wywołanie wątków obsługujących pomiary
	sem_init(&watek2,0,0);
	pthread_t pomiary_thread, koniec_thread;
	pthread_create(&pomiary_thread, NULL, pomiary, NULL);
	pthread_create(&koniec_thread, NULL, koniec, NULL);
	pthread_join(pomiary_thread, NULL);
	pthread_join(koniec_thread, NULL);

	wifi_scan_close(wifi);
	return 0;
}