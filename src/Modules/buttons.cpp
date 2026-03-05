/**
 * @file buttons.cpp
 * @brief Gestione dell'input utente, sitema di debounce e macchina a stati del menu.
 * * Questo modulo implementa la logica di navigazione tramite pulsanti GPIO, 
 * utilizzando interrupt (IRQ) e allarmi hardware per il debounce. Gestisce inoltre
 * la logica di timeout del display e le combo di tasti.
 */

#include "buttons.h"
#include "display_ui.h"
#include "leds.h"
#include "gps.h"
#include "weather.h"


/** @name Stato Navigazione Menu */
/**@{*/
int menuIndex = 0;				/**< Indice della voce di menu selezionata. */
bool menuOpen = false;      			/**< Stato di apertura del menu principale. */
bool inSubmenu = false;				/**< Flag di presenza all'interno di un sottomenu. */
bool ambientDisplay = true;			/**< Stato del display: true se in modalità risparmio/home. */
/**@}*/

/** @name Flag Interrupt Pulsanti */
/**@{*/
volatile bool up_press=false;			/**< Flag IRQ per pulsante UP. */
volatile bool down_press=false;			/**< Flag IRQ per pulsante DOWN. */
volatile bool left_press=false;			/**< Flag IRQ per pulsante LEFT. */
volatile bool right_press=false;		/**< Flag IRQ per pulsante RIGHT. */
/**@}*/

/** @name Configurazione Temporale e Debounce */
/**@{*/
uint32_t tLeft  = 0; 
uint32_t tRight = 0;
const uint32_t COMBO_MS = 300; 			/**< Finestra temporale (ms) per rilevare una pressione simultanea. */
uint32_t lastActivity = 0;			/**< Timestamp dell'ultima interazione utente. */
const uint32_t DISPLAY_TIMEOUT_MS = 5000000;	/**< Timeout (ms) per il ritorno automatico alla Home. */
const uint32_t DEBOUNCE_DELAY = 40; 		/**< Delay (ms) per il filtraggio del rimbalzo meccanico. */
/**@}*/

const int MENU_LENGTH = 5;			/**< Numero totale di voci nel menu principale. */
int ledsNumber = 0;				/**< Identificativo LED per animazione navigazione. */

/** * @brief Definizione dei sottomenu disponibili nel sistema.
 */
enum Submenu : uint8_t {
	SUB_NONE,				/**< Nessun sottomenu attivo. */
	SUB_SETTINGS,				/**< Sottomenu Impostazioni. */
	SUB_METEO,				/**< Visualizzazione Dati Meteo. */
	SUB_GPS,				/**< Visualizzazione/Log Dati GPS. */
	SUB_SYSTEM,				/**< Informazioni di Sistema e Stato Hardware. */
	SUB_INFO				/**< Crediti e informazioni progetto. */
};

/* --- Forward Declarations per Doxygen --- */
long long debounce_callback(alarm_id_t id, void *user_data);
void gpio_callback(uint gpio, uint32_t events);

/**
 * @brief Handler degli interrupt GPIO per i pulsanti.
 * * Gestisce i fronti di discesa (pressione) e di salita (rilascio). 
 * Al rilevamento della pressione, disabilita l'interrupt e avvia un allarme di debounce.
 * * @param gpio Numero del pin che ha scatenato l'evento.
 * @param events Tipo di evento IRQ (Fronti rilevati).
 */
void gpio_callback(uint gpio, uint32_t events) {
    //if released re apply the fall edge irq and disable the rising one
    if(events & GPIO_IRQ_EDGE_RISE){
        gpio_set_irq_enabled(gpio, GPIO_IRQ_EDGE_RISE, false);
        gpio_set_irq_enabled(gpio, GPIO_IRQ_EDGE_FALL, true);
        return;
    }
    
    switch(gpio){
        
        case SW_UP:
        case SW_DOWN:
        case SW_LEFT:
        case SW_RIGHT:
        {
            //Disables interrupt for the gpio
            gpio_set_irq_enabled(gpio, GPIO_IRQ_EDGE_FALL, false);
            //Add an alarm callback to be called after a delay specified in milliseconds.
            add_alarm_in_ms (DEBOUNCE_DELAY, &debounce_callback, (void*) gpio, true);
        } break; 
        
        default:{
            ;
        }
        
    } return; 
}


/**
 * @brief Callback dell'allarme per il filtraggio software del debounce.
 * * Verifica se dopo @ref DEBOUNCE_DELAY il pin è ancora basso. In caso positivo,
 * valida la pressione impostando il relativo flag @c _press.
 * * @param id ID dell'allarme hardware.
 * @param user_data Puntatore al numero del pin GPIO passato dall'handler.
 * @return long long 0 per indicare che l'allarme non deve ripetersi.
 */
