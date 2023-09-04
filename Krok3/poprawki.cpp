#include <bits/stdc++.h>
using namespace std;

int main(){
    int liczba_puzzli;
    cout<<"Podaj liczbe puzzli: ";
    cin>>liczba_puzzli;
    string pliki[liczba_puzzli];
    cout<<"Podaj nazwy plikow: "<<endl;
    string nazwa_pliku;
    for(int i=0; i<liczba_puzzli; i++){
        cin>>nazwa_pliku;
        pliki[i]=nazwa_pliku;
    }
    for(int i=0; i<liczba_puzzli; i++){
        ifstream fin;
        fin.open(pliki[i]);
        string nazwa_fout = "OK";
        for(int h=0; h<pliki[i].size(); h++){
            nazwa_fout += pliki[i][h];
        }
        ofstream fout;
        fout.open(nazwa_fout);
        int width = 0;
        int height = 0;

        int w=1;
        for(int i=0; i<9; i++){
            string a;
            fin>>a;
            fout<<a;
            if(i==0)
                fout<<endl;
            if(i>0 && i<8)
                fout<<" ";
            if(i==8)
                fout<<endl;
        }
        for(int i=0; i<3; i++){
            int b;
            fin>>b;
            if(i==0){
                fout<<b<<" ";
                width = b;
            }
            else{
                fout<<b<<endl;
                if(i==1)
                    height=b;
            }
        }
        int rogi[8] = {0};
        int bitmapa[height][width] = {0};
        int mapa2[height][width] = {0};
        int znalezionych_rogow =0;

        for(int i=0; i<width*height; i++){
            int a, b, c;
            fin>>a>>b>>c;
            if((a==0 && b==0 && c==0) || (a==63 && b==72 && c==204)){
                bitmapa[i/width][i%width] = 1;
                mapa2[i/width][i%width] = 1;
            }
            else{
                bitmapa[i/width][i%width] = 0;
                mapa2[i/width][i%width] = 0;
            }
        }

        cout<<"Modyfikacja pliku "<<pliki[i]<<endl;
        cout<<"Podaj wspolrzedne czterech rogow (x, y): "<<endl;
        int x;
        int y;
        for(int i=0; i<4; i++){
            cin>>x;
            cin>>y;
            if(bitmapa[y][x]==0){
                cout<<"Wybierz fragment puzzla!"<<endl;
                i--;
            }
            else{
                rogi[2*i]=x;
                rogi[2*i+1]=y;
            }
        }
        cout<<"Podaj wspolrzedne rogow prostokata, w ktorym wartosc punktow chcesz zmienic na 0 (po skonczeniu wpisz 0 0 0 0): "<<endl;
        int o;
        int p;
        int z;
        int b;
        bool end=false;
        while(end==false){
            cin>>o;
            cin>>p;
            cin>>z;
            cin>>b;
            if(o==0 && p==0 && z==0 && b==0){
                end = true;
            }
            else{
                for(int n=o; n<z+1; n++){
                    for(int m=p; m<b+1; m++){
                        bitmapa[m][n]=0;
                    }
                }
            }
        }

        cout<<"Podaj wspolrzedne rogow prostokata, w ktorym wartosc punktow chcesz zmienic na 1 (po skonczeniu wpisz 0 0 0 0): "<<endl;
        int o2;
        int p2;
        int z2;
        int b2;
        bool end2=false;
        while(end2==false){
            cin>>o2;
            cin>>p2;
            cin>>z2;
            cin>>b2;
            if(o2==0 && p2==0 && z2==0 && b2==0){
                end2 = true;
            }
            else{
                for(int n=o2; n<z2+1; n++){
                    for(int m=p2; m<b2+1; m++){
                        bitmapa[m][n]=1;
                    }
                }
            }
        }


        for(int i=0; i<height; i++){
            for(int j=0; j<width; j++){
                if(bitmapa[i][j]==0){
                    fout<<255<<endl<<255<<endl<<255<<endl;
                }
                else{
                    if((i==rogi[1] && j==rogi[0]) || (i==rogi[3] && j==rogi[2]) || (i==rogi[5] && j==rogi[4]) || (i==rogi[7] && j==rogi[6])){
                        fout<<63<<endl<<72<<endl<<204<<endl;
                        //mapa2[i][j]=
                    }
                    else{
                        fout<<0<<endl<<0<<endl<<0<<endl;
                    }
                }
            }
        }

        cout<<"Modyfikacja pliku "<<pliki[i]<<" zakonczona"<<endl;
        fin.close();
        fout.close();
    }

    return 0;
}


