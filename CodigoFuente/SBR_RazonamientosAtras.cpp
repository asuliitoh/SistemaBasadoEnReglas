#include<iostream>
#include"LecturaDatos.h"
#include"SBR_RazonamientoAtras.h"
#include<cmath>
#include<algorithm>
#include<fstream>


using namespace std;

//Funcion para obtener nombre del fichero sin extensiones ni rutas
string obtenerNombreFichero(string nombre) {
	unsigned int pos1 = (int) nombre.find_last_of("\\");

	string aux;
	if (pos1 != string::npos)
		aux = nombre.substr(pos1 + 1);

	else aux = nombre;

	unsigned int pos2 = (int) aux.find_last_of(".");
	if (pos1 != string::npos)
		aux = aux.substr(0, pos2);

	return aux;
}


//Funcion utilizada para comparar dos reglas. Se da prioridad a aquellas con conjunciones.
bool compararReglas(const regla& a, const regla& b) {
    if (a.operador == OPERADOR_CONJUNCION && b.operador != OPERADOR_CONJUNCION) 
        return false;
    return true;
}


//Funcion auxiliar utilizada en la que se busca obtener el conjunto de reglas que se pueden aplicar
//Segun la meta que se quiera alcanzar. Devuelve un vector de reglas con las reglas que
//tienen como consecuente dicha meta.
vector<regla> Equiparar(int meta, BC& bc){
    vector<regla> cc;



    for (const regla& r : bc.reglas) {
        if (r.consecuente == meta) {
            cc.push_back(r);
        }
    }

    //Se ordena con el fin de dar prioridad a aquellas reglas con operadores logicos y.
    sort(cc.begin(),cc.end(), compararReglas);

    //Se devuelve el conjunto conflicto.
    return cc;
}

//Funcion auxiliar utilizada para obtener el factor de destreza
//de un hecho que ya se encuentra en la base de hechos.
// Devuelve un valor tipo double.
double hallarFactor(int id, BH& bh) {

    double fc_hecho = 0.0;

    for (const hecho& h : bh.hechos) {
        if (h.id == id) {
            fc_hecho = h.fc;
            break;
        }
    } 
    //Devuelto el factor de certeza del hecho 
    return fc_hecho;
}

//Funcion auxiliar utilizada para calcular el factor de certeza resultante
//de una combinacion de antecedentes.
// Devuelve un valor tipo double.
double calcularCaso1(double fc1, double fc2, int operador){

    if (operador == NO_OPERADOR) fc1 = fc2;
        else if (operador == OPERADOR_DISYUNCION)fc1 = max(fc1, fc2);
        else fc1 = min(fc1, fc2);

    //Devuelto el factor de certeza final.
    return fc1;
}


//Funcion auxiliar utilizada para calcular el factor de certeza resultante
//de la combinacion de evidencias sobre un mismo hecho o hipotesis.
//Devuelve un double
double calcularCaso2(vector<double> factores, ofstream& logfile){

    //Inicializacion
    double fc1 = factores.back();
    factores.pop_back();
    double fc2 = 0.0;

    //Bucle while hasta que solo quede el factor de certeza resultante
    while(!factores.empty()){

        //Se escoge el segundo factor de certeza
        fc2 = factores.back();
        factores.pop_back();

  

        //Calculo del factor final, guardado como el primer factor de certeza.
        logfile << "Caso 2 entre FC=" << fc1 << " y FC=" << fc2 << " aplicado, FC="; 
        if ((fc1 >=.00) && (fc2 >= 0.0)) fc1 = fc1 + fc2*(1-fc1);
        else if ((fc1 <=0.0) && (fc2 <= 0.0)) fc1 = fc1+fc2*(1+fc1);
        else fc1 = (fc1+fc2)/(1-min(fabs(fc1), fabs(fc2)));
        logfile << fc1 << endl;

    }
    //Devuelto el factor de certeza final
    return fc1;
    
}

//Funcion auxiliar para la combinacion de dos reglas o 
//encadenamiento de evidencia. Se devuelve un double.
double calcularCaso3(double regla_fc, double fc_antecedentes){
      return regla_fc * max(0.0, fc_antecedentes);
}



