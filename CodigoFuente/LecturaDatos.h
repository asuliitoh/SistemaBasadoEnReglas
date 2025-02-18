
#ifndef LECTURADATOS_H
#define LECTURADATOS_H
#include<string>
#include<vector>
using namespace std;

#define NO_OPERADOR -1
#define OPERADOR_DISYUNCION 0
#define OPERADOR_CONJUNCION 1


struct hecho{
    int id = 0;
    double fc = 0.0;
};

struct BH{
    bool inicializada = false;
    vector<hecho> hechos;
    int objetivo =0;
}; 

struct regla{
    int id;
    vector<int> antecedentes;
    int operador = NO_OPERADOR;
    int consecuente = 0;
    double fc =0.0;

};

struct BC{
    vector<regla> reglas;
    bool inicializada = false;

};

BC LecturaBC(string fichero);
BH LecturaBH(string fichero);




#endif 