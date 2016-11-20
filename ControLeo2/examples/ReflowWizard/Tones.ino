// Play a signal
void playTones(int duration, int pause, int times) {
  for (int i = 0; i < times; i++)
  {
    digitalWrite(CONTROLEO_BUZZER_PIN, HIGH);
    delay(duration);
    digitalWrite(CONTROLEO_BUZZER_PIN, LOW);
    delay(pause);
  }
}

