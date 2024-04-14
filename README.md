# Puzzle
Projekt rozpocząłem podczas wakacji po zdaniu matury, zdecydowana większość kodu została napisana jeszcze przed rozpoczęciem studiów.
Moim celem było napisanie programu, który po zeskanowaniu puzzli będzie analizował ich kształt i pomagał użytkownikowi w ich poprawnym łączeniu.
Program rozwijałem i testowałem na układance składającej się z 1008 puzzli (rozmiar 36 x 28) - skany puzzli są dostępne w folderze Skany. 
Projekt nie jest jeszcze ukończony, obecnie program kończy działanie po ułożeniu ramki puzzli (co dla testowanej przeze mnie układanki robi poprawnie,
prezentując sposób na ułożenie ramki składającej się z 124 puzzli).

## Sposób działania
### 1. Skanowanie puzzli
Na samym początku zeskanowałem wszystkie 1008 puzzli, w partiach po 35 puzzli. Po zeskanowaniu każdej partii, numerowałem puzzle ołówkiem,
od lewej do prawej (i od góry do dołu). W ten sposób otrzymałem 29 skanów (dostępnych w folderze Skany), a każdy puzzel został oznaczony
unikalną liczbą z zakresu 1-1008.
### 2.  Uruchomienie programu solve.py
Program solve.py korzysta najpierw z pythonowego programu ScanSlicer by pociąć skany z folderu Scans na skany pojedyńczych puzzli (i zapisać je
w formie bitmapy do folderu Puzzles). Następnie program kompiluje i uruchamia program puzzle_solver (napisany w całości w języku C) odpowiedzialny
za proces przetwarzania skanów puzzli i próbę ułożenia układanki.

## Opis działania programu ScanSlicer
Na wejściu program dostaje nazwę folderu, w którym znajdują się skany puzzli (u mnie jest to folder "Skany"). Następnie program przetwarza każdy z tych
skanów rozbijając go na mniejsze, w taki sposób że każdy puzzel trafia do osobnego pliku. Ponieważ następne kroki będą opierały się na odczytywaniu
wartości RGB pikseli, zamast zapisywać pliki w formacie jpg, program zapisuje każdego puzzla w formie bitmapy w następujący sposób:
width height
R_0_0 G_0_0 B_0_0
R_0_1 G_0_1 B_0_1.
.
.
.
R_(height-1)_(width-1) G_(height-1)_(width-1) B_(height-1)_(width-1)
Gdzie R_y_x to wartość Red piksela znajdującego się w rzędzie y i kolumnie x. Analogicznie dla wartości G Green i B Blue. Wynikiem działania programu jest
utworzenie nowego folderu Puzzles i zapisanie w nim 1008 plików opisujących bimapę każdego puzzla w sposób przedstawiony wyżej.

## Opis działania programu puzzle_solver
### 1. Przetwarzanie skanu każdego puzzla
Dla każdego pliku opisującego bitmapę puzzla w folderze Puzzles program najpierw wykryje krawędzie puzzla, następnie zlokalizuje jego cztery rogi, a na koniec
zapamięta puzzla jako 4 krawędzie (każda krawędź jest zapisana jako ciąg współrzędnych (x, y) oraz typ - krawędź może być wypukła, wklęsłą lub płaska, czyli tworzyć ramkę).

![PrzetwarzaniePuzzla](https://github.com/PawelTkocz/Puzzle/assets/143350344/0ccf5da1-3caf-41b4-a212-b4de0406e17a)
(każdy z 4 przedstawionych powyżej obrazów został wygenerowany przez program wykorzystując funkcje służące do wizualizacji postępów)

### 2. Próba łączenia puzzli
Po przetworzeniu wszystkich puzzli program rozpoczyna próbę ułożenia układanki. Zaczyna (i na obecną chwilę na tym kończy) od ułożenia ramki puzzli. Aby
to zrobić oddziela od reszty puzzle tworzące ramkę (124 z 1008) i próbuje je ze sobą łączyć korzystając z funkcji obliczającej współczynnik dopasowania.
Współczynnik ten jest obliczany na podstawie podobieństwa kształu odpowiednich krawędzi.  
