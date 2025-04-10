<a id="readme-top"></a>

# B31DG Assignment 2

- [Project Overview](#project-overview)  
  - [Built With](#built-with)  
- [Repository Structure](#repository-structure)    
- [Contact](#contact)  

## Project Overview
This project is an **ESP32-based signal control system** designed to handle input interrupts and generate pulse waveforms. It utilizes GPIO for input buttons and output signals, with an efficient **interrupt-driven** approach to manage button presses and control LED signaling.

<p align="right">(<a href="#readme-top">back to top</a>)</p>

## About The Project
The project follows the requirements of B31DG Assignment 1. The system utilises GPIO pins on an ESP32-WROOM module, the system was first implemented in the Arduino ide in a .ino file and then was followed by implementing the system using the ESP-IDF extension with VSCode. The system generates two waveforms utilising two GPIO pins for LEDs and a further two GPIO pins for input buttons. The DATA signal is defined by the calculations below and the SYNC signal pulses at the beginning of each DATA signal.

The alternate signal was a reversed waveform of the original signal, this can be changed by using the SELECT button defined by the program. The ENABLE button can also be used to toggle the DATA signal on and off.


| **Parameter**           | **Numerical Mapping**      | **Calculation**                     |
|------------------------|---------------------------|-------------------------------------|
| **a** Initial DATA pulse on time    | 'H' maps to 8             | 8 × 100µs = **800µs**               |
| **b** DATA pulse off time             | 'O' maps to 12            | 12 × 100µs = **1200µs**             |
| **c** Number of Pulses                 | 'L' maps to 12            | 12 + 4 = **16**                     |
| **d** Idle Time                 | 'M' maps to 13            | 13 × 500µs = **6500µs**             |
| **Alternative Behaviour** | 'A' maps to 1        | (1 % 4) + 1 = **2**                 |
| **T<sub>ON(n)</sub>** DATA pulse width | N/A                       | 800 + ((n - 1) × 50µs) for n ≥ 2   |

<p align="right">(<a href="#readme-top">back to top</a>)</p>

### Built With

The following frameworks were used to build this project:

* [![Arduino][Arduino-logo]][Arduino-url]
* [![ESP-IDF][ESP-IDF-logo]][ESP-IDF-url]

[Arduino-logo]: https://img.shields.io/badge/Arduino-00979D?style=for-the-badge&logo=arduino&logoColor=white
[ESP-IDF-logo]: https://img.shields.io/badge/ESP--IDF-FF0000?style=for-the-badge&logo=espressif&logoColor=white

[Arduino-url]: https://www.arduino.cc/
[ESP-IDF-url]: https://idf.espressif.com/

<p align="right">(<a href="#readme-top">back to top</a>)</p>

## Repository Structure

The repository is split mainly into doc and src files. The src files contain both the arduino .ino code and espressif .c and .h files for the working waveform. The doc files contain documents in relation to the assignment.

* [docs/](../docs)  # related documents
  * [B31DG - Assignment 2 - H00368728 - Fraser Holman.docx](/docs/B31DG%20-%20Assignment%202%20-%20H00368728%20-%20Fraser%20Holman.docx)  # assignment report
  * [Cyclic Executive Design](/docs/CyclicExecutiveDesign.xlsx)
  * [Standard Declaration of Student Authorship Word Document](/docs/Standard%20Declaration%20of%20Student%20Authorship%20-HWU.docx)
  * [Standard Declaration of Student Authorship PDF](/docs/Standard%20Declaration%20of%20Student%20Authorship%20-HWU.pdf)
* [src/](../src)    # source files
  * [Cyclic Executive Firmware/](/src/Cyclic)
    * [main.ino/](/src/Cyclic/Cyclic.ino)  
  * [RTOS Firmware/](/src/RTOS)
    * [main.ino/](/src/RTOS/RTOS.ino)
* [README.md](README.md)  # read me

<p align="right">(<a href="#readme-top">back to top</a>)</p>

## Contact

Fraser Holman
fjah2000@hw.ac.uk
H00368728

<p align="right">(<a href="#readme-top">back to top</a>)</p>
