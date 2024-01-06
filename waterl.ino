#include <Wire.h>
#include <U8g2lib.h>

U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);

const int trigPin = 3;                         // Pin trigger ultrasonik terhubung ke pin digital 3
const int echoPin = 2;                         // Pin echo ultrasonik terhubung ke pin digital 2
int ledBarPin[8] = {4, 5, 6, 7, 8, 9, 10, 12}; // Pin LED bar terhubung ke pin digital 4 hingga 11
const int alarmPin = 11;                       // Pin untuk alarm
float hsensor, hair, levelair;                 // Untuk Melihat Data
const int numPoints = 128;                     // Jumlah titik pada grafik
int hairValues[numPoints];                     // Array untuk menyimpan nilai hair
int currentIndex = 0;                          // Indeks saat ini pada array

void setup()
{
  u8g2.begin();

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  for (int i = 0; i < 8; i++)
  {
    pinMode(ledBarPin[i], OUTPUT);
  }
  pinMode(alarmPin, OUTPUT);
}

void loop()
{
  // Mengirim sinyal ultrasonik
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);

  static unsigned long lastAlarmTime = 0;
  unsigned long currentTime = millis();

  float duration = pulseIn(echoPin, HIGH);
  // Menghitung jarak berdasarkan durasi
  float distance = (duration / 2) * 0.0343; // Nilai 0.0343 adalah konstanta untuk mengkonversi durasi menjadi jarak dalam satuan cm

  hsensor = distance;
  hair = 500 - hsensor;

  // Menghitung level air dalam persentase
  levelair = (hair / 500) * 100;
  // Menghitung persentase kecerahan berdasarkan jarak
  float brightness = map(hsensor, 0, 400, 255, 0);

  // Menentukan jumlah LED yang akan menyala berdasarkan nilai kecerahan
  float numLEDsToLight = map(hsensor, -40, 400, 0, 8);
  numLEDsToLight = constrain(numLEDsToLight, 0, 8); // Pastikan nilai berada dalam rentang 0-8

  // Mematikan semua LED
  for (int i = 0; i < 8; i++)
  {
    digitalWrite(ledBarPin[i], LOW);
  }

  // Menyalakan LED sesuai dengan nilai kecerahan
  for (int i = 0; i < numLEDsToLight; i++)
  {
    digitalWrite(ledBarPin[i], HIGH);
  }

  // Aktifkan alarm dan bunyikan buzzer jika jarak kurang dari atau sama dengan 50 cm (sesuaikan dengan kebutuhan)
  if (levelair >= 75)
  {
    // Cek apakah sudah 3 detik sejak alarm terakhir diaktifkan
    if (currentTime - lastAlarmTime >= 1000)
    {
      digitalWrite(alarmPin, LOW); // Aktifkan alarm
      tone(alarmPin, 1000);         // Bunyikan buzzer pada frekuensi 1000 Hz (sesuaikan dengan kebutuhan)
      lastAlarmTime = currentTime;  // Perbarui waktu terakhir alarm diaktifkan
    }
    else {
      digitalWrite(alarmPin, HIGH); // Aktifkan alarm
      noTone(alarmPin);
    }
  }
  else
  {
    digitalWrite(alarmPin, HIGH); // Matikan alarm
    noTone(alarmPin);             // Matikan bunyi buzzer
  }

  for (int i = 0; i < numPoints - 1; i++)
  {
    hairValues[i] = hairValues[i + 1];
  }
  // Masukkan nilai hair terbaru ke dalam array
  hairValues[numPoints - 1] = hair;

  // Gambar line chart
  u8g2.firstPage();
  do
  {
    drawLineChart();
  } while (u8g2.nextPage());

  delay(10);
}

void drawLineChart()
{
  // ... (rest of the drawLineChart function remains unchanged)
  // Tentukan tinggi maksimum chart
  int maxChartHeight = 32;

  // Tampilkan tulisan di atas chart
  u8g2.setFont(u8g2_font_helvB08_tr); // Ukuran font 8
  u8g2.setCursor(0, 10);
  u8g2.print("Permukaan : ");
  u8g2.print(hsensor);
  u8g2.print(" cm");
  u8g2.setCursor(0, 20);
  u8g2.print("Tinggi Air : ");
  u8g2.print(hair);
  u8g2.print(" cm");
  u8g2.setCursor(0, 30);
  u8g2.print("Level Air : ");
  u8g2.print(levelair);
  u8g2.print("%");

  // Gambar sumbu X dan Y
  u8g2.drawLine(0, 63 - maxChartHeight, 127, 63 - maxChartHeight); // sumbu X
  
// Gambar line chart dengan nilai potensiometer
  for (int i = 1; i < numPoints; i++)
  {
    int x1 = map(i - 1, 0, numPoints - 1, 0, 127);
    int y1 = map(hairValues[i - 1], 0, 500, 0, maxChartHeight);
    int x2 = map(i, 0, numPoints - 1, 0, 127);
    int y2 = map(hairValues[i], 0, 500, 0, maxChartHeight);
    u8g2.drawLine(x1, 63 - y1, x2, 63 - y2);
  }
}