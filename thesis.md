# Multi-channel biopotential DAQ with ROS2, IIO bindings

TODO abstract

# Introduction

Biopotential measurements are ...
- medical relevance:
    - diagnosis
    - rehab
    - monitoring
    - ???
- Novel human-computer interaction methods

Various projects tackle the problem of biopotential data acquisition:
- openBCI: expensive, low data rate
- that one from Mark: {TODO}
- quanser board: depends on expensive ELVIS, one channel, not open
- EXG pills: one channel, not a stand-alone product, but they're quite good for prototyping otherwise
- TODO more

## Objectives

MUST:
- [ ] Open Hardware and Software
- [ ] 8 channels
- [ ] Per-channel configuration
- [ ] 1000Hz data rate (500Hz bandwidth)
- [ ] High CMRR, PSRR (<1LSB after 50Hz filtering)
- [ ] High SNR (>40dB EMG; >20dB for ECG)
- [ ] Open and extensible interfacing:
    - [ ] ROS2
    - [ ] IIO

SHOULD (unchecked go to future work):
- [ ] 16 channels
- [ ] Differential / single-ended configurability
- [ ] Right Leg Drive
- [ ] 1024Hz/2048Hz /channel data rate (1 Hz per FFT bin)
- [ ] <1uV noise after filtering
- [ ] 1uV precision

Guiding principles:
- [ ] Cost
- [ ] Usability as an educational tool
- [ ] ADI parts
- [ ] Modularity

TODO: After finishing the prototype, make a comparison table between the already existing products and mine, especially on these objectives.

# Bibliographic research

(current state of the art + problem justification + possible approaches)

