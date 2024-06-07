#include "lib.h"

int Count_Devices(FILE *f){
    int countHeaders = 0;
    uint16_t Header;
    uint16_t LLDC;
    uint16_t aux;
    while (fread(&Header, sizeof(uint16_t), 1, f)){
        aux = (Header << 10) >> 10;
        LLDC = aux & 0x003F;
        fseek(f, 2 * sizeof(uint16_t), SEEK_CUR);
        fseek(f, LLDC * sizeof(uint16_t), SEEK_CUR);
        countHeaders++;        
    }
    rewind(f);
    return countHeaders;
}

void load_network_Devices (FILE *f, ND *netDev){
    uint16_t Header;
    uint16_t ID;
    uint8_t LLDC;
    uint8_t DTNL;
    uint8_t INFO;
    uint8_t DTNH;
    uint8_t DeviceType; 
    uint16_t ULDID; //upper level device ID
    int Pos = 0; //posicion de la estructura
    while (fread(&Header, sizeof(uint16_t), 1, f)){
        ID = (Header>>6) & 0x3FF;
        netDev[Pos].ID = ID;
        LLDC = ((Header << 10) >> 10) & 0x3F;
        netDev[Pos].LLD_COUNT = LLDC;
        netDev[Pos].LLD_ID = new uint16_t[LLDC]; //con el lower level device count asigno memmoria a mi vector de ID para los disp. inferiores en mi estructura
        fread(&Header, sizeof(uint16_t), 1, f);
        DTNH = (Header>>11)& 0x03;
        DTNL = (Header>>3)& 0x01;
        DeviceType = DTNH + DTNL;
        INFO = (Header>>4) & 0XFF;
        netDev[Pos].DeviceType = DeviceType;
        if(DeviceType == 0 || DeviceType == 3){
            netDev[Pos].DT.CPU_OR_CONCENTRATOR = DeviceType;
        } else if(DeviceType==1){ // Si Device Typer es un 1 entonces es un sensor
            netDev[Pos].DT.SENSOR.TYPE = INFO >> 4;
        }else if(DeviceType == 2){ 
            netDev[Pos].DT.ACTUATOR.TYPE = (INFO >> 2) & 0x01;
        }
        fread(&Header, sizeof(uint16_t), 1, f);
        ULDID = (Header>>3) & 0x03FF;
        netDev[Pos].ULD_ID = ULDID;
        for (int i = 0; i < LLDC; i++){
            fread(&netDev[Pos].LLD_ID[i], sizeof(uint16_t), 1, f);    
        }
        Pos++;
    }
    rewind(f);
}

int busqueda_lineal(FILE *f, ND *netDev, uint16_t ID) { //Me aseguro que la ID ingresada por el usuario exista, en el caso que no este en la Red se retorna un -1
    int sizeStruct = Count_Devices(f);
    for (int i = 0; i < sizeStruct; ++i) {
        if (netDev[i].ID == ID) {
            return i;  
        }
    }
    return -1;  
}

uint16_t validate_ID(FILE *f, ND *netDev) {   
    int value;
    uint16_t ID;
    int sizeStruct = Count_Devices(f);
    do {
        printf("\n Enter ID: ");
        scanf("%d", &ID);
        value = busqueda_lineal(f, netDev, ID);
        if (value == -1) {
            printf("\n Enter a valid ID\n");
        }
    } while (value == -1);
    return ID;
}

void ID_Connection_Sequence(FILE *f, ND *netDev) { 
    int sizeStruct = Count_Devices(f); 
    uint16_t ID = validate_ID(f, netDev);  // llamo al procedimiento para verificar si el ID ingresado es correcto
    int Pos = busqueda_lineal(f, netDev, ID); //extraigo la posicion del ID el cual sé que es correcto para comenzar a operar
    int vSize = 0;
    
    while (netDev[Pos].ULD_ID != 1023) {  
        vSize++;                     
        Pos = busqueda_lineal(f, netDev, netDev[Pos].ULD_ID); 
        if (Pos == -1) {
            break; 
        }
    }
    int *v = new int[vSize + 1]; //más uno para agregar el id superior
    Pos = busqueda_lineal(f, netDev, ID);
    for (int i = 0; i <= vSize; i++) {
        v[i] = netDev[Pos].ID;
        Pos = busqueda_lineal(f, netDev, netDev[Pos].ULD_ID);
    }
    printf("\n");

    for (int i = vSize; i >= 0; i--) {// como no ordne el vector muestro desde el ID 1 hasta el seleccionado por el usuario
        printf("ID %d", v[i]);
        if (i > 0) {
            printf(" -> ");
        }
    }
    printf("\n");
    delete[] v; 
}

void printDeviceCount(FILE *f, ND *netDev){
    int sizeStruct = Count_Devices(f);
    int CPU_count = 0;
    int Concentrator = 0;
    int FlowSensor = 0;
    int TempSensor = 0;
    int PresureSensor = 0;
    int LevelSensor = 0;
    int ValveActuator = 0;
    int MotorActuator = 0;
    for (int i = 0; i < sizeStruct; i++){
        if(netDev[i].DeviceType == 0){
            CPU_count++;
        }else if(netDev[i].DeviceType == 3){
            Concentrator++;
        }else if(netDev[i].DeviceType == 1) { 
            if(netDev[i].DT.SENSOR.TYPE == 0){
                FlowSensor++;
            }else if(netDev[i].DT.SENSOR.TYPE == 1){
                TempSensor++;
            }else if(netDev[i].DT.SENSOR.TYPE == 2){
                PresureSensor++;
            }else if(netDev[i].DT.SENSOR.TYPE == 3){
                LevelSensor++;
            }
        }else if(netDev[i].DeviceType == 2){
            if(netDev[i].DT.ACTUATOR.TYPE == 0){
                ValveActuator++;
            }else{
                MotorActuator++;
            }
        }
    }
    printf("\n All devices connected on the network.");
    printf("\n CPU:  %d", CPU_count);
    printf("\n CONCENTRATOR:  %d", Concentrator);
    printf("\n TEMP SENSOR:  %d", TempSensor);
    printf("\n VALVE:  %d", ValveActuator);
    printf("\n MOTOR:  %d", MotorActuator);
    printf("\n FLOW:  %d", FlowSensor);
    printf("\n PRESURE: %d", PresureSensor);
    printf("\n LEVEL:  %d", LevelSensor);
    printf("\n");
}

void menu(FILE *f, ND *netDev) {
    int option;
    do {
        printf("\n \t Menu ");
        printf("\n Enter 1 to view the connection sequence of a specific ID.");
        printf("\n Enter 2 to view the count of each type of device in the network");
        printf("\n Enter 0 to exit.");
        printf("\n Your option: ");

        scanf("%d", &option);

        if (option < 0 || option > 2) {
            printf("\n\n Please enter a valid option.\n");
        }
        if (option == 1) {
            ID_Connection_Sequence(f, netDev);
        }else if (option == 2){
            printDeviceCount(f, netDev);
        }
    } while (option != 0);  //el bucle no sale hasta que el usuario escriba el cero en la consola.
}