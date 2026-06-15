/**
 * @file boot_anim.h
 * @brief Gestione delle sequenze grafiche di avvio (Boot Animations).
 * @details Fornisce primitive per eseguire animazioni di sistema all'accensione
 * senza bloccare l'esecuzione dei task critici.
 * @defgroup BootAnim Animazioni Avvio
 * @{
 */

#ifndef BOOT_ANIM_H
#define BOOT_ANIM_H

/**
 * @brief Esegue la sequenza di avvio "Hacker-style".
 * @details Questa funzione esegue l'animazione di boot sul display. 
 * Utilizza una funzione di callback per permettere al sistema di eseguire 
 * operazioni (come il check delle periferiche) durante l'animazione.
 * * @param update_callback Puntatore a funzione invocato durante ogni frame 
 * dell'animazione per mantenere attivi i task di sistema.
 */
void run_hacker_boot_sequence(void (*update_callback)(void));

/** @} */ // fine del gruppo BootAnim

#endif // BOOT_ANIM_H
