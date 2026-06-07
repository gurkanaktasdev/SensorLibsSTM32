# Embedded Sensor & Communication Libraries

Bu repo, gömülü sistemler ve robotik projelerinde sıkça kullanılan çeşitli sensörler ve haberleşme modülleri için geliştirilmiş sürücü kütüphanelerini (`.c` ve `.h` dosyaları) ve bunlara ait örnek uygulamaları içermektedir.

## 🛠️ Desteklenen Modüller ve İşlevleri

Aşağıdaki tabloda repoda yer alan modüllerin kısa açıklamaları ve kullanım amaçları listelenmiştir:

| Modül / Klasör | Açıklama | Temel Kullanım Alanları |
| :--- | :--- | :--- |
| **`BMP180`** | Dijital Barometrik Basınç ve Sıcaklık Sensörü kütüphanesi. | Rakım (irtifa) tespiti, hava durumu istasyonları. |
| **`DHT11`** | Temel Sıcaklık ve Nem Sensörü kütüphanesi. | Ortam izleme, akıllı ev otomasyon sistemleri. |
| **`MPU6050`** | 6-Eksenli Ivmeölçer ve Jiroskop (IMU) kütüphanesi. | Açı tespiti, dengeleme sistemleri, yönelim (roll/pitch). |
| **`MPU9250`** | 9-Eksenli Gelişmiş IMU (Ivmeölçer + Jiroskop + Manyetometre) kütüphanesi. | İHA/UAV yönelim algoritmaları, hassas rota takibi. |
| **`NRF24L01+`**| 2.4GHz RF Kablosuz Haberleşme Modülü kütüphanesi. | Uzaktan kumanda sistemleri, kablosuz veri aktarımı. |
| **`QMC5883`** | 3-Eksenli Dijital Pusula (Manyetometre) kütüphanesi. | Manyetik kuzey tespiti, yön bulma ve navigasyon. |

---

## ⚙️ Kurulum ve Projeye Dahil Etme

Bu repodaki sürücüler, **STM32 HAL (Hardware Abstraction Layer)** kütüphaneleri kullanılarak geliştirilmiştir. Sürücüleri kendi projenize entegre etmek ve donanım mimarisini (örneğin STM32CubeIDE) hazır hale getirmek için aşağıdaki adımları takip edebilirsiniz:

### 1. Dosya Düzeni ve Entegrasyon
* Kullanmak istediğiniz modülün içerisindeki `.h` uzantılı başlık (Header) dosyasını, projenizin **`Core/Inc`** dizinine kopyalayın.
* `.c` uzantılı kaynak (Source) dosyasını ise projenizin **`Core/Src`** dizinine kopyalayın.

### 2. Donanım ve Çevre Birimi (Peripheral) Konfigürasyonu
* Kütüphaneler alt yapıda HAL fonksiyonlarını kullandığı için, CubeMX veya Device Configuration (`.ioc`) arayüzü üzerinden ilgili çevre birimlerini (**I2C, SPI veya GPIO**) aktif etmeniz gerekir.
* Örneğin; `MPU6050` kütüphanesini kullanacaksanız, STM32 üzerinde ilgili I2C birimini (I2C1, I2C2 vb.) projenizde başlatmış (Initialize) olmalısınız.

### 3. Koda Dahil Etme (Include)
* Sürücü fonksiyonlarını ve register tanımlamalarını ana kodunuzda (`main.c` veya kendi yazdığınız uygulama katmanında) kullanabilmek için dosyanın en üstüne ilgili başlığı ekleyin:
  ```c
  #include "modul_adi.h"
