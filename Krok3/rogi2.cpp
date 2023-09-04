#include <bits/stdc++.h>
using namespace std;

double kat(double p1, double p2){
    if(p1*p2==-1)
        return 90;
    if(p1==p2)
        return 0;
    double tang = abs((p1-p2)/(1+p1*p2));
    double kat= atan(tang)*360/2/M_PI;
    return kat;
}

double pochodna(double x1, double y1, double x2, double y2){
    //cout<<x1<<" "<<y1<<" "<<x2<<" "<<y2<<endl;
    double dy=abs(y1-y2);
    double dx=abs(x1-x2);
    if(dy==0)
        return 0;
    if(dx==0)
        return 1000;
    double odp=dy/dx;
    if((y2-y1)*(x2-x1)<0){
        return odp;
    }
    else{
        return -1*odp;
    }
}

bool czy_lezy_na_prostej(int x, int y, double a, double dokladnosc){
    //f(x)=ax
    double oczekiwana_warotsc=0;
    double oczekiwany_x=0;
    if(a==999.99){
        //prosta pionowa
        //nie ma oczekiwanego y dla pewnych x
        if(abs(x)>dokladnosc)
            return false;
        else
            return true;
    }
    else if(a==0){
        //prosta pozioma
        //nie ma oczekiwanego x dla pewnych wartosci
        if(abs(y)>dokladnosc)
            return false;
        else
            return true;
    }
    else{
        oczekiwana_warotsc=a*x;
        oczekiwany_x=(double)y/a;
        if(abs(oczekiwana_warotsc-y)>dokladnosc && abs(oczekiwany_x-x)>dokladnosc){
            //cout<<"funkcja: "<<x<<" "<<y<<" "<<a<<" "<<oczekiwana_warotsc<<" ";
            return false;
        }
        else
            return true;

    }
}

bool czy_prostokat(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4){
    bool odp=false;
    double p1=pochodna(double(x1), double(y1), double(x2), double(y2));
    double p3=pochodna(double(x1), double(y1), double(x3), double(y3));
    double p2=pochodna(double(x2), double(y2), double(x3), double(y3));
    double k1=kat(p1, p2);
    double k3=kat(p1, p3);
    double k2=kat(p2, p3);
    double polowa_przek_x=0;
    double polowa_przek_y=0;
    double wektor_x=0;
    double wektor_y=0;
    double cel_x=0;
    double cel_y=0;
    double bok_a=0;
    double bok_b=0;
    if(k1<80 && k2<80 && k3<80)
        return false;
    if((k1>80 && k2>80) || (k1>80 && k3>80) || (k2>80 && k3>80)){
        return false;
    }
    if(k1>80){
        polowa_przek_x=double(x1+x3)/2;
        polowa_przek_y=double(y1+y3)/2;
        wektor_x=polowa_przek_x-x2;
        wektor_y=polowa_przek_y-y2;
        cel_x=polowa_przek_x+wektor_x;
        cel_y=polowa_przek_y+wektor_y;
        bok_a=sqrt(abs(x3-x2)*abs(x3-x2)+abs(y3-y2)*abs(y3-y2));
        bok_b=sqrt(abs(x1-x2)*abs(x1-x2)+abs(y1-y2)*abs(y1-y2));
    }
    if(k2>80){
        polowa_przek_x=double(x1+x2)/2;
        polowa_przek_y=double(y1+y2)/2;
        wektor_x=polowa_przek_x-x3;
        wektor_y=polowa_przek_y-y3;
        cel_x=polowa_przek_x+wektor_x;
        cel_y=polowa_przek_y+wektor_y;
        bok_a=sqrt(abs(x3-x2)*abs(x3-x2)+abs(y3-y2)*abs(y3-y2));
        bok_b=sqrt(abs(x1-x3)*abs(x1-x3)+abs(y1-y3)*abs(y1-y3));
    }
    if(k3>80){
        polowa_przek_x=double(x2+x3)/2;
        polowa_przek_y=double(y2+y3)/2;
        wektor_x=polowa_przek_x-x1;
        wektor_y=polowa_przek_y-y1;
        cel_x=polowa_przek_x+wektor_x;
        cel_y=polowa_przek_y+wektor_y;
        bok_a=sqrt(abs(x3-x1)*abs(x3-x1)+abs(y3-y1)*abs(y3-y1));
        bok_b=sqrt(abs(x1-x2)*abs(x1-x2)+abs(y1-y2)*abs(y1-y2));
    }
    double dlugosc_przek = sqrt(wektor_x*wektor_x+wektor_y*wektor_y)*2;
    //ponizsej dla sprawdzenia czy jest prostokatem bylo abs<30 i dzialalo dla 122/124, bez r39
    if(abs(x4-cel_x)<20 && abs(y4-cel_y)<20 && dlugosc_przek>150 && bok_a>100 && bok_b>100){
        return true;
    }
    else
        return false;
}

