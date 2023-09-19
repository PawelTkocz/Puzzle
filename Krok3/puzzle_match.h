#include <stdio.h>
#include <stdlib.h>

double puzzle_match_val(struct PuzzleSide s1, struct PuzzleSide s2){
    //nakladamy najpierw bok 1 na bok 2 i liczymy pole puzzla 2 ktore nie zostanie przykryte
    //nastepnie to samo tylko nakladamy bok 2 na bok 2
    //suma tych wartosci bedzie wyznaczala jak bardzo boki do siebie pasuja - im mniej tym lepiej
    //wartosci kolejnych pikseli podane s¹ jako double
    //aby nie tracic dokladnosci mozna przelozyc je na bitmape ale np 10x dokladniejsza
    //w ten sposob zamiast +/- 1piksel skonczymy na +/- 0.1piksela dokladnosci po obrocie
    return 0;
}
