#include <stdio.h>
#include <stdlib.h>

bool get_width_points(double width1, double width2, double *res){
    double longer = width1;
    double shorter = width2;
    if(width2 > width1){
        longer = width2;
        shorter = width1;
    }

    double margin = 0.2*longer;
    double diff = longer-shorter;

    if(diff > margin)
        return true;

    double alfa = 50.0;
    return alfa*diff/margin;
}

bool get_shape_points(struct PuzzleSide s1, struct PuzzleSide s2){
    //gdzie z lewej strony zacczyna sie shape gdzie z prawej
    //i na jakiej wysokosci shape jest najszersze
    //przydaloby sie tez xy gdzie zaczyna sie ksztalt
    return 1;
}

bool get_height_points(struct PuzzleSide s1, struct PuzzleSide s2){
    //wysokosc nad i pod krawedzia
    return 1;
}

double puzzle_match_val(struct PuzzleSide s1, struct PuzzleSide s2){
    //nakladamy najpierw bok 1 na bok 2 i liczymy pole puzzla 2 ktore nie zostanie przykryte
    //nastepnie to samo tylko nakladamy bok 2 na bok 2
    //suma tych wartosci bedzie wyznaczala jak bardzo boki do siebie pasuja - im mniej tym lepiej
    //wartosci kolejnych pikseli podane s¹ jako double
    //aby nie tracic dokladnosci mozna przelozyc je na bitmape ale np 10x dokladniejsza
    //w ten sposob zamiast +/- 1piksel skonczymy na +/- 0.1piksela dokladnosci po obrocie
    double width_points;
    bool no_match = get_width_points(s1.width, s2.width, &width_points);
    if(no_match)
        return 0;

    return 0;
}
