# Sistema Basado en Reglas (SBR)

## Descripción del Proyecto

Este sistema implementa un motor de inferencia basado en reglas con soporte para factores de certeza (FC). Utiliza ficheros de entrada (bases de hechos y conocimientos) y genera un archivo de salida que muestra el proceso de inferencia paso a paso.

## Estructura del Proyecto

- **BC/**: Carpeta que contiene las bases de conocimiento utilizadas para las pruebas.
- **BH/**: Carpeta que contiene las bases de hechos utilizadas para las pruebas.
- **CodigoFuente/**: Contiene el código fuente utilizado en el programa.
  - **LecturaDeDatos**: Módulo que define los tipos necesarios, así como las reglas, hechos, BH y BC. Su objetivo es leer los archivos para obtener las bases de datos y conocimientos.
  - **SBR_RazonamientosAtras**: Módulo que define toda la lógica relacionada con el Sistema Basado en Reglas con FC.
  - **main**: Función principal del programa que llama al sistema basado en reglas.
- **Documentacion/**: Incluye el informe de la práctica.
- **Salidas/**: Contiene las salidas generadas en cada prueba.
- **SBR_FC.exe**: Archivo ejecutable del programa.

## Requisitos

- **Sistema operativo**: Windows o compatible.
- **Compilador**: g++ (mínimo versión 4.7).

## Compilación del Ejecutable

Para la compilación del ejecutable del Sistema Basado en Reglas (SBR), se ha utilizado el compilador **g++**. La versión mínima requerida para la correcta compilación de este programa es la que soporte la versión C++ 11. 

Para la correcta compilación, se debe ejecutar el siguiente comando:

```bash
g++ -Wall -Wextra -g3 main.cpp .\CodigoFuente\LecturaDatos.cpp .\CodigoFuente\SBR_RazonamientosAtras.cpp -o .\SBR_FC.exe
```

## Ejecución del ejecutable
 Su ejecución es muy sencilla, simplemente hay que ejecutar en el terminal el siguiente comando:
 ```bash
 .\SBR_FC.exe archivo_bh.txt > <archivo_bc.txt>
  ```
 
 ### Formato del fichero de base de conocimientos
 Se aconseja que el fichero tenga una extensión .txt para evitar errores a la hora de abrir el fichero. El
 contenido de este fichero debe seguir el mismo formato:

 ```plaintext
 n
 Ri: Si hj Entonces hp, FC=fc_regla
 Ri: Si hj o/y hk Entonces hp, FC=fc_regla
 Ri: Si hj o/y hk Entonces hp, FC=fc_regla
 …(nveces)
 Ri: Si hj o/y hk Entonces hp, FC=fc_regla
```
Donde:
- **n** es el número de reglas definidas.
- **i**, **j**, **k**, y **p** son números enteros positivos mayores que 0.
- **fc_regla** es el factor de certeza asociado a la regla.
  
**Nota:** No todas las reglas tienen que tener más de un antecedente.

#### Ejemplo
```plaintext
 4
 R1: Si h2 o h3 Entonces h1, FC=0.5
 R2: Si h4 Entonces h1, FC=1
 R3: Si h5 y h6 Entonces h3, FC=0.7
 R4: Si h7 Entonces h3, FC=-0.5
```

### Formato del fichero de base de hechos
 Se aconseja que el fichero tenga una extensión .txt para evitar errores a la hora de abrir el fichero. El contenido de este fichero debe seguir el mismo formato:
```plaintext
 m
 hj, FC=fc_hecho
 …(mveces)
 hj, FC=fc_hecho
 Objetivo
 hj
```
 Donde:
- **m** es el número de hechos iniciales definidos.
- **j** es un número entero positivo mayor que 0.
- **fc_hecho** es el factor de certeza asociado al hecho.
  
**Nota:** El factor de certeza **fc_hecho** está asociado a cada hecho individualmente.

 
 #### Ejemplo
 ```plaintext
 5
 h2, FC=0.3
 h4, FC=0.6
 h5, FC=0.6
 h6, FC=0.9
 h7, FC=0.5
 Objetivo
 h1
```

### Formato del Fichero de Salida
 El ejecutable una vez termina su ejecución genera un fichero de salida cuyo nombre es la concatenación del nombre del fichero de la base de hechos y del fichero de la base de conocimientos. Su extensión es **.txt** y su formato es el siguiente:

```plaintext
 Ri: Si hj Entonces hp, FC=fc_regla
 (Ri) Antecedente hj verificado
 (Ri) Caso 3 aplicado, FC = fc_hp_reglai
 Ri: Si hj o/y hk Entonces hp, FC=fc_regla
 (Ri) Antecedente hj verificado
 (Ri) Antecedente hk verificado
 (Ri) Caso 1 aplicado, FC= fc_antecedentes
 (Ri) Caso 3 aplicado, FC = fc_hp_reglai
 …
 Caso 2: FC=fc_hp_reglai y FC=fp_hp_reglai aplicado, FC=fc_hp_final
 Objetivo hp, FC=fc_hp_final
```

 Donde:
- **i**, **j**, **k**, **p**: Son números enteros positivos mayores que cero.
- **fc_regla**: El factor de certeza asociado a la regla.
- **fc_antecedentes**: El factor de certeza asociado a los antecedentes de la regla.
- **fc_hp_regla**: El factor de certeza asociado a la regla **i** aplicada.
- **fc_hp_final**: El factor de certeza total del consecuente **hp**, en caso de que se hayan aplicado varias reglas que infieran sobre este.
**Nota:** El factor de certeza total del consecuente **fc_hp_final** se calcula considerando la combinación de los factores de certeza de todas las reglas aplicadas.

 #### Ejemplo
 ```plaintext
 R1: Si h2 o h3 Entonces h1, FC=0.5
 R3: Si h5 y h6 Entonces h3, FC=0.7
 (R3):Antecedente h6 verificado
 (R3):Antecedente h5 verificado
 (R3):Caso 1 aplicado, FC = 0.6
 (R3):Caso 3 aplicado, FC = 0.42
 R4: Si h7 Entonces h3, FC=-0.5
 (R4):Antecedente h7 verificado
 (R4):Caso 1 aplicado, FC = 0.5
 (R4):Caso 3 aplicado, FC =-0.25
 Caso 2 entre FC=-0.25 y FC=0.42 aplicado, FC=0.226667
 (R1):Antecedente h3 verificado
 (R1):Antecedente h2 verificado
 (R1):Caso 1 aplicado, FC = 0.3
 (R1):Caso 3 aplicado, FC = 0.15
 R2: Si h4 Entonces h1, FC=1
 (R2):Antecedente h4 verificado
 (R2):Caso 1 aplicado, FC = 0.6
 (R2):Caso 3 aplicado, FC = 0.6
 Caso 2 entre FC=0.6 y FC=0.15 aplicado, FC=0.66
 Objetivo h1, FC=0.66
```
