# Log

The first like 6 months are very disorderly, with a lot of Romanian, these were just personal notes meant mostly to save interesting links. Please don't mind :)

## 2023-08-??

[AD8420 in-amp](https://www.analog.com/media/en/technical-documentation/data-sheets/AD8420.pdf)

si om mai vedea dupa.


## 2023-08-??

Given cum se degradeaza performantele la gain mare (as fi vrut initial 1000), cred ca ar fi mai sanatos un two-stage design. Prima data in-amp sa faca un 50-100x (mai spre 100 imo), dupa o filtrare cu pasive, dupa inca un amplificator sau direct ADC 


la multiplexed ADC, factori de considerat:
- pret
- sample rate (as vrea MACAR 1khz * num_channels)
- multiplex rate (iar, same) de ex din cauza asta cam pica LTC2444. Pe ideea asta, sunt nice alea cu channel sequencer, ca is facute sa citesti tot pe rand ciclic



caracatica cu 8 sloturi cu cate un ADC cu 8 canale each. Plus prefab 8 sensor modules, fie ca sunt grids, benzi, freeform, etc.

de citit cartea lui Tassos ca sa aleg intre single ended sau differential. Probabil raman la single ended lul

## 2023-08-08

[AD8293G160](https://www.analog.com/en/ad8293g160) e mega ieftin daca tot vreau sa fac un stage de gain mai cu bun simt la inceput, why not use this + bag si 2nd order low pass (datasheet, fig 19). Nu o sa am pasive degeaba :) Si dupa pe board-ul principal daca mai trebuie amp.


nu merita la pret sa iei mai multe amplificatoare in acelasi integrat

## 2023-08-09

https://www.analog.com/media/en/technical-documentation/technical-articles/ECG-EEG-EMG_FINAL.pdf

Neato microcontroller https://www.analog.com/en/products/adsp-bf592.html though apparently quite old

## 2023-08-10

https://www.nature.com/articles/s41467-021-25152-y
https://www.nature.com/articles/s41467-020-18503-8
PEDOT:PSS

da' cred ca o sa fac cu ceva relief de cupru electroplated cu argint si dupa maaaaaaaybe oxidat in clor da' nu ma incumat asa departe


## 2023-10-04

First meet with Tassos

Parts:
- intro: problem - lack of emg daq for ros
- bibliographic research: current stat of the art
- project objectives: find something missing in the state of the art and fix that

- [ ] #TODO written agreement from ADI for NDA messiness

look up: muscle contraction sliding filament mechanism, sarcomere


## 2023-10-??

FIRMWARE STUFF:
https://github.com/analogdevicesinc/libtinyiiod/
Put this on the microcontroller so that we may connect to it via iio serial!
TODO Test transfer rate (want thicc 16bits/sample * 32 channels * 2ksps = 128kbytes/s = 1.024Mbaud. Seems doable even with arduino uno (though I don't think it can send and process at the time, that might kill it, but more powerful uno-form-factor boards will be more than good enough)

"sinc" filter removes all multiples of X Hz -> mains hum :)

## 2023-10-??

https://www.frontiersin.org/articles/10.3389/felec.2021.700363/full

make my own electrodes?????
buy silver snap-on jewlery, then either bleach or electroplate it

https://ieeexplore.ieee.org/document/4121504

driven right leg

## 2024-01-15

https://stacks.stanford.edu/file/druid:nq977kw3386/aspen-platform-tools-iv_plus_title-augmented.pdf

![[assets/Pasted image 20240115170251.png]] i like this!


also, in recent days I have started questioning whether I should go for:
1. local amplifier + many wires (V+, V-, Vref, signal) back to main board (initial idea)
2. *shielded* single ended probe amplified on the main board

pros of second would be compatibility with already existing hardware, connectors, etc (snap connectors are single ended)

variant 1 ideas - considering shielded TRRS cables -> flexible, cheap, shielded variants exist. cons: non-standard for EMG

## 2024-03-01

could a ribbon connector be configured in such a way so that if it's mounted one way, we have single-ended amps, but otherwise we have differential neighbourin amps

idk but it's likely much easier to have a switch on each electrode (if active electrodes)

differential (with RLD) might just be overall superior. single-ended measurements can be approximated by cumulative sums, and with differential there are less amplification challenges.

## 2024-03-10

"Previous art" initial research (ddg):

[ROS-Neuro: An Open-Source Platform for Neurorobotics](https://doi.org/10.3389%2Ffnbot.2022.886050) - if this proves to be a de-facto standard, I should use its messages and structure. This project would neatly fall into the `rosneuro_acquisition` category. This would also allow for easier testing with other pre-built components.

[Development of an EMG-Controlled Mobile Robot](https://doi.org/10.3390/robotics7030036) - uses ROS and a [Thalmic Myo Armband (200usd)](https://wearabletech.io/myo-bracelet/)

[Q&A "Looking for existing Electromyograph EMG/EEG, or voltage messages"](https://answers.ros.org/question/306234/looking-for-existing-electromyograph-emgeeg-or-voltage-messages/) highlights the need for a common framework (-> rosneuro?)

[A Newly-Designed Wearable Robotic Hand Exoskeleton Controlled by EMG Signals and ROS Embedded Systems](https://www.mdpi.com/2218-6581/12/4/95) which seems to have [plagarized](https://www.mdpi.com/2218-6581/13/2/21) [EMG-driven hand model based on the classification of individual finger movements](https://doi.org/10.1016/j.bspc.2019.101834)

[Development of an EMG based SVM supported control solution for the PlatypOUs education mobile robot using MindRove headset](https://www.sciencedirect.com/science/article/pii/S2405896321016761) - TODO Check out

[A flexible architecture to enhance wearable robots: Integration of EMG-informed models](https://ieeexplore.ieee.org/document/7353997) uses ROS.

[Teleoperation Control of ROS-based Industrial Robot using EMG Signals](https://doi.org/10.14372/IEMEK.2020.15.2.87)

https://github.com/uts-magic-lab/ros_myo
https://github.com/dzhu/myo-raw
https://github.com/ruoshiwen/ros_gforce/

Also collected all pre-2024-03 log entries from the 2 obsidians and put them here.

## 2024-03...

## 2024-04...

## 2024-05...

!!! TODO collect 2024-03 2024-04 2024-05 notes !!!

## 2024-05-20

met with Tassos

TODO ros2, ros2\_control, hardware interface
#section hardware interface

#section validation against an existing device

## 2024-05-28

AD7124 not good enough :(

While it can sample 19200 sps on a single channel, and dividing that by 16 (or 8) would make it seem like we could easily measure 1024 sps per channel, switching channels incurs an extra dead time for the digital filter to settle. With this delay in mind, switching between 16 channels would lead to an effective 60-80 sps per channel depending on the setup. Oh no, I'll have to change the board... That's a big bummer, especially given the calendar.

## 2024-06-02

Will need to write about the difference between:
- iio on uc & serial + ros2\_control hardware interface on host (chosen variant)
- micro-ros on uc w/ serial custom transport + micro-ros-agent on host

Will have to ask about ros-lite and what the deal was with that, since I don't see it fitting into any part of the project.