long long debounce_callback(alarm_id_t id, void *user_data){
    uint PinNum=(uint) user_data;
    
    
    if(!gpio_get( PinNum ))
    {
        Serial.print("Button press detected on pin ");
        Serial.println(PinNum);
        switch( PinNum )
        {
            case SW_UP: up_press=true; ; break;
            
            case SW_DOWN: down_press=true; ; break;
            
            case SW_LEFT: left_press=true; ; break;
            
            case SW_RIGHT: right_press=true; ; break;
            
            default:
            {
                #if DEBUG == 1
                Serial.println("ERROR in debounce_callback: PinNum not correct");
                #endif
                break;
            }
        }  
        //re enables gpio interrupt but for complementary edge
        gpio_set_irq_enabled(PinNum, GPIO_IRQ_EDGE_RISE, true);
        return 0;
    }
    gpio_set_irq_enabled(PinNum, GPIO_IRQ_EDGE_FALL, true); 
    return 0;
}

/**
 * @brief Inizializza i pin dei pulsanti e configura gli interrupt.
 */
void buttonsInit() {
    const uint8_t SWpin[]={SW_UP,SW_DOWN,SW_LEFT,SW_RIGHT};
    
    #if DEBUG == 1
    Serial.println(F("Buttons and relative interrupts initialized"));
    #endif
    
    for(uint8_t i=0; i<(sizeof(SWpin)/sizeof(SWpin[0])); ++i){
        pinMode(SWpin[i], INPUT_PULLUP);
        
    }   
    // Imposta interrupt sui pulsanti
    gpio_set_irq_enabled_with_callback(SW_UP, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &gpio_callback);
    for(uint8_t i= 1; i<(sizeof(SWpin)/sizeof(SWpin[0])); ++i){
        gpio_set_irq_enabled(SWpin[i], GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true);
    }
    
    return;
}

/**
 * @brief Funzione di update della logica dei menu. Da chiamare nel loop principale.
 * * Gestisce la navigazione, l'attivazione dei sottomenu, le animazioni LED e
 * il timeout per l'autospegnimento del menu (Ambient Display).
 */
