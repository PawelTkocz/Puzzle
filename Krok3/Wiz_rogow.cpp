#include <bits/stdc++.h>
using namespace std;


int main(){
    int ile_puzzli;
    cout<<"Podaj ilosc puzzli: ";
    cin>>ile_puzzli;

    cout<<"Podaj nazwy plikow: ";
    string pliki[ile_puzzli];
    string nazwa_pliku;

    for(int qq=0; qq<ile_puzzli; qq++){
        cin>>nazwa_pliku;
        pliki[qq]=nazwa_pliku;
    }
    cout<<endl<<"Postep: "<<endl;
    for(int vv=0; vv<ile_puzzli; vv++){
        ifstream fin;
        fin.open(pliki[vv]);
        string nazwa_fout = "SprRogi";
        for(int h=4; h<pliki[vv].size(); h++){
            nazwa_fout += pliki[vv][h];
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
        int znalezionych_rogow =0;
        int rogi[4][2]={0};

        for(int i=0; i<width*height; i++){
            int a, b, c;
            fin>>a>>b>>c;
            if(a==63 && b==72 && c==204){
                bitmapa[i/width][i%width] = 1;
                rogi[znalezionych_rogow][0]=i/width;
                rogi[znalezionych_rogow][1]=i%width;
                znalezionych_rogow++;
            }
            if(a!=255 || b!=255 || c!=255){
                bitmapa[i/width][i%width] = 1;
            }
            else{
                bitmapa[i/width][i%width] = 0;
            }
        }



        //rysowanie puzzla
        int grubosc_kropy=7;
        for(int i=0; i<4; i++){
            int x=rogi[i][1];
            int y=rogi[i][0];
            for(int k=0; k<grubosc_kropy; k++){
                for(int h=0; h<grubosc_kropy; h++){
                    int t=y-grubosc_kropy/2+k;
                    int r=x-grubosc_kropy/2+h;
                    if(t>0 && t<height && r>0 && r<width)
                        bitmapa[t][r]=9;
                }
            }
        }
        for(int i=0; i<height; i++){
            for(int j=0; j<width; j++){
                if(bitmapa[i][j]==1){
                    fout<<0<<endl<<0<<endl<<0<<endl;
                }
                else if(bitmapa[i][j]==9){
                    fout<<0<<endl<<0<<endl<<255<<endl;
                }
                else{
                    fout<<255<<endl<<255<<endl<<255<<endl;
                }
            }
        }

        cout<<nazwa_fout<<" gotowe!"<<endl;;
        fin.close();
        fout.close();
    }


    return 0;
}


