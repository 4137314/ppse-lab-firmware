/**
 * @file filesystem.h
 * @brief Interfaccia per la gestione del filesystem FatFS e l'integrazione USB
 * Mass Storage.
 * * Questo header definisce le funzioni e i flag globali necessari per gestire
 * la memoria flash interna come disco FAT, permettendo la condivisione sicura
 * dei dati tra il firmware dell'RP2040 e un PC esterno.
 */

#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <FatFS.h>
#include <FatFSUSB.h>
#include <VFS.h>
#include <stdint.h>

/* --- Variabili di stato globali --- */

/**
 * @brief Flag di notifica per il ricaricamento dei file.
 * * Viene impostato a @c true quando il disco viene scollegato dal PC,
 * segnalando al firmware di rieseguire il mount per leggere i nuovi dati.
 */
extern volatile bool updated;

/**
 * @brief Stato della connessione USB.
 * * Vale @c true se il PC ha preso il controllo del filesystem (mount
 * effettuato).
 */
extern volatile bool driveConnected;

/**
 * @brief Semaforo di accesso al filesystem.
 * * Vale @c true quando il firmware sta scrivendo o leggendo attivamente dalla
 * flash. Impedisce al PC di montare il disco contemporaneamente per evitare
 * corruzione.
 */
extern volatile bool inPrinting;

/* --- Funzioni di Configurazione --- */

/**
 * @brief Inizializza il filesystem FAT sulla Flash interna.
 * * @return true se il mount (o l'eventuale formattazione) è riuscito.
 */
bool FatFS_Setup();

/**
 * @brief Configura e avvia il servizio USB Mass Storage.
 * * @return true se il dispositivo USB è stato inizializzato correttamente.
 */
bool FatFSUSB_Setup();

/* --- Callback e Gestione Risorse --- */

/**
 * @brief Callback richiamato al distacco del cavo USB.
 * @param i Identificativo dell'interfaccia USB (fornito dal framework).
 */
void unplug(uint32_t i);

/**
 * @brief Callback richiamato quando il PC monta l'unità.
 * @param i Identificativo dell'interfaccia USB.
 */
void plug(uint32_t i);

/**
 * @brief Funzione di verifica per il mount sicuro su PC.
 * * Controlla il flag @ref inPrinting per decidere se concedere l'accesso al
 * PC.
 * @param i Identificativo dell'interfaccia USB.
 * @return true se il filesystem è libero e montabile dall'host.
 */
bool mountable(uint32_t i);

/**
 * @brief Gestisce la logica di sincronizzazione post-connessione.
 * * Se il disco è stato modificato dal PC, questa funzione si occupa di
 * ripristinare l'accesso locale.
 * @return true se il filesystem è pronto per l'uso da parte del firmware.
 */
bool sync_files();

#endif  // FILESYSTEM_H
