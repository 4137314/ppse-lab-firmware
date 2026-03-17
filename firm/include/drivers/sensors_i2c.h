/**
 * @file sensors_i2c.h
 * @brief Driver agnostico per il bus I2C0 e la sensoristica inerziale/ambientale.
 * @details Gestisce l'astrazione dell'accelerometro e del bus di comunicazione.
 * Repository: https://github.com/4137314/ppse-lab-firmware
 */

#ifndef SENSORS_I2C_H
#define SENSORS_I2C_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Struttura dati per vettori inerziali (Accelerazione/Giroscopio).
 * @details Utilizza float per rappresentare i valori in unità fisiche (g o m/s^2).
 */
typedef struct {
    float x;
    float y;
    float z;
    uint32_t timestamp; /**< Tempo di acquisizione (ms) per calcoli PID/Integrazione. */
} Vector3D_t;

/**
 * @brief Inizializza il bus I2C0 (GPIO 12/13) e i sensori collegati.
 * @return true se il bus e i sensori critici sono pronti.
 */
bool sensors_i2c_init(void);

/** * @brief Legge i dati dall'accelerometro.
 * @details Astrae il modello specifico del sensore (es. LIS3DH).
 * @param data Puntatore alla struttura dove salvare i dati.
 * @return true se la lettura è andata a buon fine.
 */
bool sensors_accel_get_data(Vector3D_t* data);

/** * @brief Esegue una scansione del bus I2C0 per diagnostica.
 * @details Utile in fase di boot per verificare che display e sensori siano alimentati.
 * @return true se almeno un device risponde sul bus.
 */
bool sensors_i2c_is_healthy(void);

/**
 * @brief Calibra il sensore inerziale.
 * @details Calcola l'offset statico (bias). Il drone deve essere perfettamente piano.
 */
void sensors_accel_calibrate(void);

#endif /* SENSORS_I2C_H */
