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
* Any platforms supporting nl80211 netlink interface (new 802.11 interface). Generally *nix platforms.
* Wireless devices that have cfg80211/mac80211 driver. Currently all new Linux wireless drivers should be written targeting either cfg80211 for fullmac devices or mac80211 for softmac devices.
*
*
*

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
