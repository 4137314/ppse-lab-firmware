#include "buttons.h"
#include "display_ui.h"
#include "leds.h"
#include "gps.h"
#include "weather.h"


// Variabili globali per menu
int menuIndex = 0;
bool menuOpen = false;      // Menu sempre visibile
bool inSubmenu = false;
bool ambientDisplay = true;

volatile bool up_press=false;
volatile bool down_press=false;
volatile bool left_press=false;
volatile bool right_press=false;

// funzioanlita combo
uint32_t tLeft  = 0; 
uint32_t tRight = 0;
const uint32_t COMBO_MS = 300; // finestra per considerare "insieme"

// attività timeout display
uint32_t lastActivity = 0;
const uint32_t DISPLAY_TIMEOUT_MS = 5000000; // 15 secondi
// Debounce
const uint32_t DEBOUNCE_DELAY = 40; // ms

// Numero di voci menu
const int MENU_LENGTH = 5;

// leds navigation animation
//  diffinito nel header enum KeyEvent : uint8_t { KEY_NONE, KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT };
int ledsNumber = 0;

//forward declaration
long long debounce_callback(alarm_id_t id, void *user_data);
void gpio_callback(uint gpio, uint32_t events);

// Definizione enum per i sottomenu
enum Submenu : uint8_t { SUB_NONE, SUB_SETTINGS, SUB_SENSORS, SUB_GPS, SUB_SYSTEM, SUB_INFO };

// funzione handler per interrupt gpio
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



long long debounce_callback(alarm_id_t id, void *user_data){
    uint PinNum=(uint) user_data;
    
    Serial.print("Debounce callback for pin ");
    Serial.println(PinNum);
    
    if(!gpio_get( PinNum ))
    {
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



// Inizializzazione dei pulsanti
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
    if (ev != KEY_NONE) {
        if (checkLeftRightCombo(ev)) return; // se combo: toggle e fine
    }

    // se ritorna false => nessuna combo: puoi eseguire l’azione singola (LEFT o RIGHT)
    
    if(!ambientDisplay && (millis() - lastActivity > DISPLAY_TIMEOUT_MS)) {
        ambientDisplay = true;
        menuOpen = false;
        inSubmenu = false;
        ledsIncativityAnimation();
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
                drawSensorsScreen(); 
                currentSubmenu = SUB_SENSORS; 
                break;
                case 2: 
                drawGPSScreen();  
                currentSubmenu = SUB_GPS;
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
        
        if(currentSubmenu == SUB_SETTINGS){
            if(ev == KEY_RIGHT){
            // Toggle menu style
            ScreenStyle = (ScreenStyle == 1) ? 0 : 1;
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


// Ritorna true se ha gestito qualcosa (combo o attesa); false se puoi proseguire con azioni normali
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

