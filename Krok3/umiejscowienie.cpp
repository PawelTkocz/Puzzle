#include <bits/stdc++.h>
using namespace std;

int new_x = 0;


double potrzebny_kat(double x1, double y1, double x2, double y2){
    double odleglosc = sqrt((x2-x1)*(x2-x1)+(y1-y2)*(y1-y2));
    double sin_kata = (y1-y2)/odleglosc;
    double jaki_to_kat = asin(sin_kata);
    if(x2>x1){
        jaki_to_kat=M_PI-jaki_to_kat;
    }
    return jaki_to_kat;
}

int y_po_obrocie(double kat_obrotu, double x_pkt_obr, double y_pkt_obr, double x, double y){
    double cos_kat = cos(kat_obrotu);
    double sin_kat = sin(kat_obrotu);
    double new_y = (y-y_pkt_obr)*cos_kat + (x-x_pkt_obr)*sin_kat;
    new_x = round((x-x_pkt_obr)*cos_kat-(y-y_pkt_obr)*sin_kat+x_pkt_obr);
    return round(new_y+y_pkt_obr);
}

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
        for(int h=4; h<pliki[i].size(); h++){
            if(pliki[i][h]=='.'){
                nazwa_fout += "OK.ppm";
                h=pliki[i].size();
            }
            else{
                nazwa_fout += pliki[i][h];
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
                width = b;
            }
            else{
                if(i==1)
                    height=b;
            }
        }
        int rogi[8] = {0};
        int bitmapa[height][width] = {0};
        int znalezionych_rogow =0;

        for(int i=0; i<width*height; i++){
            int a, b, c;
            fin>>a>>b>>c;
            if(a!=0 || b!=0 || c!=0){
                if(a==63 && b==72 && c==204){
                    rogi[2*znalezionych_rogow] = i%width;
                    rogi[2*znalezionych_rogow+1] = i/width;
                    znalezionych_rogow++;
                    bitmapa[i/width][i%width] = 2;
                }
                else{
                    bitmapa[i/width][i%width] = 0;
                }
            }
            else{
                bitmapa[i/width][i%width] = 1;
            }
        }

        int najwyzszy = -1;
        int najnizszy = 0;
        int naj_lewo = width;
        int naj_prawo = 0;
        for(int e=0; e<height; e++){
            for(int q=0; q<width; q++){
                if(bitmapa[e][q]==1 || bitmapa[e][q]==2){
                    if(najwyzszy==-1){
                        najwyzszy=e;
                    }
                    if(najnizszy<e){
                        najnizszy=e;
                    }
                    if(naj_lewo>q){
                        naj_lewo=q;
                    }
                    if(naj_prawo<q){
                        naj_prawo=q;
                    }
                }
            }
        }
        int najw_p=najwyzszy;
        int najn_p=najnizszy;
        int naj_p_p=naj_prawo;
        int naj_l_p=naj_lewo;
        //cout<<najw_p<<" "<<najn_p<<" "<<naj_p_p<<" "<<naj_l_p<<endl;

        for(int ax=0; ax<4; ax++){
            int r1=(ax/2)*3;
            int r2=(ax%2)+1;
            int punkt_x = rogi[2*r1];
            int punkt_y = rogi[2*r1+1];
            int punkt2_x = rogi[2*r2];
            int punkt2_y = rogi[2*r2+1];

            if(punkt2_y>punkt_y){
                int pomx = punkt_x;
                int pomy = punkt_y;
                punkt_y = punkt2_y;
                punkt_x = punkt2_x;
                punkt2_y = pomy;
                punkt2_x = pomx;
            }
            double kat_obrotu = potrzebny_kat(punkt_x, punkt_y, punkt2_x, punkt2_y);
            for(int i=0; i<height; i++){
                for(int j=0; j<width; j++){
                    if(bitmapa[i][j]==1 || bitmapa[i][j]==2){
                        int new_y = y_po_obrocie(kat_obrotu, double(punkt_x), double(height-punkt_y), double(j), double(height-i));
                        //mapa_obrot1[height-new_y][new_x] = 1;
                        if(height-new_y<najwyzszy){
                            najwyzszy = height-new_y;
                        }
                        if(height-new_y>najnizszy){
                            najnizszy = height-new_y;
                        }
                        if(new_x>naj_prawo){
                            naj_prawo=new_x;
                        }
                        if(new_x<naj_lewo){
                            naj_lewo=new_x;
                        }
                    }
                }
            }
        }
        //cout<<najwyzszy<<" "<<najnizszy<<" "<<naj_prawo<<" "<<naj_lewo<<endl;
        long long rozmiar=(najnizszy-najwyzszy+3)*(naj_prawo-naj_lewo+3);
        if(rozmiar>240000){
            cout<<"Problem z plikiem "<<pliki[i]<<" -rozmiar: "<<rozmiar<<endl;
        }
        else{
            fout<<naj_prawo-naj_lewo+3<<" "<<najnizszy-najwyzszy+3<<endl<<255<<endl;
            //cout<<"Rozmiar: "<<rozmiar<<endl;
            int mapa2[najnizszy-najwyzszy+3][naj_prawo-naj_lewo+3];
            for(int i=0; i<najnizszy-najwyzszy+3; i++){
                for(int j=0; j<naj_prawo-naj_lewo+3; j++){
                    mapa2[i][j]=0;
                }
            }
            for(int i=0; i<najn_p-najw_p+1; i++){
                for(int j=0; j<naj_p_p-naj_l_p+1; j++){
                    mapa2[najw_p-najwyzszy+i+1][naj_l_p-naj_lewo+j+1]=bitmapa[najw_p+i][naj_l_p+j];
                }
            }
            for(int i=0; i<najnizszy-najwyzszy+3; i++){
                for(int j=0; j<naj_prawo-naj_lewo+3; j++){
                    if(mapa2[i][j]==2){
                        fout<<63<<endl<<72<<endl<<204<<endl;
                    }
                    if(mapa2[i][j]==1){
                        fout<<0<<endl<<0<<endl<<0<<endl;
                    }
                    if(mapa2[i][j]==0){
                        fout<<255<<endl<<255<<endl<<255<<endl;
                    }
                }
            }
            cout<<pliki[i]<<" gotowy"<<endl;
        }

        fin.close();
        fout.close();
    }

    return 0;
}


