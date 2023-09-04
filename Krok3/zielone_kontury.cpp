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
    cout<<endl<<"Postep: "<<endl;


    for(int i=0; i<liczba_puzzli; i++){
        ifstream fin;
        fin.open(pliki[i]);
        string nazwa_fout = "";
        for(int h=0; h<pliki[i].size(); h++){
            if(pliki[i][h]=='.'){
                nazwa_fout += "Kontury";
            }
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

        int bitmapa[height][width] = {0};
        int mapa2[height][width] = {0};


        for(int i=0; i<width*height; i++){
            int a, b, c;
            fin>>a>>b>>c;
            if(abs(a-b)>40 || abs(a-c)>40 || abs(c-b)>40 || a+b+c<210 || a+b+c>720 || i/width<10 || i/width>height-10 || i%width<10 || i%width>width-10 ){
                bitmapa[i/width][i%width] = 0;
                mapa2[i/width][i%width] = 0;
            }
            else{
                bitmapa[i/width][i%width] = 1;
                mapa2[i/width][i%width] = 1;
            }
        }


        for(int i=0; i<height; i++){
            for(int j=0; j<width; j++){
                if(bitmapa[i][j]==0){
                    fout<<255<<endl<<255<<endl<<255<<endl;
                }
                else{
                    if(i==0 || i==height-1 || j==0 || j==width-1){
                        fout<<0<<endl<<0<<endl<<0<<endl;
                    }
                    else{
                        if(bitmapa[i][j+1]==1 && bitmapa[i][j-1]==1 && bitmapa[i+1][j]==1 && bitmapa[i-1][j]==1){
                            fout<<255<<endl<<255<<endl<<255<<endl;
                        }
                        else{
                            fout<<0<<endl<<0<<endl<<0<<endl;
                        }
                    }
                }
            }
        }

        cout<<pliki[i]<<" gotowy"<<endl;
        fin.close();
        fout.close();
    }

    return 0;
}