double odleglosc(int x1, int y1, int x2, int y2){
    double odl=sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));
    return odl;
}

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
        string nazwa_fout = "rogi";
        for(int h=5; h<pliki[vv].size(); h++){
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
        int dowolny_x=0;
        int dowolny_y=0;
        int bitmapa[height][width] = {0};
        int znalezionych_rogow =0;
        int obwod=0;

        for(int i=0; i<width*height; i++){
            int a, b, c;
            fin>>a>>b>>c;
            if(a!=255 || b!=255 || c!=255){
                bitmapa[i/width][i%width] = 1;
                obwod++;
                dowolny_x=i%width;
                dowolny_y=i/width;
            }
            else{
                bitmapa[i/width][i%width] = 0;
            }
        }

        int punkty[obwod*2] = {0};
        punkty[0]=dowolny_y;
        punkty[1]=dowolny_x;
        bitmapa[dowolny_y][dowolny_x]=2;
        for(int i=0; i<obwod-1; i++){
            int x=punkty[2*i+1];
            int y=punkty[2*i];
            for(int h=0; h<3; h++){
                for(int u=0; u<3; u++){
                    if(bitmapa[y-1+h][x-1+u]==1){
                        punkty[2+2*i]=y-1+h;
                        punkty[3+2*i]=x-1+u;
                        bitmapa[y-1+h][x-1+u]=2;
                        u=3;
                        h=3;
                    }
                }
            }
        }

        int r=33;   //bylo 20 ok przy dokl=1.5 i zakresie bledu = 5 dla puzzla numer 1
        double pochodne1[obwod] = {0};
        double pochodne2[obwod] = {0};

        int znacznik=2*obwod-2*r;
        for(int i=0; i<obwod; i++){
            pochodne1[i]=pochodna(punkty[2*i+1], punkty[2*i], punkty[(2*r+2*i+1)%(2*obwod)], punkty[(2*r+2*i)%(2*obwod)]);
            pochodne2[i]=pochodna(punkty[2*i+1], punkty[2*i], punkty[(znacznik+1)%(2*obwod)], punkty[(znacznik)%(2*obwod)]);
            znacznik+=2;
            //cout<<pochodne1[i]<<" "<<pochodne2[i]<<endl;
        }

        double dokladnosc=1.0;//zdecydowanie najlepiej 2 ;)
        int zakres_bledu=2;
        int kandydaci[50]={-1};

        bool mamy_czterch=false;
        int proba=0;
        int znalezionych_kand=0;
        while(mamy_czterch==false && znalezionych_kand<20){
            for(int i=0; i<obwod; i++){
                double k = kat(pochodne1[i], pochodne2[i]);
                //cout<<punkty[2*i]<<" "<<punkty[2*i+1]<<" "<<k<<endl;
                if(k>60){
                    int bledow1=0;
                    //czy wszystkie punkty posrodku leza na jednej prostej
                    //pierwsza strona
                    double a=0;
                    if(punkty[(2*(i+r)+1)%(2*obwod)]-punkty[2*i+1]==0){
                        a=999.99;
                    }
                    else{
                        a=(double)(punkty[2*i]-punkty[(2*(i+r))%(2*obwod)])/(punkty[(2*(i+r)+1)%(2*obwod)]-punkty[2*i+1]);   //y/x
                    }
                    for(int p=1; p<r; p++){
                        if(czy_lezy_na_prostej(punkty[(2*(i+p)+1)%(2*obwod)]-punkty[2*i+1], punkty[2*i]-punkty[(2*(i+p))%(2*obwod)], a, dokladnosc)==false){
                            bledow1++;
                            //cout<<j<<" "<<p<<endl;
                            if(czy_lezy_na_prostej(punkty[(2*(i+p)+1)%(2*obwod)]-punkty[2*i+1], punkty[2*i]-punkty[(2*(i+p))%(2*obwod)], a, dokladnosc*2)==false){
                                bledow1+=10;
                            }
                        }
                    }
                    //i druga strona
                    int bledow2=0;
                    double a2=0;
                    if(punkty[(2*obwod+(2*(i-r)+1))%(2*obwod)]-punkty[2*i+1]==0){
                        a2=999.99;
                    }
                    else{
                        a2=(double)(punkty[2*i]-punkty[(2*obwod+(2*(i-r)))%(2*obwod)])/(punkty[(2*obwod+(2*(i-r)+1))%(2*obwod)]-punkty[2*i+1]);   //y/x
                    }
                    for(int p=1; p<r; p++){
                        if(czy_lezy_na_prostej(punkty[(2*obwod+(2*(i-p)+1))%(2*obwod)]-punkty[2*i+1], punkty[2*i]-punkty[(2*obwod+(2*(i-p)))%(2*obwod)], a2, dokladnosc)==false){
                            bledow2++;
                            //cout<<j<<" "<<p<<endl;
                            if(czy_lezy_na_prostej(punkty[(2*obwod+(2*(i-p)+1))%(2*obwod)]-punkty[2*i+1], punkty[2*i]-punkty[(2*obwod+(2*(i-p)))%(2*obwod)], a2, dokladnosc*2)==false){
                                bledow2+=10;
                            }
                        }
                    }

                    if(bledow1<zakres_bledu && bledow2<zakres_bledu){
                        //cout<<i+1<<". "<<punkty[2*i]<<" "<<punkty[2*i+1]<<" "<<proste[i]<<" "<<punkty[(2*(i+j-1))%(2*obwod)]<<" "<<punkty[(2*(i+j-1)+1)%(2*obwod)]<<endl;
                        //cout<<"Kandydat!"<<endl;
                        //cout<<punkty[2*i]<<" "<<punkty[2*i+1]<<" "<<k<<endl;
                        bool nowy=true;
                        for(int z=0; z<znalezionych_kand; z++){
                            if(abs(kandydaci[z]-i)<10 || abs(kandydaci[z]-i)>obwod-10){
                                nowy=false;
                            }
                        }
                        if(nowy){
                            kandydaci[znalezionych_kand]=i;
                            znalezionych_kand++;
                            //cout<<punkty[2*i]<<" "<<punkty[2*i+1]<<" "<<dokladnosc<<" "<<r<<" "<<zakres_bledu<<endl;
                            i+=5;
                        }
                    }
                }
            }
            bool prostokat=false;
            if(znalezionych_kand>=4){
                //czy ze znalezionych kandydatow da sie zlozyc prostokat
                //jesli nie szukamy nowych kandydatow
                for(int q=0; q<znalezionych_kand-3; q++){
                    for(int w=q+1; w<znalezionych_kand-2; w++){
                        for(int e=w+1; e<znalezionych_kand-1; e++){
                            for(int r=e+1; r<znalezionych_kand; r++){
                                prostokat=czy_prostokat(punkty[2*kandydaci[q]+1], punkty[2*kandydaci[q]], punkty[2*kandydaci[w]+1], punkty[2*kandydaci[w]], punkty[2*kandydaci[e]+1], punkty[2*kandydaci[e]], punkty[2*kandydaci[r]+1], punkty[2*kandydaci[r]]);
                                if(prostokat){
                                    int zd =kandydaci[q];
                                    int zc =kandydaci[w];
                                    int zb =kandydaci[e];
                                    int za =kandydaci[r];
                                    kandydaci[0]=zd;
                                    kandydaci[1]=zc;
                                    kandydaci[2]=zb;
                                    kandydaci[3]=za;
                                    q=znalezionych_kand;
                                    w=znalezionych_kand;
                                    e=znalezionych_kand;
                                    r=znalezionych_kand;
                                }
                            }
                        }
                    }
                }
            }
            if(prostokat==false){
                //zmieniamy kryteria: dokladnosc, zakres bledu, ewentualnie r
                if(dokladnosc<5.0 && r>25){
                    dokladnosc+=0.1;
                }
                else{
                    if(zakres_bledu<5){
                        zakres_bledu++;
                    }
                    else{
                        if(r>25){
                            r--;
                        }
                        else{
                           dokladnosc+=0.1;
                        }
                    }
                }
            }
            else{
                mamy_czterch=true;
            }
            proba++;
            //cout<<endl;
        }
        if(mamy_czterch==false){
            cout<<"Problem z kandydatami!!----------------------------"<<endl;
        }
        //cout<<proba<<endl;
        zakres_bledu=3;
        dokladnosc=1.5;
        r=25;

        int rogi[4][2]={0};
        int zakres=7;
        int zasieg=20;
        for(int i=0; i<4; i++){
            int p1=kandydaci[i]-zasieg;
            if(p1<0)
                p1+=obwod;
            int p2=kandydaci[i]-zakres;
            if(p2<0)
                p2+=obwod;
            int p3=kandydaci[i]+zasieg;
            int p4=kandydaci[i]+zakres;
            if(p3>obwod-1)
                p3-=obwod;
            if(p4>obwod-1)
                p4-=obwod;
            //cout<<punkty[2*p1]<<" "<<punkty[2*p1+1]<<endl;
            //cout<<punkty[2*p2]<<" "<<punkty[2*p2+1]<<endl;
            //cout<<punkty[2*p3]<<" "<<punkty[2*p3+1]<<endl;
            //cout<<punkty[2*p4]<<" "<<punkty[2*p4+1]<<endl;
            //cout<<endl<<endl;
            //cout<<kandydaci[i]<<endl;
            double a1=0;
            double a2=0;
            if(punkty[2*p1+1]-punkty[2*p2+1]==0){
                a1=10000;
            }
            else{
                a1=(double)(punkty[2*p1]-punkty[2*p2])/(punkty[2*p1+1]-punkty[2*p2+1]);
            }
            if(punkty[2*p3+1]-punkty[2*p4+1]==0){
                a2=10000;
            }
            else{
                a2=(double)(punkty[2*p3]-punkty[2*p4])/(punkty[2*p3+1]-punkty[2*p4+1]);
            }
            double b1=(double)(punkty[2*p1]-a1*punkty[2*p1+1]);
            double b2=(double)(punkty[2*p3]-a2*punkty[2*p3+1]);
            double xs=(b2-b1)/(a1-a2);
            double ys=a1*xs+b1;
            int x = (int)xs;
            int y = (int)ys;
            //cout<<x<<" "<<y<<endl;
            double min_o=1000;
            int chyba_rog=-1;
            for(int j=0; j<2*zakres+1; j++){
                double o=odleglosc(x, y, punkty[(2*(p2+j)+1)%(2*obwod)], punkty[(2*(p2+j))%(2*obwod)]);
                if(o<min_o){
                    min_o=o;
                    chyba_rog=(p2+j)%obwod;
                }
            }
            rogi[i][0]=punkty[2*chyba_rog];
            rogi[i][1]=punkty[2*chyba_rog+1];
            bitmapa[rogi[i][0]][rogi[i][1]]=9;
            //cout<<rogi[i][0]<<" "<<rogi[i][1]<<endl;
        }

        for(int i=0; i<height; i++){
            for(int j=0; j<width; j++){
                if(bitmapa[i][j]==2){
                    fout<<0<<endl<<0<<endl<<0<<endl;
                }
                else if(bitmapa[i][j]==9){
                    fout<<63<<endl<<72<<endl<<204<<endl;
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