void buttonsUpdate() {
    static Submenu currentSubmenu = SUB_NONE;
    
    
    
    KeyEvent ev = KEY_NONE;
    
    if (right_press) ev = KEY_RIGHT;
    else if (left_press) ev = KEY_LEFT;
    else if (up_press) ev = KEY_UP;
    else if (down_press) ev = KEY_DOWN;
    
    bool anypress = (ev != KEY_NONE);
    
    
    
    if (anypress) {
        // mapping per animazione LED
        if (ev == KEY_RIGHT)      ledsNumber = 1;
        else if (ev == KEY_LEFT)  ledsNumber = 5;
        else if (ev == KEY_UP)    ledsNumber = 7;
        else if (ev == KEY_DOWN)  ledsNumber = 3;
        
        ledsNavigationAnimation();
        
        // consuma i flag (così non si ripete)
        up_press = down_press = left_press = right_press = false;
        
        lastActivity = millis();
    }
    
    if (lastActivity == 0) {
        lastActivity = millis();
    }
    // Se ev è LEFT o RIGHT, gestisci combo/attesa
    if (ev == KEY_LEFT || ev == KEY_RIGHT) {
        if (checkLeftRightCombo(ev)) return; // se combo: toggle e fine
    }

    // se ritorna false => nessuna combo: puoi eseguire l’azione singola (LEFT o RIGHT)
    
    if(!ambientDisplay && (millis() - lastActivity > DISPLAY_TIMEOUT_MS)) {
        ambientDisplay = true;
        menuOpen = false;
        inSubmenu = false;
        //ledsInactivityAnimation();
        drawHomeScreen();
        return;
    }
    
    // --- Se siamo in un ambiente display ---
    if(ambientDisplay && anypress){
        
        ambientDisplay = false;
        menuOpen = true;
        drawMenu(menuIndex);
        
        return;
    }
    
    // (NOTA: in questo punto left_press/right_press sono già stati consumati)*/
    
    // --- Menu aperto ---
    if (ev == KEY_UP && menuOpen) {           // scroll up
        menuIndex = (menuIndex == 0) ? MENU_LENGTH - 1 : menuIndex - 1;
        drawMenu(menuIndex);
        return;
    }
    
    if (ev == KEY_DOWN && menuOpen) {         // scroll down
        menuIndex = (menuIndex + 1) % MENU_LENGTH;
        drawMenu(menuIndex);
        return;
    }
    
    if (ev == KEY_RIGHT && menuOpen) {        // entra nel submenu
        inSubmenu = true;
        menuOpen = false;
        switch (menuIndex) {
            case 0: 
                drawSettingsScreen(); 
                currentSubmenu = SUB_SETTINGS;
                break;
                case 1: 
                drawMeteoScreen(); 
                currentSubmenu = SUB_METEO; 
                break;
                case 2: 
                 currentSubmenu = SUB_GPS;
                drawGPSScreen();    // disegna subito
                break;
                break;
                case 3: 
                drawSystemScreen();  
                currentSubmenu = SUB_SYSTEM; 
                break;
                case 4: 
                drawInfoScreen(); 
                currentSubmenu = SUB_INFO;
                break;
            }
        }
        if(currentSubmenu == SUB_METEO){
            switch (ev)
            {
            case KEY_LEFT:
                gpsDayIndex = (gpsDayIndex + 6) % 7;
                drawMeteoScreen();
                return;
            case KEY_RIGHT:
                gpsDayIndex = (gpsDayIndex + 1) % 7;
                drawMeteoScreen();
                return;
            case KEY_UP:
                gpsHourIndex = (gpsHourIndex + 23) % 24;
                drawMeteoScreen();
                return;
            case KEY_DOWN:
                gpsHourIndex = (gpsHourIndex + 1) % 24;
                drawMeteoScreen();
                return;
            default:
                break;
            }
        }
        
        if(currentSubmenu == SUB_GPS && (ev==KEY_RIGHT)){
            // Save GPS data to log file
            save_gps_log20(&gpsData);
            save_gps_last(&gpsData);
            drawGPSScreen();    // aggiorna schermata dopo salvataggio
        }

        if(currentSubmenu == SUB_SETTINGS){
            if(ev == KEY_RIGHT){
            // Toggle menu style
            ScreenStyle = (ScreenStyle == 0) ? 1 : 0;
            drawSettingsScreen();
            return;
        }
        
       
        if (ev == KEY_LEFT)
        {
            inSubmenu = false;
            menuOpen = true;
            drawMenu(menuIndex);
            return;
        }
        return;
        
    }

    if (inSubmenu && currentSubmenu == SUB_GPS)
    {
        int timer= 500; // ms
        unsigned int lastMillis = millis();
        if(millis() - lastMillis > timer){
            lastMillis = millis();
            drawGPSScreen();    // disegna subito
        }

    }
    


    if (inSubmenu) {
        if (ev == KEY_LEFT) { // esci dal submenu
            inSubmenu = false;
            menuOpen = true;
            drawMenu(menuIndex);
        }
        return;
    }
    
    /*
    if (left_press && menuOpen) {         // torni al menu (resta aperto)
    left_press = false;
    menuOpen = false;
        ambientDisplay = true;
        drawHomeScreen();
    }*/

   /*
   if (inSubmenu && currentSubmenu == SUB_GPS) {
    if (ev == KEY_UP) {
        gpsDayIndex = (gpsDayIndex == 0) ? 6 : gpsDayIndex - 1;
        drawGPSScreen();
        return;
    }
    if (ev == KEY_DOWN) {
        gpsDayIndex = (gpsDayIndex + 1) % 7;
        drawGPSScreen();
        return;
    }
    if (ev == KEY_LEFT) {
        gpsHourIndex = (gpsHourIndex == 0) ? 23 : gpsHourIndex - 1;
        drawGPSScreen();
        return;
    }
    if (ev == KEY_RIGHT) {
        gpsHourIndex = (gpsHourIndex + 1) % 24;
        drawGPSScreen();
        return;
    }
    */
    
    // uscita dal submenu (se vuoi usare una combo o long press)
  // esempio: LEFT+RIGHT combo già ce l’hai per toggle home/menu.
  return;
}

/**
 * @brief Verifica la pressione simultanea dei tasti LEFT e RIGHT.
 * * @param ev L'ultimo evento tasto rilevato.
 * @return true se la combo è stata rilevata e gestita.
 */
bool checkLeftRightCombo(KeyEvent ev) {
    static uint32_t lastL = 0;
    static uint32_t lastR = 0;

    uint32_t now = millis();

    if (ev == KEY_LEFT)  lastL = now;
    if (ev == KEY_RIGHT) lastR = now;

    if (lastL && lastR && (uint32_t)abs((int32_t)(lastL - lastR)) <= COMBO_MS) {
        lastL = 0;
        lastR = 0;
        onLeftRightCombo();
        return true;   // combo gestita
    }

    // pulizia (opzionale)
    if (lastL && (now - lastL > COMBO_MS)) lastL = 0;
    if (lastR && (now - lastR > COMBO_MS)) lastR = 0;

    return false;
}

/**
 * @brief Azione eseguita al rilevamento della combo LEFT+RIGHT.
 * Esegue il toggle tra modalità Home (Ambient) e Menu.
 */
void onLeftRightCombo() {
    ledsToggleAnimation();
    ambientDisplay = !ambientDisplay;
    if (!ambientDisplay) {
        ambientDisplay = false;
        drawMenu(menuIndex);
    } else {
        ambientDisplay = true;
        drawHomeScreen();
    }
}

