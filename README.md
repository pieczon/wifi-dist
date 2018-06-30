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
* prezentacja informacji o sile sygnału dBm, wyliczanie średniej siły sygnału dBm, częstotliwości sygnału MHz
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
* pomysły na rozwinięcie projektu: 
* dodanie obliczeń dla kolejnych modeli propagacyjnych: ITU-R P.1238, Motleya-Keenana, nieliniowy model Multi-Wall (COST231)
* wizualizacja położenia routera/AP z wykorzystaniem odbiornika GPS (w środowisku zewnętrznym), na podstawie podania połżenia komputera (w środowisku wewnątrz budynkowym)
* odczytywanie informacji o bieżącej częstotliwości sygnału wi-fi i kanale komunikacji, reorganizacja kodu aplikacji (np. tylko wywołanie funkcji w main(), funkcje w plikach .h), przepisanie aplikacji do C++, opracowanie ładniejszego interfejsu/sposobu prezentacji danych
* jakie były problemy:
* walidacja wprowadzanych przez użytkownika danych w języku C jest nietrywialna :)
* zakończenie pracy programu, działającego w pętli nieskończonej poprzez naciśnięcie np. klawisza ESC jest nietrywialne :)
* wyliczanie odległości połączonych urządzeń w sieci wi-fi jest bardzo relatywne

![wifi-dist-work](https://github.com/pieczon/wifi-dist/blob/master/Zrzut%20ekranu%20z%202018-05-25%2001-13-48.png)
