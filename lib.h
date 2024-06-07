#pragma once
#include <cstdint>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

struct Sensor{
    uint8_t TYPE;      
};

struct Actuator{
    uint8_t TYPE;     
};

struct Device_Type{
    uint8_t CPU_OR_CONCENTRATOR;      
    struct Sensor SENSOR;
    struct Actuator ACTUATOR;
};

struct networkDevices{
    uint16_t ID;         
    uint16_t *LLD_ID;   
    uint16_t LLD_COUNT; 
    struct Device_Type DT;
    uint8_t DeviceType; //voy a sumar el Nibble High y el Nibble LOW y almacenarlos juntos para saber el tipo de equipo y no andar con dos variables para leer por separado
    uint16_t ULD_ID;    
};

typedef struct networkDevices ND; //ND de network devices

/**
 * @brief Retorna la totalidad de dispositivos conctados en la red.
 * 
 * Esta función toma como parametro el manipulador del archivo, con el primer header 
 * de uint16_t, obtengo el Lower Level Device Count, salto los otros dos uint16_t del 
 * header que no me interesan por el momento, salto los Lower Level Device ID
 * con un fseek() y cuento con la variable countHeaders la cantiadad total de dispositivos.
 * 
 * @param f  Manipulador del archivo archivo network_structure.dat.
 * @return countHeader,  El número total de dispositivos conectados en la red.
 */
int Count_Devices(FILE *f);

/**
 * @brief LLeno la estructura networkDevices con los datos extraidos del archivo.
 * 
 * Esta función lee los datos del archivo y los carga en la estructura donde corresponde para su posterior lectura.
 * Tambien asigna memoria dinámicamente para los IDs de los dispositivos de nivel inferior (LLD_ID).
 *@param f  Manipulador del archivo archivo network_structure.dat.
 *@param netDev Puntero a los registros cargados en la estructura networkDevices.
 */
void load_network_Devices (FILE *f, ND *netDev);

/**
 * @brief Realiza una búsqueda de un ID en cada uno de los registros en la estructura.
 * Esta funcion buclea con un For() y recorre toda la estructura hasta encontrar la ID.
 * En el caso de no encontrar la ID retorna un -1.
 * @param f  Manipulador del archivo archivo network_structure.dat.
 * @param netDev Puntero a los registros cargados en la estructura networkDevices.
 * @param ID ID que el usuario desea buscar.
 * @return int, retorna La posición del ID en la que se encuentra en la estructura o -1 si no se encuentra.
 */
int busqueda_lineal(FILE *f, ND *netDev, uint16_t ID);

/**
 * @brief Verifica si el ID ingresado esta en la estructura ya cargada.
 * 
 * Esta funcion hace uso de la busqueda lineal para saber si el ID ingresado por l usuario
 * esta presente, en el caso de que no este, el bucle Do va a hacer que el usuario pueda ejecutar la tarea
 * una vez que ingrese un ID valido.
 * 
 * @param f  Manipulador del archivo archivo network_structure.dat.
 * @param netDev Puntero a los registros cargados en la estructura networkDevices.
 * @return uint16_t, El ID que se verifico que esta en la estructura.
 */
uint16_t validate_ID(FILE *f, ND *netDev);

/**
 * @brief Encuentra la secuencia de conexiones de un ID ingresado por el usuario.
 * 
 * Esta funcion, va a buscar ID's que esten conectados en secuencia hasta que el upper level device ID de
 * un equipo sea el 1023, el ID invalido.
 * 
 * @param f  Manipulador del archivo archivo network_structure.dat.
 * @param netDev Puntero a los registros cargados en la estructura networkDevices.
 */
void ID_Connection_Sequence(FILE *f, ND *netDev);

/**
 * @brief Muestra la contidad de dispositivos conectados en la red.
 *
 * Esta funcion muestra en la pantalla la cantiadad total de dispositivos conectados en la red.
 *
 * @param f  Manipulador del archivo archivo network_structure.dat.
 * @param netDev Puntero a los registros cargados en la estructura networkDevices.
 */
void printDeviceCount(FILE *f, ND *netDev);

/**
 * @brief Es un menú para que el usuario interactue y busque lo que desea ver.
 * 
 * Con este procedimiento el usuairo puede elegir entre buscar una secuencia de IDs o 
 * saber la cantidad de dispositivos de una misma especie conectados en la red.
 * 
 * @param f  Manipulador del archivo archivo network_structure.dat.
 * @param netDev Puntero a los registros cargados en la estructura networkDevices.
 */
void menu(FILE *f, ND *netDev);