//Funcion que contiene toda la logica del motor de inferencia.
//Devuelve verdadero si el objetivo se logra verificar o inferir
//Devuelve falso si no se encuentra nignuna regla aplicable.
bool verificar(int meta, BH& bh, BC& bc, ofstream& logfile){

    //Si la meta se encuentra ya en la base de hechos se devuelve true
    for (hecho hecho : bh.hechos)
    {
       if (hecho.id == meta) {

        return true;
          
       }
    }
    
    //Equiparar los consecuentes de la base de conocimientos con la meta
    vector<regla> cc = Equiparar(meta, bc);

    //Variable en el que se almacenan los factores de las reglas que tienen un mismo consecuente, con el 
    //fin de aplicar posteriormente el caso 2
    vector<double> fc_reglas;

    
    //Bucle while hasta que el conjunto conflicto quede vacio
    while (!cc.empty()){
        //Inicializacion
        double fc_antecedentes;
        regla regla = cc.back();
        cc.pop_back();


         //Condicional utilizado para imprimir la regla actual escogida en el fichero 
         //que almacena el proceso de inferencias
         if (regla.antecedentes.size() > 1){
            string operadorCaracter;
            if (regla.operador == OPERADOR_CONJUNCION) operadorCaracter = " y ";
            else if (regla.operador == OPERADOR_DISYUNCION) operadorCaracter =" o ";
            logfile << "R" << regla.id << ": Si h" << regla.antecedentes[0] << operadorCaracter << "h" << regla.antecedentes[1] << " Entonces h" << regla.consecuente << ", FC=" << regla.fc << endl;
        }

         else  logfile << "R" << regla.id << ": Si h" << regla.antecedentes[0] << " Entonces h" << regla.consecuente << ", FC=" << regla.fc << endl;


        //Vector que almacena los identificadores de los antecedentes
        //De la regla actual escogida
        vector<int> nuevasMetas(regla.antecedentes.begin(), regla.antecedentes.end());
        
        //operador almacena el operador de la regla actual
        int operador = regla.operador;

        //Condicional utilizado para inicializar fc_antecedentes
        //fc_antecedentes almacena el factor de certeza total de los antecedentes
        if (operador == OPERADOR_CONJUNCION) fc_antecedentes = 1.0;
        else fc_antecedentes = 0.0;


        //Bucle while hasta que no queden mas antecedentes
        while (!nuevasMetas.empty()){

            //Se escoge el antecedente que será la meta a partir de ahora 
            int meta_actual = nuevasMetas.back();
            nuevasMetas.pop_back();

            //Hallar factor de destreza del antecedente actual escogido
            if(verificar(meta_actual, bh, bc, logfile)){

                //Se calcula el caso 1 (si es necesario)
                double fc_hecho = hallarFactor(meta_actual,bh);
                fc_antecedentes = calcularCaso1(fc_antecedentes, fc_hecho, operador);
                logfile << "\t(R" << regla.id <<  "):Antecedente h" << meta_actual << " verificado" << endl;

                //Si esta vacio, no quedan mas antecedentes por verificar y por tanto se puede aplicar el caso 3.
                if(nuevasMetas.empty()){
                    logfile << "\t(R" << regla.id << "):Caso 1 aplicado, FC = " << fc_antecedentes << endl;
                    fc_antecedentes = calcularCaso3(regla.fc, fc_antecedentes);
                    logfile << "\t(R" << regla.id <<  "):Caso 3 aplicado, FC = " << fc_antecedentes << endl;
                    
                    //Se introduce a fc_reglas, quiere decir que se ha aplicado una regla
                    fc_reglas.push_back(fc_antecedentes);
                    }                    
            }
        }

        //Si hay al menos una regla aplicada
        if(!(fc_reglas.empty()) && (cc.empty())){
            
            double fcfinal = 0.0;
            
            //Si se ha aplicado mas de una regla, se requiere calcular el caso 2
            if(fc_reglas.size() > 1) fcfinal = calcularCaso2(fc_reglas, logfile);
            else fcfinal = fc_reglas.back();
            
            //Se inicializa el hecho objetivo a introducir y se introduce a la base de hechos
            hecho hechoAIntroducir;
            hechoAIntroducir.fc = fcfinal;
            hechoAIntroducir.id = meta;
            bh.hechos.push_back(hechoAIntroducir);
            
          
           
        } 

    

    }

    return true;

}

//Funcion principal
void EncadenamientoHaciaAtras(string hechos, string conocimientos){
    
    //Se leen la base de hechos y la de conocimientos
    BH bh = LecturaBH(hechos);
    BC bc = LecturaBC(conocimientos);


    //Si la lectura ha sido valida se prepara el fichero en el que se 
    //va a escribir el proceso de inferencia y se aplica el motor de inferencia
    if((bh.inicializada) && (bc.inicializada)){

        string ficherosalida=obtenerNombreFichero(hechos)+"_"+obtenerNombreFichero(conocimientos)+".txt";
        
        ofstream logfile(ficherosalida);

        //Se aplica el motor de inferencia y se imrpimen los resultados
        logfile << endl;
        bool resultado = verificar(bh.objetivo, bh, bc, logfile);
        double fc = hallarFactor(bh.objetivo,bh);

        if (resultado) logfile << "\nObjetivo h" << bh.objetivo << ", FC=" << fc << endl;
        else logfile << "\nObjetivo no se ha podido verificar." << endl;

    }

}
