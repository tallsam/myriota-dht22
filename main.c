#include <unistd.h>
#include <stdlib.h> // need rand()
#include "myriota_hardware_api.h"
#include "myriota_user_api.h"
#include "dht22.h" // DHT22 Lib.

// Format of the messages to be transmitted. Values are little endian
typedef struct
{
  uint16_t sequence_number;
  float temp;
  float humidity;
  uint32_t timestamp; // epoch timestamp of last fix
} __attribute__((packed)) satMessage;

time_t TempToSatellite()
{
  // Set the sequence number
  static unsigned int sequence_number = 0;

  // Get current timestamp.
  time_t timestamp;
  timestamp = TimeGet();

  // Get temp and humidity from DHT22 sensor.

  static uint8_t pin = PIN_GPIO7; //pin 13 on board.
  struct dht22 dht;
  float temp, humidity;

  dht_init(&dht, pin);
  dht_read_data(&dht, &temp, &humidity);

  //printf("temp = %f, humidity = %f\n", temp, humidity);

  //Build the message
  const satMessage message = {sequence_number, temp, humidity, timestamp};

  //printf("Size of message: %i", sizeof(message));

  //Schedule the message
  if (ScheduleMessage((void *)&message, sizeof(message)) > 1)
  {
    printf("Overloaded\n");
  }

  sequence_number++;

  return HoursFromNow(1); // HoursFromNow(3)
}

int BoardStart(void)
{
  LedTurnOn();
  Delay(200);
  LedTurnOff();
  
  // Set the time and location manually as we can't use GNSS inside.
  // printf("Input current time and location\n");
  // int Time;
  // float Lat, Lon;
  // char Input[100] = {0};
  // size_t Index = 0;
  // while (1)
  // {
  //   char Ch;
  //   if (read(0, &Ch, 1) == 1)
  //   {
  //     Input[Index++] = Ch;
  //     if (Ch == '\n')
  //       break;
  //   }
  // }
  // sscanf(Input, "%d,%f,%f", &Time, &Lat, &Lon);
  // TimeSet(Time);
  // LocationSet(Lat * 1e7, Lon * 1e7);
  return 0;
}

void AppInit()
{
  GPIOSetModeOutput(PIN_GPIO7);
  ScheduleJob(TempToSatellite, ASAP());
}
