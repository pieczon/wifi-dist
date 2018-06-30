# wifi-dist-ap

## Cel i założenia projektu
* program oblicza szacunkową odległość urządzenia od Routera/Access Pointa
* odległość wyliczana jest na podstawie 4 różnych modeli propagacyjnych
* program dokonuje pomiarów na podstawie parametrów sygnału połączonej sieci Wi-Fi

## Użyte technologie i biblioteki
* C, threads
* small C/C++ library for monitoring signal strength of WiFi networks (https://github.com/bmegli/wifi-scan.git)
* Netlink Library (libnl)
* Minimalistic Netlink Library (libmnl) doxygen
* nl80211 header file, netlink nl80211 user space

## Wymagania funkcjonalne
* wybór interfejsu sieciowego, prezentacja informacji o nazwie podłączonej sieci i MAC adresie routera/AP
* pobieranie od użytkownika parametrów do wyliczeń: indeks odległościowego zaniku mocy, wartość tłumienia w odległości 1 metr, liczba przeszkód na drodze sygnału (ścian/stropów), częstotliwość nadawanego sygnału
* walidacja wprowadzonych danych, zakresów referencyjnych oraz eliminacja błędnych wartości (wpisanie liter i innych niepoprawnych znaków)
* działanie na 2 wątkach, jeden przelicza modele propagacyjne, drugi przełącza terminal w tryb niebuforowany i czeka na sygnał do zakończenia programu
* prezentacja informacji o sile sygnału dBm, wyliczanie średniej siły sygnału dBm
* informacje o wyliczonych odległościach w metrach z dokładnością do 2 miejsc po przecinku, dla modeli: One-Slope, Liniowego, Swobodnej Przestrzeni, Multi-Wall

## Wymagania niefunkcjonalne
* OS wspierający NL80211 netlink interface (systemy linux/unix)
* karta sieciowa posiadająca sterownik cfg80211/mac80211 dla urządzeń SoftMAC/FuLLMAC (wszystkie nowe wydania systemów linux)
* pliki wykorzystywanej biblioteki: wifi_scan.c, wifi_scan.h, Makefile
* instalacja libmnl0 libmnl-dev

## Testy
* manualne
*
*
*
*

## Podsumowanie
* jak rozwinąć, wizualizacja położenia AP
* jakie były problemy
*
*
*

![wifi-dist-work](https://github.com/pieczon/wifi-dist/blob/master/Zrzut%20ekranu%20z%202018-05-25%2001-13-48.png)
