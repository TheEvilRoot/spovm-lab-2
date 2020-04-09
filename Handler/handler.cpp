#include <iostream>
#include <ctime>

#include <Windows.h>

int getRandomInt(int min, int max) {
  return min + (rand() % static_cast<int>(max - min + 1));
}

char *getRandomString(size_t size) {
  char *string = new char[size + 1];

  for (size_t i = 0; i < size; i++) {
    if (getRandomInt(0, 3) > 1) {
      string[i] = getRandomInt('A', 'Z');
    } else {
      string[i] = getRandomInt('a', 'z');
    }
  }

  string[size] = '\0';
  return string;
}

void waitForQueue(HANDLE &allowedEvent) { 
  WaitForSingleObject(allowedEvent, INFINITE);
}

int main(const int argc, const char *argv[]) {
  srand(time(NULL));
  
  auto allowedEvent = OpenEventA(EVENT_ALL_ACCESS, false, "Allowed");
  char *string = getRandomString(10);

  while (true) {
    waitForQueue(allowedEvent);

    fprintf(stderr, "\r");
    for (int i = 0; string[i]; i++) {
      Sleep(100);
      fprintf(stderr, "%c", string[i]);
    }
    fprintf(stderr, "\n");

    SetEvent(allowedEvent);
  }
  
  return 0;
}