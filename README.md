<a id="readme-top"></a>

# B31DG Assignment 2

- [Project Overview](#project-overview) 
- [Repository Structure](#repository-structure)    
- [Contact](#contact)  

## Project Overview
The project follows the requirements of B31DG Assignment 2. The project explores the comparison between a Cyclic Executive Design and RTOS System to fulfil a Real Time System constraints. The coursework introduces 7 different requirements which can each represent its own task which needs to be implemented into a Real Time System. The 5 main tasks that rely on the Real Time System constraints can be seen in the following table which the coursework will be mainly based around.

| Task    | Method            | Pi (ms) | Ci (ms) |
|---------|-------------------|--------:|--------:|
| Task 1  | `digitalSig1()`   |       4 |   0.605 |
| Task 2  | `digitalSig2()`   |       3 |   0.355 |
| Task 3  | `getF1()`         |      10 |   1.501 |
| Task 4  | `getF2()`         |      10 |   1.200 |
| Task 5  | `monitorProgram()`|       5 |   0.502 |

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

* [docs/](/docs)  # related documents
  * [B31DG - Assignment 2 - H00368728 - Fraser Holman.docx](/docs/B31DG%20-%20Assignment%202%20-%20H00368728%20-%20Fraser%20Holman.docx)  # assignment report
  * [Cyclic Executive Design](/docs/CyclicExecutiveDesign.xlsx)
  * [Standard Declaration of Student Authorship Word Document](/docs/Standard%20Declaration%20of%20Student%20Authorship%20-HWU.docx)
  * [Standard Declaration of Student Authorship PDF](/docs/Standard%20Declaration%20of%20Student%20Authorship%20-HWU.pdf)
* [src/](/src)    # source files
  * [Cyclic Executive Firmware/](/src/Cyclic)
    * [Cyclic Executive ino File](/src/Cyclic/Cyclic.ino)  
  * [RTOS Firmware/](/src/RTOS)
    * [RTOS ino File](/src/RTOS/RTOS.ino)
* [README.md](README.md)  # read me

<p align="right">(<a href="#readme-top">back to top</a>)</p>

## Contact

Fraser Holman
fjah2000@hw.ac.uk
H00368728

<p align="right">(<a href="#readme-top">back to top</a>)</p>
