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
* 
*
*
*
*

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
