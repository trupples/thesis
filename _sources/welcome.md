# Welcome to my thesis project

This [Jupyter Book](https://jupyterbook.org/) contains notes, [logs](./log.md), and the [current draft]() for my Bachelor's Thesis, **Multi-channel electromyogram data acquisition system with iio, ROS bindings**.

## Concept

Electromyography (EMG) is the measurement of electrical signals in skeletal muscles. Last year, I worked with the very neat [QNET Myoelectric board for NI Elvis](https://www.quanser.com/wp-content/uploads/2017/03/QNET-Myoelectric-Datasheet-v1.0.pdf) for my project in Control Engineering 2. While it was a very interesting exercise, it was also very limited in that it only had one electrode, which meant you couldn't even distinguish the direction of motion, just whether a particular muscle group is active (contracting) or not (either not moting or relaxing). As such, I set out to create a board with more electrodes and even better software support.


### Stakeholders

To somewhat explain how these requirements came to be, I'd like to also point out the *stakeholders* of this project:

- me, I want to build cool shit and optionally get a good grade off of it. I expect this to be a reproducible, budget-friendly (uh-oh), suckless project, which could potentially become a lab prop.
- [Tassos Natsakis](https://natsakis.com), my thesis supervisor, who has a very useful biomedical engineering background, and furthermore expects me to integrate this system with [ROS]()
- [Analog Devices](https://analog.com), my employer, who allows me to spend company time on this project only as long as it becomes a Circuit Note, Demo Project or Workshop they can further use. This implies using at least some latest-generation chips so they can show these off (uh-oh, can't be both budget friendly and do
this).
- [TUCN](https://utcluj.ro), my University, which sets [requirements for the thesis]() and are otherwise a superposition of chill and vengeful lunatics.

### How is this project valuable

Some main areas I'd like to focus on as guiding principles are:

- Maintainability:
	- repairability / DIY-ability (no advanced manufacturing techniques)
	- Openness (fully open-source + well documented)
- Ease of use, especially the software APIs
- Measurement quality (channels, sample rate, resolution)
- Cost (if I can't optimize IC cost, at least do it for consumables, PCB, mechanical elements, etc.)

### Key parts of the thesis

(to be refined...)

- EMG anatomy
- Medical instrumentation
- Regurgitate electronics basics
- Grand Purpose:tm:
- Electrical design
    - Specifications
    - Part choice
    - PCB Design
    - (ideally) a fully functional PCB
    - electrode choice
    - connector choice
- Firmware
- Software
    - iio driver
    - pyadi-iio class (easypeasy)
    - ROS provides

```{tableofcontents}
```
