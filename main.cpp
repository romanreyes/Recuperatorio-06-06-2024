#include "lib.h"

int main(){

    FILE *f = fopen ("C:\\Users\\arace\\Downloads\\Recu 06-06-2024\\network_structure.dat","rb");
        if( f == NULL){
        printf("\n File dont open.");
        return -1;
    }
    int size_struct = Count_Devices(f);
    ND *netDev= new ND[size_struct];    //netDev = network Devices
    load_network_Devices(f, netDev);


    for (int i = 0; i < size_struct; i++){     //libero memoria y cierro los archivos
        delete[] netDev[i].LLD_ID;
    }
    ID_Connection_Sequence(f, netDev);
    delete []netDev;
    fclose(f);
}