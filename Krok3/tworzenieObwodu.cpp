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
        string nazwa_fout = "obwod";
        for(int h=0; h<pliki[vv].size(); h++){
            if(pliki[vv][h]=='K'){
                nazwa_fout += ".ppm";
                h=pliki[vv].size();
            }
            else{
                nazwa_fout += pliki[vv][h];
            }
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

        int dowolny_x=0;
        int dowolny_y=0;
        int bitmapa[height][width] = {0};
        int ile_mozliwosci[height][width];
        int numer_proby[height][width];
        int liczba_punktow=0;

        for(int i=0; i<width*height; i++){
            int a, b, c;
            fin>>a>>b>>c;
            if(a!=255 || b!=255 || c!=255){
                bitmapa[i/width][i%width] = 1;
                liczba_punktow++;
            }
            else{
                bitmapa[i/width][i%width] = 0;
            }
            ile_mozliwosci[i/width][i%width]=0;
            numer_proby[i/width][i%width]=0;
        }
        int pierwotne_punkty[liczba_punktow][2]={0};
        int kolejka[liczba_punktow][2];
        int wykreslone[liczba_punktow][16];
        for(int i=0; i<liczba_punktow; i++){
            for(int j=0; j<4; j++){
                kolejka[i][j/2]=0;
                wykreslone[i][4*j]=0;
                wykreslone[i][4*j+1]=0;
                wykreslone[i][4*j+2]=0;
                wykreslone[i][4*j+3]=0;
            }
        }
        int n=0;
        bool cofanie=false;
        for(int i=0; i<width*height; i++){
            if(bitmapa[i/width][i%width] == 1){
                pierwotne_punkty[n][0]=i/width;
                pierwotne_punkty[n][1]=i%width;
                n++;
            }
        }
        n=0;
        bool udalosie=false;
        int poczatkowy_punkt=0;
        int er=1;
        while(udalosie==false){
            n=0;
            cofanie=false;
            int p0_x=pierwotne_punkty[poczatkowy_punkt+er*30][1];
            int p0_y=pierwotne_punkty[poczatkowy_punkt+er*30][0];
            kolejka[0][0]=p0_y;
            kolejka[0][1]=p0_x;
            bitmapa[p0_y][p0_x]=9;
            int now_x=p0_x;
            int now_y=p0_y;
            bool niedasie=false;
            while(udalosie==false && niedasie==false){
                if(cofanie){
                    for(int u=0; u<8; u++){
                        if(wykreslone[n][2*u]!=0 || wykreslone[n][2*u+1]!=0){
                            bitmapa[wykreslone[n][2*u]][wykreslone[n][2*u+1]]=1;
                            //cout<<"Przywracamy "<<wykreslone[n][2*u]<<" "<<wykreslone[n][2*u+1]<<endl;
                            wykreslone[n][2*u]=0;
                            wykreslone[n][2*u+1]=0;
                        }
                    }
                    now_y=kolejka[n][0];
                    now_x=kolejka[n][1];
                    if(ile_mozliwosci[now_y][now_x]>numer_proby[now_y][now_x]){
                        //cout<<"Jest szansa"<<endl;
                        numer_proby[now_y][now_x]++;
                        cofanie=false;
                        int r=0;
                        int yy=now_y;
                        int xx=now_x;
                        for(int h=0; h<3; h++){
                            for(int u=0; u<3; u++){
                                if(bitmapa[yy-1+h][xx-1+u]==1){
                                    r++;
                                    if(r==numer_proby[yy][xx]){
                                        now_y=now_y-1+h;
                                        now_x=now_x-1+u;
                                        //cout<<"Znaleziono nowa szanse! "<<now_y<<" "<<now_x<<endl;
                                        kolejka[n+1][0]=now_y;
                                        kolejka[n+1][1]=now_x;
                                        bitmapa[now_y][now_x]=-1;
                                    }
                                    else{
                                        wykreslone[n][2*r]=yy-1+h;
                                        wykreslone[n][2*r+1]=xx-1+u;
                                        bitmapa[yy-1+h][xx-1+u]=-1;
                                        //cout<<"Szukanie nowej szansy, wykreslam "<<yy-1+h<<" "<<xx-1+u<<endl;
                                    }
                                }
                            }
                        }
                        n++;
                    }
                    else{
                        //cout<<"Bez szans"<<endl;
                        if(n==0){
                            niedasie=true;
                            er++;
                        }
                        kolejka[n][0]=0;
                        kolejka[n][1]=0;
                        ile_mozliwosci[now_y][now_x]=0;
                        numer_proby[now_y][now_x]=0;
                        bitmapa[now_y][now_x]=1;
                        n--;
                    }
                }
                else{
                    for(int h=0; h<3; h++){
                        for(int u=0; u<3; u++){
                            if(bitmapa[now_y-1+h][now_x-1+u]==9 && n>50){
                                udalosie=true;
                            }
                        }
                    }
                    if(udalosie==false){
                        int moz=0;
                        int yy=now_y;
                        int xx=now_x; //punkt wokol ktorego szukamy nastepnego
                        //cout<<"Przod "<<n<<" "<<now_y<<" "<<now_x<<endl;
                        for(int h=0; h<3; h++){
                            for(int u=0; u<3; u++){
                                if(bitmapa[yy-1+h][xx-1+u]==1){
                                    moz++;
                                    if(moz==1){
                                        now_y=now_y-1+h;
                                        now_x=now_x-1+u;
                                        kolejka[n+1][0]=now_y;
                                        kolejka[n+1][1]=now_x;
                                        bitmapa[now_y][now_x]=-1;
                                    }
                                    else{
                                        if(n!=0){
                                            //wykreslone przy analizowaniu punktu numer n (szukaniu wokol punktu)
                                            wykreslone[n][2*(moz-2)]=yy-1+h;
                                            wykreslone[n][2*(moz-2)+1]=xx-1+u;
                                            bitmapa[yy-1+h][xx-1+u]=-1;
                                            //cout<<"kreslimy przy wchodzeniu do pkt "<<yy-1+h<<" "<<xx-1+u<<endl;
                                        }
                                    }
                                }
                            }
                        }
                        if(moz>0){
                            ile_mozliwosci[yy][xx]=moz;
                            numer_proby[yy][xx]=1;
                            //cout<<"Ustalono nowe mozliwosci w liczbie "<<moz<<endl;
                        }
                        else{
                            if(n==0){
                                n=1;
                            }
                            cofanie=true;
                            kolejka[n][0]=0;
                            kolejka[n][1]=0;
                            bitmapa[now_y][now_x]=1;
                            n-=2;
                        }
                        n++;
                    }
                }
            }
            if(udalosie){
                for(int h=0; h<height; h++){
                    for(int k=0; k<width; k++){
                        bitmapa[h][k]=0;
                    }
                }
                for(int h=0; h<liczba_punktow; h++){
                    if(kolejka[h][0] != 0 || kolejka[h][1] != 0){
                        bitmapa[kolejka[h][0]][kolejka[h][1]]=1;
                        //cout<<kolejka[i][0]<<" "<<kolejka[i][1]<<endl;
                    }
                }
                //test

                int jed=0;
                int tro=0;
                for(int i=0; i<height; i++){
                    for(int j=0; j<width; j++){
                        if(i==0 || i==height-1 || j==0 || j==width-1 || bitmapa[i][j]==0){
                            //fout<<0<<endl<<0<<endl<<0<<endl;
                        }
                        else{
                            int ile=0;
                            for(int h=0; h<3; h++){
                                for(int u=0; u<3; u++){
                                    if(bitmapa[i-1+h][j-1+u]==1 || bitmapa[i-1+h][j-1+u]==3){
                                        ile++;
                                    }
                                }
                            }
                            if(ile==2){
                                jed++;
                                //cout<<"Jedynka: "<<i<<" "<<j<<endl;
                            }
                            if(ile!=2 && ile!=3){
                                tro++;
                                bitmapa[i][j]=3;
                                //cout<<"Trojka: "<<i<<" "<<j<<endl;
                            }
                        }
                    }
                }
                if(jed>0 || tro>0){
                    //cout<<nazwa_fout<<"----------------------"<<endl;
                    udalosie=false;
                    er++;
                    for(int i=0; i<height; i++){
                        for(int j=0; j<width; j++){
                            if(bitmapa[i][j]==3){
                                bitmapa[i][j]=1;
                            }
                            ile_mozliwosci[i][j]=0;
                            numer_proby[i][j]=0;
                        }
                    }
                    for(int i=0; i<liczba_punktow; i++){
                        kolejka[i][0]=0;
                        kolejka[i][1]=0;
                    }
                    for(int i=0; i<liczba_punktow; i++){
                        for(int j=0; j<16; j++){
                            wykreslone[i][j]=0;
                        }
                    }
                }
                else{
                    cout<<nazwa_fout<<" ok"<<endl;
                }
            }
        }



        for(int i=0; i<height; i++){
            for(int j=0; j<width; j++){
                if(bitmapa[i][j]==1 || bitmapa[i][j]==3){
                    fout<<0<<endl<<0<<endl<<0<<endl;
                }
                else{
                    fout<<255<<endl<<255<<endl<<255<<endl;
                }
            }
        }

        fin.close();
        fout.close();
    }


    return 0;
}

