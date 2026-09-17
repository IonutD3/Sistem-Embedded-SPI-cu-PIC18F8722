# Sistem Embedded de Comunicație SPI cu PIC18F8722

---

# 🇷🇴 Română

## Descriere

Proiectul prezintă implementarea comunicației **SPI (Serial Peripheral Interface)** pe microcontrolerul **PIC18F8722**, dezvoltată progresiv prin patru aplicații.

Prima etapă implementează comunicația SPI de bază și transferul unor coduri pentru cifrele `0–9`. În etapele următoare, comunicația SPI este utilizată pentru controlul unui **MCP23S17 GPIO Expander**, care asigură interfațarea cu un **LCD 16x2**. Ultima aplicație combină controlul LCD-ului cu transmiterea unor coduri pentru un **afișaj 7-segment** și afișarea datelor recepționate pe `PORTD`.

Codul este scris în **C pentru microcontrolere PIC**, utilizând direct registrele perifericului SPI/SSP și porturile GPIO. Pentru fiecare etapă este inclusă o schemă de simulare realizată în **Proteus**.

---

## 🔧 Componente și tehnologii

- **PIC18F8722**
- **Embedded C**
- **SPI / MSSP**
- **MCP23S17 GPIO Expander**
- **LCD 16x2**
- **7-segment display**
- **GPIO / hardware registers**
- **Proteus**
- **XC8 / MPLAB**

---

## 📌 Etapele proiectului

### 1. Comunicație SPI de bază

Prima aplicație configurează PIC18F8722 ca **SPI Master** și transmite succesiv codurile corespunzătoare cifrelor `0–9`.

Tabelul utilizat pentru codurile cifrelor este:

```c
unsigned ch[] = {
    0x3f, 0x06, 0x5b, 0x4f, 0x66,
    0x6d, 0x7d, 0x07, 0x7f, 0x6f
};
```

Pentru fiecare valoare, programul efectuează un transfer prin registrul `SSPBUF`. Valoarea returnată de transfer este stocată în `data1` și ulterior scrisă pe `PORTD`.

SPI este configurat cu:

- `Fosc = 4 MHz`
- clock SPI: `Fosc/64`
- `CKE = 1`
- `CKP = 0`
- `SMP = 0`

Funcțiile principale implementate sunt:

```c
SPI_init()
SPI_open()
SPI_close()
SPI_write()
SPI_read()
```

Această etapă reprezintă baza pentru aplicațiile următoare.

---

### 2. SPI + MCP23S17 + LCD 16x2

A doua aplicație introduce **MCP23S17**, utilizat ca GPIO expander pentru controlul LCD-ului.

Comunicația cu MCP23S17 este realizată prin SPI. Pentru scrierea unui registru sunt transmise succesiv:

```text
0x40
↓
Register Address
↓
Register Value
```

Selectarea circuitului MCP23S17 este realizată prin `RA2`, utilizat ca **Chip Select**.

Sunt utilizate registrele:

```c
IODIRA = 0x00
IODIRB = 0x01
GPIOA  = 0x12
GPIOB  = 0x13
```

Porturile A și B ale MCP23S17 sunt configurate ca ieșiri, după care sunt utilizate pentru interfațarea LCD-ului.

Controlul LCD-ului este organizat prin funcțiile:

```c
LCD_init()
LCD_cmd()
LCD_out()
```

Aplicația afișează succesiv caracterele:

```text
0 1 2 3 4 5 6 7 8 9
```

pe prima linie a LCD-ului.

---

### 3. SPI + MCP23S17 + deplasarea pe LCD + Afișarea unui text scrolling

A treia aplicație extinde controlul LCD-ului prin introducerea lucrului cu **șiruri de caractere**.

Mesajul utilizat în aplicație este:

```text
Mesaj SPI SPI SPI SPI SPI
```

Deoarece mesajul depășește lățimea vizibilă a LCD-ului, programul utilizează comanda:

```c
LCD_cmd(0b00011000);
```

pentru deplasarea conținutului afișat.

Sunt implementate două funcții pentru afișarea unui șir:

```c
write_mesaj()
write_mes()
```

A doua variantă utilizează un pointer către șir:

```c
void write_mes(char *c, int linie, int coloana)
{
    while(*c)
        LCD_out(*c++, linie, coloana++);
}
```

Lungimea mesajului este determinată cu `strlen()`.

Această etapă combină controlul hardware al LCD-ului cu concepte de programare C precum:

- string-uri;
- pointeri;
- `strlen()`;
- parcurgerea unui șir de caractere;
- transmiterea succesivă a caracterelor către LCD.

---

### 4. SPI + MCP23S17 + LCD + 7-Segment

Ultima aplicație reunește funcționalitățile dezvoltate anterior.

Programul:

