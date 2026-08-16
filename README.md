\# STM32 Instrument Console



A modular UART command-line console developed for the \*\*NUCLEO-F446RE\*\* using C and STM32 HAL.



This project is the first stage of an embedded instrumentation platform that will later include analog signal acquisition, measurements, and configurable event detection.



\## Current Status



The UART console infrastructure is implemented and tested on hardware.



Current features:



\- USART2 communication at 115200 baud

\- UART reception using interrupts

\- Character-by-character input processing

\- Line receive buffer

\- Enter detection

\- Backspace and Delete handling

\- Prompt protection

\- Input overflow detection and recovery

\- Modular console implementation

\- Cooperative superloop architecture



Current terminal behavior:



```text

STM32 Instrument Console

> hello

Received: hello

>

```



The `Received:` response is temporary and will be replaced by the command parser.



\## Hardware



\- NUCLEO-F446RE

\- Integrated ST-LINK Virtual COM Port

\- User LED LD2

\- FNIRSI oscilloscope and signal generator for future experiments



\## Software



\- STM32CubeIDE

\- STM32 HAL

\- C

\- Git and GitHub

\- PuTTY



\## Architecture



The application initializes and continuously processes the console from `main.c`:



```c

Console\\\_Init(\\\&huart2);



while (1)

{

\&#x20;   Console\\\_Process();

}

```



The UART reception callback redirects the event to the console module:



```c

void HAL\\\_UART\\\_RxCpltCallback(UART\\\_HandleTypeDef \\\*huart)

{

\&#x20;   Console\\\_RxCpltCallback(huart);

}

```



\*roject organization\*



\*``text

Core/

├── Inc/

│   ├── console.h

│   └── ...

└── Src/

&#x20;   ├── console.c

&#x20;   ├── main.c

&#x20;   └── ..\*

```



The console\\\*module keeps its UART state, receive buffer, flags, and input-process\\\*ng logic private inside `console.c.



\\## Serial Configuration



Use the ST-LINK Virtual COM Port with:



Baud rate:    115200

Data bits:    8

Parity:       None

Stop bits:    1

Flow control: None



For PuTTY:



Local echo: Force off

Local line editing: Force off



The STM32 performs the character echo.



\\## Build and Run

1\\. Open the project in STM32CubeID.

2\\. Build the project using Ctrl + B.

3\\. Connect the NUCLEO-F446RE through the ST-LINK USB connector. 4. Program the MCU using Run or Debug.

5\\. Open the ST-LINK Virtual COM Port at 115200 8N1.



\\## Next\\\*Milestone



The next development stage is the command interface:



\\- \\\[ ] Command parser

\\- \\\[ ] Command dispatch table

\\- \\\[ ] help

\\- \\\[ ] status

\\- \\\[ ] version

\\- \\\[ ] Modular LED driver

\\- \\\[ ] led on

\\- \\\[ ] led off

\\- \\\[ ] led toggle

\\- \\\[ ] led status



Future states will include ADC measurements, timer-controlled sampling, DMA, signal analysis, and triggered event acquisition.



\\## Author



\\\*\\\*Samuel David Medina Contreras\\\*\\\*



Electronic Engineering student focused on firmware, embedded systems, data acquisition, and scientific instrumentation.




