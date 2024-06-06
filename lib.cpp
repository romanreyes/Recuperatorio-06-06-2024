#include "lib.h"

int Count_Devices(FILE *f){
    int countHeaders = 0;
    uint16_t Header;
    uint16_t LLDC;
    uint16_t aux;
    while (fread(&Header, sizeof(uint16_t), 1, f)){
        aux = (Header << 10) >> 10;
        LLDC = aux & 0x3F;
        fseek(f, 2*sizeof(uint16_t), SEEK_CUR);
        fseek(f, LLDC * sizeof(uint16_t), SEEK_CUR);
        countHeaders++;        
    }
    rewind(f);
    return countHeaders;
}

void load_network_Devices (FILE *f, ND *netDev){
    uint16_t Header;
    uint16_t aux;
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
        DTNH = (Header>>11)& 0x2;
        DTNL = (Header>>3)& 0x1;
        DeviceType = DTNH + DTNL;
        INFO = (Header>>4) & 0XFF;
        if(DeviceType == 0 || DeviceType == 3){
            netDev[Pos].DT.CPU_OR_CONCENTRATOR = DeviceType;
        } else if(DeviceType==1){ // Si Device Typer es un 1 entonces es un sensor
            INFO = (INFO<<2)>>6;
            netDev[Pos].DT.SENSOR.TYPE = INFO;
        }else if(DeviceType==2){ 
            INFO = (INFO>>2) & 0x1;
            netDev[Pos].DT.ACTUATOR.TYPE = INFO;
        }
        fread(&Header, sizeof(uint16_t), 1, f);
        ULDID = (Header>>3) & 0x3FF;
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
    int *v = new int[vSize + 1]; 
    Pos = busqueda_lineal(f, netDev, ID);
    for (int i = 0; i <= vSize; i++) {
        v[i] = netDev[Pos].ID;
        Pos = busqueda_lineal(f, R, R[Pos].ULD_ID);
    }
    printf("\n");

    for (int i = vSize; i >= 0; i--) { 
        printf("ID %d", v[i]);
        if (i > 0) {
            printf(" -> ");
        }
    }
    printf("\n");
    delete[] v; 
}