1. configurează `PORTD` ca ieșire;
2. inițializează MCP23S17;
3. inițializează interfața SPI;
4. inițializează LCD-ul;
5. afișează mesajul `Mesaj SPI SPI SPI SPI SPI`;
6. deplasează conținutul LCD-ului pentru partea de mesaj care depășește zona vizibilă;
7. transmite codurile pentru cifrele `0–9` prin SPI;
8. stochează valoarea recepționată în `data1`;
9. scrie valoarea recepționată pe `LATD`;
10. afișează caracterele `0–9` pe LCD, începând de la coloana 5.

Codurile pentru afișajul 7-segment sunt definite în program:

```c
unsigned ch1[] = {
    0x3f, 0x06, 0x5b, 0x4f, 0x66,
    0x6d, 0x7d, 0x07, 0x7f, 0x6f
};
```

În această etapă sunt utilizate atât funcțiile de comunicație SPI, cât și funcțiile de control MCP23S17/LCD dezvoltate în etapele precedente.

---

## ⚙️ Configurarea SPI

În aplicațiile care utilizează SPI sunt configurate direct registrele modulului MSSP:

```c
SSPM3 = 0;
SSPM2 = 0;
SSPM1 = 1;
SSPM0 = 0;

CKE = 1;
CKP = 0;
SMP = 0;

SSPEN = 1;
```

Modulul este utilizat în modul **SPI Master**, cu frecvența de clock setată la `Fosc/64`.

Pentru `Fosc = 4 MHz`:

```text
SPI Clock = 4 MHz / 64
          = 62.5 kHz
```

Liniile SPI utilizate în aplicațiile care configurează explicit interfața completă sunt:

| SPI signal | PIC18F8722 |
|---|---|
| SCK | RC3 |
| SDI | RC4 |
| SDO | RC5 |
| CS | RA2 |

---

## 🔌 MCP23S17

MCP23S17 este controlat prin SPI și este utilizat pentru a furniza liniile GPIO necesare LCD-ului.

Scrierea unui registru este realizată prin funcția:

```c
void GPIO_set(char address, char value)
{
    CS = 0;
    SPI_write(0x40);
    SPI_write(address);
    SPI_write(value);
    CS = 1;
}
```

Configurarea direcției porturilor este realizată prin:

```c
IODIR_set()
```

iar inițializarea circuitului prin:

```c
MCP_init()
```

În acest fel, controlul LCD-ului este realizat indirect:

```text
PIC18F8722
     │
     │ SPI
     ▼
 MCP23S17
     │
     │ GPIO
     ▼
  LCD 16x2
```

---

## 🖥️ LCD 16x2

Interfața LCD este implementată software prin funcțiile:

```c
LCD_init()
LCD_cmd()
LCD_out()
```

Poziționarea pe cele două linii utilizează:

```c
#define linie1_inc 0x80
#define linie2_inc 0xC0
```

`LCD_out()` primește caracterul, linia și coloana și transmite datele către LCD prin MCP23S17.

Pentru mesajele mai lungi decât zona vizibilă este utilizată comanda de shift:

```c
LCD_cmd(0b00011000);
```

---

## 🔢 7-Segment

Codurile utilizate pentru cifrele `0–9` sunt definite explicit în cod:

| Digit | Code |
|---:|---:|
| 0 | `0x3F` |
| 1 | `0x06` |
| 2 | `0x5B` |
| 3 | `0x4F` |
| 4 | `0x66` |
| 5 | `0x6D` |
| 6 | `0x7D` |
| 7 | `0x07` |
| 8 | `0x7F` |
| 9 | `0x6F` |

În ultima etapă, aceste valori sunt transmise prin SPI, iar valoarea recepționată este scrisă pe `LATD`.

---

## 📁 Structura proiectului

```text
Sistem-Embedded-SPI-cu-PIC18F8722/
│
├── README.md
│
├── 01-basic-spi/
│   ├── proteus/
│   │   └── project.pdsprj
│   └── src/
│       └── main.c
│
├── 02-spi-mcp23s17-lcd/
│   ├── proteus/
│   │   └── project.pdsprj
│   └── src/
│       └── main.c
│
├── 03-spi-mcp23s17-lcd-text/
│   ├── proteus/
│   │   └── project.pdsprj
│   └── src/
│       └── main.c
│
└── 04-spi-mcp23s17-lcd-7segment/
    ├── proteus/
    │   └── project.pdsprj
    └── src/
        └── main.c
```

Fiecare director conține o etapă independentă a proiectului și propria schemă Proteus.

---

## 🧪 Simulare

Fiecare dintre cele patru aplicații are asociat un proiect **Proteus**, astfel încât implementarea poate fi urmărită progresiv:

