void setup() {
  Serial.begin(9600); //Serielle Verbindung starten
  Serial.println("Arduino Rechner bereit. Eingabe im Format: Zahl1+Zahl2"); 
}

bool processing = false; //Verhindert, das mehrere Berechnungen gleichzeitig laufen

void loop() {
  if (Serial.available() > 0 && !processing) {
    String input = Serial.readStringUntil('\n'); //Liest Eingabe bis zum Zeilenumbruch
    input.trim(); //Entfernt leerzeichen

    if (input.length() == 0) {
      processing = false;
      return;
    }

    Serial.print("Ergebnis: ");
    Serial.println(calculate(input)); //Berechnen und Ergebnis ausgeben

    processing = false;
  }
  delay(50); //Pause
}

//Führt Rechnung durch
String calculate(String term) {
  int operatorIndex = -1;
  char operation = '\0';
  

  //Findet den Rechenoperator (+, -, *, /)
  for (int i = 0; i < term.length(); i++) {
    if (term[i] == '+' || term[i] == '-' || term[i] == '*' || term[i] == '/') {
      operatorIndex = i;
      operation = term[i];
      break;
    }
  }

  if (operatorIndex == -1) return "Fehler: Kein Operator gefunden";
  
  //Teilt den String in zwei zahlen auf
  String num1Str = term.substring(0, operatorIndex);
  String num2Str = term.substring(operatorIndex + 1);


  if (!isValidNumber(num1Str) || !isValidNumber(num2Str)) {
    return "Fehler: Ungültige Zahl";
  }

  float num1 = num1Str.toFloat();
  float num2 = num2Str.toFloat();
  float result = 0;

  switch (operation) {
    case '+': result = num1 + num2; break;
    case '-': result = num1 - num2; break;
    case '*': result = num1 * num2; break;
    case '/': 
      if (num2 == 0) return "Fehler: Division durch 0";
      result = num1 / num2; 
      break;
    default: return "Fehler: Ungültiger Operator";
  }

  return String(result);
}

//Prüft, ob der String eine gültige Zahl ist
bool isValidNumber(String numStr) {
  for (int i = 0; i < numStr.length(); i++) {
    if (!isDigit(numStr[i])) return false;
  }
  return true;
}
