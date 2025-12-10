#ifndef DISPLAY_UI_H
#define DISPLAY_UI_H

    // Menu
    extern const char* menuItems[];
    extern const int menuLength;

    // Timeout schermo
    extern unsigned long lastActivity;
    extern const unsigned long timeout;

    // Funzioni principali
    bool displayInit();
    void drawHomeScreen();
    void drawMenu(int index);
    void drawSelected(const char* title);

    void drawSettingsScreen();
    void drawSensorsScreen();
    void drawSystemScreen();
    void drawInfoScreen();
    void drawGPSScreen();

    // Aggiorna timer attività e gestisce timeout
    void updateDisplayTimeout();

#endif
