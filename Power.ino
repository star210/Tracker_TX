//===============================================================================
// Setup the defined power pins for output
void setup_PowerPins()
{
#if (POWER_PIN1 > 0)
  pinMode(POWER_PIN1, OUTPUT);
#endif

#if (POWER_PIN2 > 0)
  pinMode(POWER_PIN2, OUTPUT);
#endif

#if (POWER_PIN3 > 0)
  pinMode(POWER_PIN3, OUTPUT);
#endif

#if (POWER_PIN4 > 0)
  pinMode(POWER_PIN4, OUTPUT);
#endif

#if (POWER_PIN5 > 0)
  pinMode(POWER_PIN5, OUTPUT);
#endif
}


//===============================================================================
// Set the defined power pins HIGH
void enable_PowerPins() {
#if (POWER_PIN1 > 0)
  digitalWrite(POWER_PIN1, HIGH);
#endif

#if (POWER_PIN2 > 0)
  digitalWrite(POWER_PIN2, HIGH);
#endif

#if (POWER_PIN3 > 0)
  digitalWrite(POWER_PIN3, HIGH);
#endif

#if (POWER_PIN4 > 0)
  digitalWrite(POWER_PIN4, HIGH);
#endif

#if (POWER_PIN5 > 0)
  digitalWrite(POWER_PIN5, HIGH);
#endif

#if defined(DEVMODE)
  Serial.println(F("[GPS] On"));
#endif
}


//===============================================================================
// Set the defined power pins LOW
void disable_PowerPins()
{
  // Only disable the power pins when there it is safe to do so
#if (POWER_PIN1 > 0)
  digitalWrite(POWER_PIN1, LOW);
#endif

#if (POWER_PIN2 > 0)
  digitalWrite(POWER_PIN2, LOW);
#endif

#if (POWER_PIN3 > 0)
  digitalWrite(POWER_PIN3, LOW);
#endif

#if (POWER_PIN4 > 0)
  digitalWrite(POWER_PIN4, LOW);
#endif

#if (POWER_PIN5 > 0)
  digitalWrite(POWER_PIN5, LOW);
#endif

#if defined(DEVMODE)
  Serial.println(F("[GPS] Off"));
#endif
}

void resetPower() {
  disable_PowerPins();
  delay(1000);
  enable_PowerPins();
}
