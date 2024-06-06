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

int Count_Devices(FILE *f);

void load_network_Devices (FILE *f, ND *netDev);

int busqueda_lineal(FILE *f, ND *netDev, uint16_t ID);

uint16_t validate_ID(FILE *f, ND *netDev);

void ID_Connection_Sequence(FILE *f, ND *netDev);