```text
01 → SPI de bază
     ↓
02 → SPI + MCP23S17 + LCD
     ↓
03 → LCD + text + scrolling
     ↓
04 → LCD + MCP23S17 + 7-Segment
```

---

## 🎯 Concepte demonstrate

Proiectul oferă experiență practică în:

- programare **Embedded C**;
- utilizarea microcontrolerului **PIC18F8722**;
- configurarea perifericului **MSSP/SPI**;
- lucrul direct cu registre hardware;
- SPI Master communication;
- transmiterea și recepția datelor prin `SSPBUF`;
- controlul unui **Chip Select**;
- utilizarea **MCP23S17** prin SPI;
- configurarea registrelor `IODIRA` și `IODIRB`;
- controlul unui **LCD 16x2**;
- poziționarea și afișarea caracterelor;
- scrolling pentru mesaje mai lungi;
- lucrul cu string-uri și pointeri în C;
- generarea codurilor pentru un afișaj **7-segment**;
- utilizarea GPIO;
- simularea circuitelor în **Proteus**.

---

# 🇬🇧 English

## Description

This project demonstrates **SPI (Serial Peripheral Interface)** communication on the **PIC18F8722 microcontroller**, developed progressively through four embedded applications.

The first stage implements basic SPI communication and transfers digit codes for `0–9`. The following stages use SPI to control an **MCP23S17 GPIO Expander**, which provides the GPIO interface required for a **16x2 LCD**. The final application combines LCD control with SPI transmission of **7-segment digit codes** and outputs the received transfer data to `PORTD`.

The firmware is written in **C for PIC microcontrollers**, using direct hardware-register configuration of the SPI/MSSP peripheral and GPIO ports. Each stage includes a corresponding **Proteus simulation project**.

---

## 🔧 Technologies & Hardware

- **PIC18F8722**
- **Embedded C**
- **SPI / MSSP**
- **MCP23S17 GPIO Expander**
- **16x2 LCD**
- **7-segment display**
- **GPIO / hardware registers**
- **Proteus**
- **XC8 / MPLAB**

---

## 📌 Project Stages

### 1. Basic SPI Communication

The first application configures the PIC18F8722 as an **SPI Master** and sequentially transfers the codes corresponding to digits `0–9`.

The digit-code table is:

```c
unsigned ch[] = {
    0x3f, 0x06, 0x5b, 0x4f, 0x66,
    0x6d, 0x7d, 0x07, 0x7f, 0x6f
};
```

For each value, the application performs an SPI transfer through `SSPBUF`. The returned value is stored in `data1` and subsequently written to `PORTD`.

SPI configuration:

- `Fosc = 4 MHz`
- SPI clock: `Fosc/64`
- `CKE = 1`
- `CKP = 0`
- `SMP = 0`

Main SPI functions:

```c
SPI_init()
SPI_open()
SPI_close()
SPI_write()
SPI_read()
```

This application provides the SPI foundation used by the following stages.

---

### 2. SPI + MCP23S17 + 16x2 LCD

The second application introduces the **MCP23S17 GPIO expander**, which is controlled through SPI and used to interface with the LCD.

MCP23S17 register writes are performed by transmitting:

```text
0x40
↓
Register Address
↓
Register Value
```

The MCP23S17 chip-select signal is controlled through `RA2`.

The following registers are used:

```c
IODIRA = 0x00
IODIRB = 0x01
GPIOA  = 0x12
GPIOB  = 0x13
```

The MCP23S17 ports are configured as outputs and then used for LCD control.

LCD functionality is organized through:

```c
LCD_init()
LCD_cmd()
LCD_out()
```

The application sequentially displays:

```text
0 1 2 3 4 5 6 7 8 9
```

on the first LCD line.

---

### 3. SPI + MCP23S17 + LCD + Text Scrolling

The third application extends the previous implementation with **string handling** and LCD text output.

The message used by the application is:

```text
Mesaj SPI SPI SPI SPI SPI
```

Because the message exceeds the visible LCD width, the application uses:

```c
LCD_cmd(0b00011000);
```

to shift the display content.

Two string-output functions are implemented:

```c
write_mesaj()
write_mes()
```

The pointer-based implementation is:

```c
void write_mes(char *c, int linie, int coloana)
{
    while(*c)
        LCD_out(*c++, linie, coloana++);
}
```

The message length is determined using `strlen()`.

This stage combines hardware interfacing with C concepts such as:

- strings;
- pointers;
- `strlen()`;
- character-by-character string traversal;
- LCD character output.

---

### 4. SPI + MCP23S17 + LCD + 7-Segment

The final application combines the functionality developed in the previous stages.

The program:

1. configures `PORTD` as an output;
2. initializes the MCP23S17;
3. initializes the SPI interface;
4. initializes the LCD;
5. displays the `Mesaj SPI SPI SPI SPI SPI` message;
6. shifts the LCD display when the message exceeds the visible area;
7. transmits digit codes `0–9` through SPI;
8. stores the received transfer value in `data1`;
9. writes the received value to `LATD`;
10. displays the characters `0–9` on the LCD starting at column 5.

