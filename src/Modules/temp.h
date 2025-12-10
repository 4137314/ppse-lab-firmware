#ifndef TEMP_H
#define TEMP_H

    // Inizializza il pin del sensore e controlla se è valido
    void TempInit();

    // Lettura in gradi Celsius (LM35)
    float readTemp();

#endif