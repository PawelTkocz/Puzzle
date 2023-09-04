#include <bits/stdc++.h>
using namespace std;

int main(){
    int liczba_puzzli;
    cout<<"Podaj liczbe plikow: ";
    cin>>liczba_puzzli;
    string pliki[liczba_puzzli];
    cout<<"Podaj nazwy plikow: "<<endl;
    string nazwa_pliku;
    for(int i=0; i<liczba_puzzli; i++){
        cin>>nazwa_pliku;
        pliki[i]=nazwa_pliku;
    }
    cout<<"Podaj liczbe kolumn: "<<endl;
    int ile_kolumn;
    cin>>ile_kolumn;
    cout<<"Podaj liczbe wierszy: "<<endl;
    int ile_wierszy;
    cin>>ile_wierszy;
    cout<<endl<<"Postep: "<<endl;
    int h=1;


    for(int i=0; i<liczba_puzzli; i++){
        ifstream fin;
        fin.open(pliki[i]);
        string a;
        for(int i=0; i<9; i++){
            fin>>a;
        }
        int x, y;
        fin>>x>>y;
        int nic;
        fin>>nic;
        static int tab[4000000][3];
        for(int i=0; i<x*y; i++){
            int g, t, e;
            fin>>g>>t>>e;
            tab[i][0]=g;
            tab[i][1]=t;
            tab[i][2]=e;
        }
        for(int i=0; i<ile_kolumn*ile_wierszy; i++){
                ofstream fout;
                string nazwa="p";
                nazwa+=to_string(h+i);
                nazwa+=".ppm";
                fout.open(nazwa);
                fout<<"P3"<<endl;
                fout<<"# Created by GIMP version 2.10.22 PNM plug-in"<<endl;
                fout<<x/ile_kolumn<<" "<<y/ile_wierszy<<endl;
                fout<<255<<endl;
                int rog_x=(i%ile_kolumn)*(x/ile_kolumn);
                int rog_y=(i/ile_kolumn)*(y/ile_wierszy);
                for(int j=0; j<y/ile_wierszy; j++){
                    for(int k=0; k<x/ile_kolumn; k++){
                        fout<<tab[x*(rog_y+j)+rog_x+k][0]<<endl;
                        fout<<tab[x*(rog_y+j)+rog_x+k][1]<<endl;
                        fout<<tab[x*(rog_y+j)+rog_x+k][2]<<endl;
                    }
                }
                cout<<nazwa<<" gotowy"<<endl;
                fout.close();
        }
        h+=ile_kolumn*ile_wierszy;

        fin.close();
    }

}
