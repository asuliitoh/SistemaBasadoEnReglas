#include<iostream>
#include"SBR_RazonamientoAtras.h"
#include"LecturaDatos.h"
using namespace std;

int main(int argc, char* argv[]){
    if (argc != 3)cout << "Formato: " << argv[0] << "<archivoBH> <archivoBC>" << endl;
    
    else{
    EncadenamientoHaciaAtras(argv[1],argv[2]);
    }

    return 0;
}