/**
 * @file sensors_i2c.h
 * @brief Driver agnostico per il bus I2C0 e la sensoristica inerziale/ambientale.
 * @details Gestisce l'astrazione dei sensori (IMU, Termometro, ADC) e del bus di comunicazione.
 * @defgroup Sensors Gestione Sensori
 * @{
 */

#ifndef SENSORS_I2C_H
#define SENSORS_I2C_H

#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Struttura dati per vettori inerziali (Accelerazione/Giroscopio).
 * @details Utilizza float per rappresentare i valori in unità fisiche (g o m/s^2).
 */
typedef struct {
    float x;              /**< Asse X. */
    float y;              /**< Asse Y. */
    float z;              /**< Asse Z. */
    uint32_t timestamp;   /**< Tempo di acquisizione (ms) per calcoli PID/Integrazione. */
} Vector3D_t;

/**
 * @brief Inizializza il bus I2C0 e i sensori collegati.
 * @return true se il bus e i sensori critici sono pronti.
 */
bool sensors_i2c_init(void);

/**
 * @brief Legge i dati dall'accelerometro.
 * @details Astrae il modello specifico del sensore (es. LIS3DH).
 * @param data Puntatore alla struttura dove salvare i dati.
 * @return true se la lettura è andata a buon fine.
 */
bool sensors_accel_get_data(Vector3D_t* data);

/**
 * @brief Esegue una scansione del bus I2C0 per diagnostica.
 * @details Utile in fase di boot per verificare che display e sensori siano correttamente alimentati.
 * @return true se almeno un device risponde sul bus.
 */
bool sensors_i2c_is_healthy(void);

/**
 * @brief Calibra il sensore inerziale.
 * @details Calcola l'offset statico (bias). Il sistema deve essere in posizione piana durante l'operazione.
 */
void sensors_accel_calibrate(void);

/**
 * @brief Legge la temperatura corrente.
 * @return Temperatura in gradi Celsius (°C).
 */
float sensors_read_temperature_c(void);

/**
 * @brief Legge la tensione della batteria.
 * @return Voltaggio letto dal partitore analogico (Volt).
 */
float sensors_read_battery_v(void);

/** @} */ // fine del gruppo Sensors

#endif /* SENSORS_I2C_H */
