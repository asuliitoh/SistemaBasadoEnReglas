#include"LecturaDatos.h"
#include<iostream>
#include<fstream>
#include<string>
#include<fstream>
#include<regex>
#include <vector>
using namespace std;



bool obtenerLinea(string& linea, ifstream &archivo){

    if(!getline(archivo,linea)){
        cout << "ERROR: No se ha podido leer el archivo" << endl;
        return false;
    }

    else return true;


}
//Funcion que recibe un string que hace referencia al fichero que contiene la base de hechos y devuelve un objeto base de hechos
BH LecturaBH(string fichero){

    //Inicializacion
    bool resultado = true; //Usado para comprobar que la lectura está siendo correca
    regex verificacion("h([0-9]+)\\s*,\\s*FC\\s*=\\s*((-){0,1}(1|(0.[0-9]{1,10})))"); //Expresion Regular para leer la linea actual
    regex identificadorObjetivo("h([0-9]+)"); //Expresion regular usada para leer el identificador del objetivo
    smatch resultados; // smatch es igual a match_results<string::const_iterator>
    BH bh; //Base de Hechos a devolver
    ifstream archivo(fichero); //Variable con la que se accede al archivo

    //Comprobacion de si se ha abierto el archivo mal
    if (!archivo.is_open()){
        cout << "Error, no se ha encontrado el archivo que contiene la base de hechos." << endl;
        return bh;
    }

    //En el caso de que se haya abierto bien el archivo:
    else{

        int size = 0; //Numero de hechos que se han declarado
        string linea; //Utilizada para guardar la linea actual leida
        
        //La primera linea del archivo debe contener el numero de hechos
             if (!obtenerLinea(linea,archivo)) resultado = false;
             else{
             size = stoi(linea);
             bh.hechos = vector<hecho>(size);
             }


        //Lectura de los hechos. Ocurre un error en la lectura si:
        //-Ha introducido menos hechos que los especificados. En este caso, da un error en obtenerLinea.
        //-Ha introducido mas hechos de los especificados. En este caso, sale del bucle y a la hora de comprobar el Objetivo, da error.
        //-Formato de un hecho invalido.
        //-Error a la hora de leer una linea-
        for(int i=0; resultado && (i < size); i++){
            
            //Si ocurre un fallo al leer la linea, salimos del for
           if (!obtenerLinea(linea,archivo)) {resultado = false; continue;};
            

            //Si la linea actual coincide con el formato esperado, se continua
            if (regex_match(linea,verificacion)) { //La linea actual cumple con el formato
                regex_search(linea,resultados,verificacion);
                bh.hechos[i].id = stoi(resultados[1]);
                bh.hechos[i].fc = stof(resultados[2]);
              }

            //linea actual no sigue el formato esperado
            else{
                resultado = false;
            }
        }

        //Lectura del objetivo
        //Si se han leido los hechos, no ocurre un error al leer la linea y esta linea es "Objetivo", se lee la siguiente linea
        if(resultado && obtenerLinea(linea,archivo) && linea.compare("Objetivo") == 0){
            //Si se ha introducido un hecho con el formato valido, se guarda y la base de hechos se considera inicializada.
            if( obtenerLinea(linea,archivo) && regex_match(linea,resultados,identificadorObjetivo)){
                regex_search(linea,resultados,identificadorObjetivo);
                bh.objetivo = stoi(resultados[1]);
                bh.inicializada = true;
                }

            //Si se incumple el formato u ocurre un fallor al leer la linea actual
            else resultado=false;

            }


        //En otro caso se devuelve falso
        else resultado = false;


        //Se imprime en caso de que en algun punto haya habido un error mencionado
        if(!resultado) {
            cout << "ERROR: Formato de Archivo Invalido. Lea README para conocer el formato del Archivo BH." << endl;
           
        }

       //Se cierra el archivo
       archivo.close();
       //Devuelve la base de hechos. Si inicializada = true, los datos se consideran válidos
       return bh;
    
    }

}

BC LecturaBC(string fichero){

    //Inicializacion
    BC bc; //Base de Conocimiento a devolver
    bool resultado=true; //Variable utilizada para comprobar que la lectura vaya bien
    //Expresion regular para leer la regla actual

    regex verificacion("(R([0-9]+))\\s*:\\s*Si\\s+(h([0-9]+))((\\s+(o|y)\\s+(h([0-9]+))){0,1})\\s+Entonces\\s+(h([0-9]+))\\s*,\\s*FC\\s*=\\s*((-{0,1})(1|(0.[0-9]{1,10})))");
    
    //Utilizado posteriormente para leer la conjuncion o disyuncion de hechos
    smatch resultados; // smatch es igual a match_results<string::const_iterator>
    ifstream archivo(fichero); //Variable utilizda para leer el fichero


    //Comprobamos que el archivo se ha abierto mal. En este caso la función retorna.
    if(! archivo.is_open()){
         cout << "Error, no se ha encontrado el archivo que contiene la base de conocimientos." << endl;
        return bc;
    }

    //Si se ha abierto el archivo bien, se continua
    else{
       
        int size = 0; //Numero de reglas que se tienen
        string linea; //Variable utilizada para guardar la linea actual leida
        
        //Se supone que la primera linea contiene el numero de hechos
        if (!obtenerLinea(linea,archivo)) resultado = false;
        else{
            size = stoi(linea);
            bc.reglas = vector<regla>(size);
            }

        //Lectura de los hechos. Ocurre un fallo si:
        //-Se introduen mas o menos hechos que los especificados
        //-Ocurre un fallo al leer la linea del archivo
        //-Formato invalido de la regla
        for(int i=0; resultado && (i < size); i++){
            
            //Si ocurre un fallo al leer la linea, no se continua
           if (!obtenerLinea(linea,archivo)) {resultado = false;continue;};
             //Si la linea actual sigue el formato de la regla especificada, se obtienen el identificador de la regla, antecedentes y consecuente
             if (regex_match(linea,verificacion)) {
                //Identificador de la regla
                regex_search(linea,resultados,verificacion);
                bc.reglas[i].id = stoi(resultados[2]); 
                
                //Primer Antecedente de la regla
                bc.reglas[i].antecedentes.push_back(stoi(resultados[4]));
                // Patrón para capturar cada antecedente
                // Subcadena con las condiciones adicionales
                string antecedentes = resultados[5].str();
                
                //Patron para coger unicamente los antecedentes adicionales
                regex patron_condiciones("\\s+(o|y)\\s+(h([0-9]+))");
                smatch cadena; //Aqui se guardan las coincidencias encontradas que cumplan el anterior patron
                
                
                    if (regex_search(linea, cadena, patron_condiciones)){

                        //Se actualiza el operador
                        if (cadena[1] == "o") bc.reglas[i].operador = OPERADOR_DISYUNCION; //si es una disyunción es 0
                        else if (cadena[1] == "y") bc.reglas[i].operador = OPERADOR_CONJUNCION; 
                        
                        //Se introduce el identificador del antecedente
                        bc.reglas[i].antecedentes.push_back(stoi(cadena[3])); 
                    }

                //Capturamos el consecuente y el FC de la regla
                bc.reglas[i].consecuente = stoi(resultados[11]);
                bc.reglas[i].fc = stof(resultados[12]);
            }   

            else resultado = false; //Inicializada igual a false

        }

        if (resultado) bc.inicializada = true;
        else cout << "ERROR: Formato de Archivo Invalido. Lea README para conocer el formato del Archivo BC." << endl;

        //Si bc.inicializada el contenido de los datos es valido
        return bc;



}

}

