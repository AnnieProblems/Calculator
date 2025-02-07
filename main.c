#include <stdio.h>
#include <windows.h>
#include <string.h>

#define COM_PORT "COM3" //COM-Port hier anpassen
#define BUFFER_SIZE 128 //Größe des Puffers für Ein- und Ausgaben
#define LOG_FILE "Calculator LOG.txt" //Logdatei-Name

//Serielle Verbindung einrichten
void setupSerial(HANDLE *hSerial) { //
    *hSerial = CreateFile(COM_PORT, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0); 

    if (*hSerial == INVALID_HANDLE_VALUE) {
        printf("Fehler: COM-Port nicht gefunden.\n");
        return;
    }

    DCB dcb = {0};
    dcb.DCBlength = sizeof(dcb);
    GetCommState(*hSerial, &dcb);

    //Standard-Seriell-Einstellungen: 9600 Baud, 8 Datenbits, 1 Stopbit, keine Parität
    dcb.BaudRate = CBR_9600;
    dcb.ByteSize = 8;
    dcb.StopBits = ONESTOPBIT;
    dcb.Parity = NOPARITY;
    SetCommState(*hSerial, &dcb);
}

//Daten an Arduino senden
void sendData(HANDLE hSerial, const char *data) {
    DWORD bytesWritten;
    PurgeComm(hSerial, PURGE_TXCLEAR); // Vorherigen Puffer leeren
    WriteFile(hSerial, data, strlen(data), &bytesWritten, NULL);
    Sleep(200); //Arduino etwas Zeit geben
}

//Daten vom  Arduino empfangen
int receiveData(HANDLE hSerial, char *buffer) {
    DWORD bytesRead;
    int index = 0;
    char c;

    //Zeichenweise lesen, bis ein Zeilenumbruch kommt
    while (index < BUFFER_SIZE - 1) {
        if (ReadFile(hSerial, &c, 1, &bytesRead, NULL) && bytesRead > 0) {
            buffer[index++] = c;
            if (c == '\n') break; //Aus der Schleife gehen wenn Zeilenumbruch empfangen wurde
        }
    }

    buffer[index] = '\0'; //String ende
    return 1;
}

//Rechnung und Ergebnis in Logdatei speichern
void logToFile(const char *rechnung, const char *ergebnis) {
    FILE *file = fopen(LOG_FILE, "a"); //Datei im anhang-Modus öffnen
    if (file == NULL) {
        printf("Fehler: Logdatei konnte nicht geöffnet werden.\n");
        return;
    }

    fprintf(file, "Rechnung: %s", rechnung);
    fprintf(file, "Ergebnis: %s", ergebnis); 
    fprintf(file, "--------------\n\n");

    fclose(file);
}


int main() {
    HANDLE hSerial;
    setupSerial(&hSerial);
    if (hSerial == INVALID_HANDLE_VALUE) return -1; //Falls kein Port geöffnet werden konnte

    char buffer[BUFFER_SIZE];

    while (1) {
        printf("Rechnung eingeben: ");
        char input[BUFFER_SIZE];
        fgets(input, sizeof(input), stdin); //Eingabe lesen

        if (strlen(input) < 2) {
            printf("Leere Eingabe ignoriert.\n");
            continue;
        }

        sendData(hSerial, input); //An Arduino senden

        if (receiveData(hSerial, buffer)) { //Antwort empfangen
            printf("%s", buffer); //Antwort anzeigen
            logToFile(input, buffer); //In Logdatei speichern
        }

        Sleep(300); //Kurze Pause
        printf("\n----------------\n\n");
    }

    CloseHandle(hSerial); //Verbindung schließen
    return 0;
}