Although the electrical design of biopotential measurement devices has been studied for hundreds {cite???} of years, and there exist a plethora of commercial and DIY solutions, very few systems aspire to the ideals of Open Source Hardware and Open Source Software (EMG pill, openBCI, the one from Mark), and there exist no systems that integrate with ROS2 (ros-neuro is ROS1 and they don't define hardware, just ros infrastructure) or the Linux IIO (Industrial I/O) subsystem.

Of the Open Source projects, none check all our requirements, and as such this project fills in this gap and provides an end-to-end hardware and software solution for measuring multi-channel biopotential signals and exposing them to ROS2 and IIO. Adapting this system to work with other software suites should be made as simple as possible thanks to the versatility of IIO, and adapting it to other hardware platforms should also be facilitated by the decoupled nature of the hardware and software parts.

## Anatomical basis

EMG, skeletal muscles:
Sarcomere, sliding filament model

ECG:
???

## Noise sources

TODO: Thermal noise model for baseline noise.
TODO: ECG noise models
TODO: do EMG noise models exist?

## Signal chain gain management

Stage N noise should be >= stage N+1 noise, just barely passing it, to get the most out of all stages. I should really find a citation for this, because I just know it from Mark. We can use this relationship, some noise density models, and the guaranteed datasheet figures of the components used in the signal chain to compute the ideal amplifier stage gain.


# Analysis, design and implementation

## System architecture

TODO: Signal chain figure:
1. Patient, anatomy
2. Electrodes
3. Buffer and amplification
4. ADC
5. Microcontroller, using the no-OS framework. Contains customizable digital filters
6. IIO protocol
7. Linux IIO subsystem
8. libiio, pyadi-iio
variant A, with a network-enabled microcontroller:
from 5, ROS2 protocol {find the name!} to ROS2 network running on the microcontroller
variant B, with a microcontrollwe without networking capabilities:
from 8, ROS2 protocol to ROS2 network running on the carrier board / host

## Electical part

### Interfacing with the {patient}

The first step we control in the signal chain is the point of contact with the patient. The electrode type and quality have a great influence on the overall design, performance, and subjective user experience.

Leading research: PEDOT:PSS {cite, cite, cite!}. Flexible, self-healing polymer, conforms to skin ensuring very good contact even when in motion, 

DIY cheap alternative {cite that paper with DIY AgCl electrodes}

This project settles for clip-on adhesive ECG gel electrodes because:
- Cheap, readily avaialble, standard
- They can be interfaced with inexpensive aligator clips, besides special (but more expensive) snap clips
Downsides:
- Shelf life {TODO: compare expired electrode signal quality with fresh electrodes}
- Large-ish contact area
- Adhesive gives for a bad subjective experience {author's anecdote, or citeable? you zetetic asshole...}

### ADC

Initially chose the AD7124-8 thanks to its very diverse feature set, including: programmable gain amplifiers (which had the potential to remove the need for the external amplification stage), high precision, high number of channels, and high sample rate. Unfortunately, it is not a single cycle ADC. This means that although it has very good throughput on a single channel, after switching between two channels there is a dead time for the integrated filter to settle, which effectively reduces the per-channel sample rate to less than 100Hz when sampling all 16, which is basically useless for ECG and EMG {cite: most of the signal's useful information is in the 300Hz ballpark}

The next choice is the AD4114. It doesn't include PGAs, but otherwise has very good specs and has a mugh higher sample rate, and a single cycle conversion mode. {stai, sigur?}

TODO spec comparison between AD7124-8 and AD4114.

### Amplification & buffer stage

Ask whether they're needed. To figure this out, we'll check if we get better measurement noise with an amplification stage than without, because in preliminary tests, we got decent signals without any amplifier. Is "decent" as well as we can get it, or can we have *great* signal quality with an amp stage? Should be able to do these computations quite easily.

### Microcontroller

The chosen microcontroller platform is the: {Inca nu am ales......}
- ADARPblabla max arduino
- MAX78000FTHR
Both are overkill for the needs of this project.

Main requirements for the microcontroller:
- SPI rate high enough to read the ADC at the wanted sample rate
- Enough CPU freq & memory to run the IIO Daemon. #todo: iiod specs? if they don't exist, I could do a small experiment for this paper.

Nice-to-haves:
- Networking capabilities so it can run a ROS2 node as a standalone device, not needing another "carrier" to forward the data to the ROS network.

Suitable very cheap microcontrollers:
- ATMEGA328p series {TEST?}
- ESP8266, ESP32
- RP Pico wifi and clones (groundstudio pico)
I sure bloody hope no-OS has support for them......... TODO check

## Firmware part

### SPI Communication basic proof of concept (2024-06 single channel demo)

For initial validation of the concept on the AD4114, I wrote a basic {TODO write about the PoC}

### No-OS Driver

For easy portability to different microcontroller platforms and even other ADCs, I'm using the No-OS framework, which facilitates separating the general logic from product-specific code and settings.

### IIOd integration

Defining the structure of the IIO context is a very important step as it solidifies a mapping between the device-specific functionality on the hardware side and the application-specific functionality on the software side. Choosing a versatile collection of attributes to configure the device and channels to structure the data is no trivial task with no unique solution. The exposed attributes may encode very low level details, as in {find an AD part with a very bare-metal attribute mapping}, or more high level ones. Choosing which configuration is to be specified on the software side and which configuration is to be derived from that on the firmware side {of pula mea ce scriu aici... vreau sa zic ca nu o sa expun direct registri, ci o sa expun chestii mai high level: frecventa de mains hum (50/60), parametri de filtrare, poate sample rate daca o sa imi dau variante (posibil tho sa fie fix la 1k), si pe baza lor se ocupa firmware sa puna registrii cum trebuie.

For lower-level functionality, an IIO debug interface is also specified, which allows for direct manipulation of the ADC's registers. This shouldn't be normally used, but allows for total control of the device in case the user needs it.

1. Defining the desired iio context structure: what do we expose, in which formats, etc
2. Handler functions
3. Boom done, connect to it from software

### ROS2 on network-capable microcontrollers

In recent years, there has been a wave of developments in fast, network-capable, cheap microcontroller platforms, leading to products such as the Espressif ESP8266 and ESP32, or the Raspberry Pi Pico-W, which are capable of running ROS2 nodes all by themselves. 

TODO

### Digital filtering of the signal

Although not the main topic of this thesis, the chosen system architecture offloads most of the signal conditioning to the digital domain. The main source of noise is mains hum, with a fundamental frequency of 50Hz and various harmonics, which can all be removed using a notching filter, resulting in minimal loss of information in other portions of the frequency spectrum. The Qaulity factor $Q$ of the filter controls how narrow the attenuated frequency bands are, and it can be adjusted. It poses a tradeoff between good noise rejection and signal quality, and is designed to be adjustable. Experimentally, we found values of around 5-10 to give the best waveforms for visual inspection. Figure X illustrates the tradeoff between small Q doing ??? and high Q doing ???.

Taking from scipy's implementation of an IIR Notching filter (`scipy.signal.iircomb`), I also use the structure described in {cite: Sophocles J Orfanidis, "Introduction to Signal Processing", Prentice-Hall, 1996, ch. 11, "Digital Filter Design"} thanks to its adjustable Q factor, numerical stability even at higher orders, and default rejection of the DC component of the signal, which are all very useful for the required signal conditioning. Parametrized by the the sampling frequency $f_s$, the cutoff freqiency $f_0$ (which must be an integer divisor of $f_s$), quality factor $Q$, the IIR filter $H(z)$ is computed as:

\[
N = f_s / \omega_0 \overset{!}{\in} \mathbb{N}
\omega_0' = \frac{2\pi\omega_0}{f_s}
\omega_\Delta = \frac{\omega_0'}{Q}
\beta = \tan(N * \omega_\Delta / 4)
a = \frac{1 - \beta}{1 + \beta}

H(z) = \frac{1}{1 + \beta} \cdot \frac{1 - z^{-N}}{1-az^{-N}}
\]

TODO check I transcribed these correctly and actually implement them myself! I only blindly copied these for now.

## Software part

### ROS2 agent for non network-capable microcontrollers

In the case of microcontroller platforms that aren't network capable, such as the ATMEGA328P family popularized by Arduino, the MAX78000FTHR, the device itself won't have a means of directly connecting to the ROS2 network as a node. As such, it needs to pass along the data to another device that can. Two implementation variants are immediately evident:
- Using the micro-ROS framework, letting ros logic take over the serial device instead of having it communicate using the IIO protocol, and running a ROS-micro agent on the host
- Using the ros-control framework and an IIO-ROS2 adapter on the host / carrier board.

Of which I chose the second because it doesn't require reconfiguring the serial port and allows for simultaneous use of ROS and IIO.

TODO implement :)

### libiio, pyadi-iio

ROS is for robotics, what about just doing some basic signal processing in python without setting it up as a ros node? Libiio has got you covered! Pyadi-iio adds a cherry on top, exposing the entire system as a very simple to initialize and query python object.

TODO implement :)

## Results and validation

To test the performance of the system:

- ECG is more difficult and as such a greater achievement. It has smaller amplitudes and very well-defined features, requiring more care all across the signal chain. 
- EMG is the main goal of the project, but it is more forgiving, because:
    - The signal has greater amplitude (about 10x? check both sources and experimental data!!!)
    - The signal features are not as well defined and are usually modeled like noise for signal processing.

### Electrical validation

Using a waveform generator, we can compare this device's measurements with both:
- Precision oscilloscope readings
- Another biopotential DAQ system (Tassos')

TODO :)

### Usage as EMG

TODO :)

### Usage as ECG

TODO :)

# Conclusion

## Future work

# Acknowledgements

- Mark Thoren

# Bibliography



