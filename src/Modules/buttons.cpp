#include "buttons.h"
#include "display_ui.h"
#include "leds.h"
#include "gps.h"

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
const uint32_t COMBO_MS = 500; // finestra per considerare "insieme"

// attività timeout display
    uint32_t lastActivity = 0;
    const uint32_t DISPLAY_TIMEOUT_MS = 15000; // 15 secondi
// Debounce
const uint32_t DEBOUNCE_DELAY = 40; // ms

// Numero di voci menu
const int MENU_LENGTH = 5;

// leds navigation animation
int ledsNumber = 0;

//forward declaration
long long debounce_callback(alarm_id_t id, void *user_data);
void gpio_callback(uint gpio, uint32_t events);



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
    gpio_set_irq_enabled_with_callback(SW_UP, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
    for(uint8_t i= 1; i<(sizeof(SWpin)/sizeof(SWpin[0])); ++i){
        gpio_set_irq_enabled(SWpin[i], GPIO_IRQ_EDGE_FALL, true);
    }

    return;
}
/*
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
*/
void buttonsUpdate() {

    enum Submenu : uint8_t { SUB_NONE, SUB_SETTINGS, SUB_SENSORS, SUB_GPS, SUB_SYSTEM, SUB_INFO };
    static Submenu currentSubmenu = SUB_NONE;

    bool anypress = left_press || right_press || up_press || down_press;
    if (anypress) {
        if (right_press){
        
            ledsNumber = 1;
        } else if (left_press){
          
            ledsNumber = 5;
        } else if (up_press){
           
            ledsNumber = 7;
        } else if (down_press){
           
            ledsNumber = 3;
        }
        ledsNavigationAnimation();
        lastActivity = millis();
    }


    // --- Se siamo in un ambiente display ---
    if(ambientDisplay){
        setBrightness(255); // luminosità ridotta
        if(left_press || right_press || up_press || down_press){
            left_press = false;
            right_press = false;
            up_press = false;
            down_press = false;
            ambientDisplay = false;
            menuOpen = true;
            drawMenu(menuIndex);
        }
    }

    
    // --- Menu aperto ---
    if (up_press && menuOpen) {           // scroll up
        up_press = false;
        menuIndex = (menuIndex == 0) ? MENU_LENGTH - 1 : menuIndex - 1;
        drawMenu(menuIndex);
        return;
    }
    
    if (down_press && menuOpen) {         // scroll down
        down_press = false;
        menuIndex = (menuIndex + 1) % MENU_LENGTH;
        drawMenu(menuIndex);
        return;
    }
    
    if (right_press && menuOpen) {        // entra nel submenu
        right_press = false;
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
        
        if(inSubmenu && currentSubmenu == SUB_SETTINGS){
            if(right_press){
            right_press = false;
            // Toggle menu style
            ScreenStyle = (ScreenStyle == 1) ? 0 : 1;
            drawSettingsScreen();
            return;
        }
        
       
        if (left_press)
        {
            left_press = false;
            inSubmenu = false;
            menuOpen = true;
            drawMenu(menuIndex);
            return;
        }
        return;
        
    }

    if (inSubmenu) {
        if (left_press) { // esci dal submenu
            left_press = false;
            inSubmenu = false;
            menuOpen = true;
            drawMenu(menuIndex);
        }
        if (right_press)
        {
            right_press = false;

            // Gestisci altre azioni nei sottomenu se necessario
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

    uint32_t now = millis();
    // --- Rilevamento combo LEFT + RIGHT (entro COMBO_MS) ---
    if (left_press)  { left_press  = false; tLeft  = now; }
    if (right_press) { right_press = false; tRight = now; }

    // Se entrambi presenti e vicini nel tempo -> combo
    if (tLeft && tRight && (uint32_t)abs((int32_t)tLeft - (int32_t)tRight) <= COMBO_MS) {
        tLeft = 0;
        tRight = 0;
        onLeftRightCombo();
        return;
    }

    // Opzionale: “scadenza” eventi per non tenerli appesi
    if (tLeft && (now - tLeft > COMBO_MS))   tLeft = 0;
    if (tRight && (now - tRight > COMBO_MS)) tRight = 0;

    // (NOTA: in questo punto left_press/right_press sono già stati consumati)*/


    



}


void onLeftRightCombo() {
    menuOpen = !menuOpen;
    if (menuOpen) {
        ambientDisplay = false;
        drawMenu(menuIndex);
    } else {
        ambientDisplay = true;
        drawHomeScreen();
    }
}
