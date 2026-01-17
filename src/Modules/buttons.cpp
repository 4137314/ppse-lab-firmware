#include "buttons.h"

// Variabili globali per menu
int menuIndex = 0;
bool menuOpen = true;      // Menu sempre visibile
bool inSubmenu = false;

volatile bool up_press=false;
volatile bool down_press=false;
volatile bool left_press=false;
volatile bool right_press=false;

// Debounce
const uint32_t DEBOUNCE_DELAY = 40; // ms

// Numero di voci menu
const int MENU_LENGTH = 5;

// Inizializzazione dei pulsanti
void buttonsInit() {
    const uint8_t SWpin[]={SW_UP,SW_DOWN,SW_LEFT,SW_RIGHT};

    #if DEBUG == 1
    Serial.println(F("Buttons and relative interrupts initialized"));
    #endif

    for(uint8_t i=0; i<sizeof(SWpin); ++i){
        pinMode(SWpin[i], INPUT_PULLUP);
        gpio_set_irq_enabled_with_callback(SWpin[i], GPIO_IRQ_EDGE_FALL, true, &buttons_callback);
    }   

    return;
}


// funzione handler per interrupt gpio
void buttons_callback(uint gpio, uint32_t events) {
   //if released enables the fall edge irq and disable the rising one
    if(events == GPIO_IRQ_EDGE_RISE){
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

    if(!gpio_get( PinNum ))
    {
        switch( PinNum )
        {
            case SW_UP: up_press=true; break;

            case SW_DOWN: down_press=true; break;
            
            case SW_LEFT: left_press=true; break;

            case SW_RIGHT: right_press=true; break;

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


// Gestione dei pulsanti, da chiamare nel loop
void buttonsUpdate() {

    // --- Se siamo in un sottomenu ---
    if (inSubmenu) {
        if (left_press) { // esci dal submenu
            left_press = false;
            inSubmenu = false;
            drawMenu(menuIndex);
        }
        return;
    }

    // --- Menu aperto ---
    if (up_press) {           // scroll up
        up_press = false;
        menuIndex = (menuIndex == 0) ? MENU_LENGTH - 1 : menuIndex - 1;
        drawMenu(menuIndex);
        return;
    }

    if (down_press) {         // scroll down
        down_press = false;
        menuIndex = (menuIndex + 1) % MENU_LENGTH;
        drawMenu(menuIndex);
        return;
    }

    if (right_press) {        // entra nel submenu
        right_press = false;
        inSubmenu = true;
        switch (menuIndex) {
            case 0: drawSettingsScreen(); break;
            case 1: drawSensorsScreen();  break;
            case 2: drawGPSScreen();      break;
            case 3: drawSystemScreen();   break;
            case 4: drawInfoScreen();     break;
        }
    }

    if (left_press) {         // torni al menu (resta aperto)
        left_press = false;
        drawMenu(menuIndex);                 // utile se vuoi refresh manuale
    }
}