The 7-segment digit codes are explicitly defined in the source:

```c
unsigned ch1[] = {
    0x3f, 0x06, 0x5b, 0x4f, 0x66,
    0x6d, 0x7d, 0x07, 0x7f, 0x6f
};
```

This stage combines the SPI, MCP23S17 and LCD functionality with the 7-segment data-transfer example.

---

## ⚙️ SPI Configuration

The applications configure the PIC18F8722 MSSP peripheral directly through its hardware registers:

```c
SSPM3 = 0;
SSPM2 = 0;
SSPM1 = 1;
SSPM0 = 0;

CKE = 1;
CKP = 0;
SMP = 0;

SSPEN = 1;
```

The SPI peripheral operates in **Master mode** with a clock frequency of `Fosc/64`.

With `Fosc = 4 MHz`:

```text
SPI Clock = 4 MHz / 64
          = 62.5 kHz
```

SPI pins explicitly configured in the applications using the complete SPI interface:

| SPI signal | PIC18F8722 |
|---|---|
| SCK | RC3 |
| SDI | RC4 |
| SDO | RC5 |
| CS | RA2 |

---

## 🔌 MCP23S17

The MCP23S17 is controlled over SPI and provides the GPIO lines used by the LCD.

Register writes are implemented through:

```c
void GPIO_set(char address, char value)
{
    CS = 0;
    SPI_write(0x40);
    SPI_write(address);
    SPI_write(value);
    CS = 1;
}
```

GPIO direction is configured through:

```c
IODIR_set()
```

and the device is initialized through:

```c
MCP_init()
```

The resulting interface can be represented as:

```text
PIC18F8722
     │
     │ SPI
     ▼
 MCP23S17
     │
     │ GPIO
     ▼
  16x2 LCD
```

---

## 🖥️ 16x2 LCD

The LCD interface is implemented through:

```c
LCD_init()
LCD_cmd()
LCD_out()
```

The two LCD line addresses are defined as:

```c
#define linie1_inc 0x80
#define linie2_inc 0xC0
```

`LCD_out()` receives a character, line and column and transfers the corresponding data to the LCD through the MCP23S17.

Longer messages use the LCD display-shift command:

```c
LCD_cmd(0b00011000);
```

---

## 🔢 7-Segment Display Codes

The source code defines the following digit codes:

| Digit | Code |
|---:|---:|
| 0 | `0x3F` |
| 1 | `0x06` |
| 2 | `0x5B` |
| 3 | `0x4F` |
| 4 | `0x66` |
| 5 | `0x6D` |
| 6 | `0x7D` |
| 7 | `0x07` |
| 8 | `0x7F` |
| 9 | `0x6F` |

In the final stage, these values are transmitted through SPI and the received transfer value is written to `LATD`.

---

## 📁 Project Structure

```text
Sistem-Embedded-SPI-cu-PIC18F8722/
│
├── README.md
│
├── 01-basic-spi/
│   ├── proteus/
│   │   └── project.pdsprj
│   └── src/
│       └── main.c
│
├── 02-spi-mcp23s17-lcd/
│   ├── proteus/
│   │   └── project.pdsprj
│   └── src/
│       └── main.c
│
├── 03-spi-mcp23s17-lcd-text/
│   ├── proteus/
│   │   └── project.pdsprj
│   └── src/
│       └── main.c
│
└── 04-spi-mcp23s17-lcd-7segment/
    ├── proteus/
    │   └── project.pdsprj
    └── src/
        └── main.c
```

Each directory contains an independent project stage together with its corresponding Proteus schematic.

---

## 🧪 Simulation

Each of the four applications includes a **Proteus** project, allowing the implementation to be followed progressively:

```text
01 → Basic SPI
     ↓
02 → SPI + MCP23S17 + LCD
     ↓
03 → LCD + text + scrolling
     ↓
04 → LCD + MCP23S17 + 7-segment
```

---

## 🎯 Concepts Demonstrated

- **Embedded C** programming;
- PIC18F8722 microcontroller programming;
- MSSP/SPI peripheral configuration;
- direct hardware-register manipulation;
- SPI Master communication;
- data transmission and reception through `SSPBUF`;
- Chip Select control;
- MCP23S17 control over SPI;
- MCP23S17 `IODIRA` and `IODIRB` configuration;
- 16x2 LCD interfacing;
- LCD cursor positioning and character output;
- LCD display shifting;
- C strings and pointers;
- 7-segment digit-code generation;
- GPIO control;
- Proteus-based embedded-system simulation.

---

## 👤 Autor / Author

**IonutD**
