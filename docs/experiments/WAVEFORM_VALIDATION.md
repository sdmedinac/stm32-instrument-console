\# Controlled Waveform Acquisition Validation



\## Purpose



The purpose of these experiments was to validate the ability of the STM32 Instrumentation Platform to acquire controlled time-varying analog signals using hardware-timed ADC sampling and DMA.



The experiments evaluated whether the complete acquisition chain could preserve the waveform shape, amplitude, timing, periodicity, and sample order.



The validated acquisition path was:



```text

FNIRSI signal generator

&#x20;       |

&#x20;       v

Series current-limiting resistor

&#x20;       |

&#x20;       v

PA0 / ADC1\_IN0

&#x20;       |

&#x20;       v

ADC1 regular conversion

&#x20;       |

&#x20;       v

TIM2 TRGO sampling trigger

&#x20;       |

&#x20;       v

DMA2 Stream 0

&#x20;       |

&#x20;       v

Acquisition buffer

&#x20;       |

&#x20;       v

UART data export

```



The tests included:



\- Square wave

\- Sine wave

\- Triangle wave

\- Unipolar pulse



All waveforms were observed using the FNIRSI oscilloscope before being connected to the STM32 ADC.



\---



\# Test Equipment



The experiments used:



\- NUCLEO-F446RE

\- STM32F446RE microcontroller

\- FNIRSI signal generator

\- FNIRSI oscilloscope

\- Oscilloscope probe

\- Breadboard

\- Jumper wires

\- Series resistor between the generator and PA0

\- USB connection for ST-LINK and the Virtual COM Port

\- PuTTY serial terminal



\---



\# Electrical Connection



The signal-generator output was connected to PA0 through a series resistor.



```text

FNIRSI OUT

&#x20;   |

&#x20;   v

Series resistor

&#x20;   |

&#x20;   +-------- PA0 / A0 / ADC1\_IN0

&#x20;   |

&#x20;   +-------- Oscilloscope CH1 probe tip



FNIRSI GND

&#x20;   |

&#x20;   +-------- Nucleo GND

&#x20;   |

&#x20;   +-------- Oscilloscope probe ground

```



The oscilloscope was connected in parallel with PA0 so that the observed voltage corresponded to the signal reaching the ADC input.



The oscilloscope was configured using DC coupling to preserve the absolute voltage level and DC offset.



Before connecting each waveform to PA0, the following conditions were checked:



```text

Vmin greater than or equal to 0 V

Vmax lower than or equal to 3.3 V

Common ground between the generator, oscilloscope, and Nucleo board

```



The series resistor provided current limiting, but it was not considered a complete overvoltage or negative-voltage protection circuit.



\---



\# Common STM32 Configuration



Unless stated otherwise, the waveform tests used:



```text

Sampling frequency: 1000 Hz

Sampling period:    1 ms

Sample count:       512

Acquisition window: 512 ms

ADC resolution:     12 bits

ADC input:          PA0 / ADC1\_IN0

Transfer mode:      DMA

```



The approximate number of samples per signal period was calculated using:



```text

Samples per period =

sampling frequency / signal frequency

```



For a signal near 10 Hz:



```text

Samples per period =

1000 / 10



Samples per period =

approximately 100

```



The approximate number of periods inside the acquisition window was:



```text

Captured periods =

0.512 seconds x 10 Hz



Captured periods =

approximately 5.12

```



\---



\# Square-Wave Test



\## Generator Configuration



```text

Waveform:             Square

Configured frequency: 10 Hz

Duty cycle:           50 percent

Generator amplitude:  1.0 V

```



\## Oscilloscope Reference



```text

Measured Vmin:       0.02 V

Measured Vmax:       1.00 V

Measured Vpp:        0.98 V

Measured frequency:  10.08 Hz

Coupling:            DC

```



\## STM32 Configuration



```text

Sampling frequency: 1000 Hz

Sample count:       512

Acquisition window: 512 ms

```



\## STM32 Results



```text

Valid samples:      512

Minimum:            0 counts, 0.000 V

Maximum:            1232 counts, 0.993 V

Average:            601 counts, 0.484 V

Peak-to-peak:       1232 counts, 0.993 V

Acquisition state:  COMPLETE

Acquisition error:  NONE

```



\## Timing Analysis



The main rising edges appeared approximately every 100 samples.



```text

Approximate samples per period: 100

Approximate signal period:      100 ms

Estimated signal frequency:     10 Hz

Approximate captured periods:   5.12

```



The high and low regions lasted approximately 50 samples each, which was consistent with the configured 50 percent duty cycle.



\## Observations



\- The exported buffer contained two clearly separated voltage levels.

\- The high level was close to 1 V.

\- The low level was close to 0.02 V.

\- The rising and falling transitions repeated every approximately 100 samples.

\- The waveform matched the expected square-wave behavior.

\- The STM32 peak-to-peak result was close to the oscilloscope measurement.



\## Result



```text

Square-wave acquisition: PASS

```



\---



\# Sine-Wave Test



\## Generator Configuration



```text

Waveform:             Sine

Configured frequency: 10 Hz

Generator amplitude:  1.0 V

```



The generator produced a unipolar sine wave containing a positive DC offset.



The approximate signal was:



```text

Vmin: approximately 0.02 V

Vmax: approximately 1.00 V

Vpp:  approximately 0.98 V

```



\## STM32 Configuration



```text

Sampling frequency: 1000 Hz

Sample count:       512

Acquisition window: 512 ms

```



\## STM32 Results



A clean representative acquisition produced:



```text

Valid samples:      512

Minimum:            0 counts, 0.000 V

Maximum:            1188 counts, 0.957 V

Average:            598 counts, 0.482 V

Peak-to-peak:       1188 counts, 0.957 V

Acquisition state:  COMPLETE

Acquisition error:  NONE

```



\## Timing Analysis



The waveform maxima appeared approximately every 100 samples.



```text

Approximate samples per period: 100

Approximate signal period:      100 ms

Estimated signal frequency:     10 Hz

Approximate captured periods:   5.12

```



\## Observations



\- The samples changed gradually between the minimum and maximum.

\- The exported buffer clearly differed from the square-wave buffer.

\- The maxima and minima repeated with stable periodicity.

\- The acquisition contained approximately five complete waveform periods.

\- The measured peak-to-peak voltage was close to the oscilloscope reference.

\- Occasional brief downward glitches were observed during some measurements.

\- Similar glitches remained visible after the Nucleo board was disconnected.

\- The glitches disappeared when the oscilloscope input was connected directly to ground.

\- The intermittent disturbance was attributed to the signal-generation or measurement setup rather than to ADC, DMA, or acquisition-firmware behavior.

\- The anomalous samples were preserved without filtering.



\## Result



```text

Sine-wave acquisition: PASS

```



\---



\# Triangle-Wave Test



\## Generator Configuration



```text

Waveform:             Triangle

Configured frequency: 10 Hz

Generator amplitude:  1.0 V

```



An initial triangular-wave test showed an unstable minimum level and temporary lower clipping.



The final representative acquisition was repeated after the signal-generator output became more stable.



\## STM32 Configuration



```text

Sampling frequency: 1000 Hz

Sample count:       512

Acquisition window: 512 ms

```



\## STM32 Results



```text

Valid samples:      512

Minimum:            0 counts, 0.000 V

Maximum:            1188 counts, 0.957 V

Average:            598 counts, 0.482 V

Peak-to-peak:       1188 counts, 0.957 V

Acquisition state:  COMPLETE

Acquisition error:  NONE

```



\## Timing Analysis



The waveform maxima and minima repeated approximately every 100 samples.



```text

Approximate samples per period: 100

Approximate rising duration:    50 samples

Approximate falling duration:   50 samples

Approximate signal period:      100 ms

Estimated signal frequency:     10 Hz

Approximate captured periods:   5.12

```



The approximate digital slope magnitude was:



```text

1188 counts / 50 samples

approximately 23.8 counts per sample

```



At a 1 ms sampling period:



```text

approximately 23.8 counts per millisecond

```



\## Observations



\- The exported buffer contained nearly linear rising and falling segments.

\- The waveform was clearly distinguishable from the sine and square waves.

\- The average result was close to the midpoint of the measured range.

\- The final acquisition did not contain the prolonged lower clipping seen during the initial attempt.

\- A small number of samples remained close to zero because the signal approached the lower ADC limit.

\- The signal-generator stability affected some measurements and was treated as a limitation of the test setup.



\## Result



```text

Triangle-wave acquisition: PASS

```



\---



\# Unipolar-Pulse Test



\## Generator Configuration



```text

Waveform:             Square

Configured frequency: 10 Hz

Duty cycle:           10 percent

Generator amplitude:  1.0 V

```



The square wave with a reduced duty cycle was used as a periodic unipolar pulse signal.



\## STM32 Configuration



```text

Sampling frequency: 1000 Hz

Sample count:       512

Acquisition window: 512 ms

```



\## STM32 Results



```text

Valid samples:      512

Minimum:            0 counts, 0.000 V

Maximum:            1214 counts, 0.978 V

Average:            148 counts, 0.119 V

Peak-to-peak:       1214 counts, 0.978 V

Acquisition state:  COMPLETE

Acquisition error:  NONE

```



\## Timing Analysis



The pulses began approximately at the following sample indices:



```text

1

101

201

301

401

501

```



The separation between pulse starts was approximately:



```text

100 samples

```



The high region of each pulse lasted approximately:



```text

10 samples

```



With a 1 ms sampling period:



```text

Pulse period:   approximately 100 ms

Pulse duration: approximately 10 ms

Signal frequency: approximately 10 Hz

Duty cycle: approximately 10 percent

```



\## Observations



\- The buffer contained periodic high-level pulses over a low baseline.

\- The high level was close to 1 V.

\- The low baseline was generally between 0 and approximately 30 ADC counts.

\- The pulse timing matched the configured generator frequency and duty cycle.

\- Samples taken during signal transitions sometimes produced intermediate ADC values.

\- The pulse waveform provides a suitable reference for future baseline, amplitude, peak-index, peak-time, duration, and saturation calculations.



\## Result



```text

Unipolar-pulse acquisition: PASS

```



\---



\# Instrument and Test-Setup Limitations



The waveform-validation experiments revealed several limitations associated with the test setup:



\- The FNIRSI generator and oscilloscope were operated simultaneously.

\- The FNIRSI interface occasionally became slow after extended use.

\- Short intermittent glitches were visible in some generated signals.

\- Similar disturbances were observed even after disconnecting the Nucleo board.

\- The STM32 correctly preserved disturbances that were physically present at the ADC input.

\- The measured generator output occasionally approached or slightly crossed the zero-volt boundary.

\- Signals close to zero produced ADC samples equal to zero.

\- The current circuit includes only a series current-limiting resistor and does not yet contain the final input-protection and offset circuit.

\- The ADC voltage conversion currently uses a nominal reference value.

\- The test setup uses breadboard connections and jumper wires.

\- The acquisition is not phase-synchronized with the generator waveform.



No digital filtering was applied to remove anomalous samples.



All raw observations were preserved.



\---



\# Overall Validation Result



The STM32 Instrumentation Platform successfully acquired four controlled waveform types:



```text

Square wave:       PASS

Sine wave:         PASS

Triangle wave:     PASS

Unipolar pulse:    PASS

```



The experiments demonstrated that:



\- TIM2 generated stable hardware sampling events.

\- ADC1 converted the input signal at the requested sampling frequency.

\- DMA stored the requested number of samples in sequence.

\- The exported indices were complete and ordered.

\- The digital waveform shapes corresponded to the analog reference shapes.

\- Approximately 100 samples were acquired per period for a 10 Hz signal sampled at 1000 Hz.

\- Approximately five complete periods were stored inside each 512-sample acquisition.

\- The system captured signal amplitude, periodicity, pulse duration, and duty-cycle behavior.

\- The complete sample buffer remained available for repeated export and later analysis.



These tests validate the current hardware-timed acquisition architecture:



```text

TIM2 TRGO

&#x20;       |

&#x20;       v

ADC1

&#x20;       |

&#x20;       v

DMA

&#x20;       |

&#x20;       v

Acquisition buffer

&#x20;       |

&#x20;       v

UART data export

```



The next development phase is signal characterization in firmware, including:



\- Baseline estimation

\- RMS-noise calculation

\- Amplitude relative to baseline

\- Peak-index detection

\- Peak-time calculation

\- Saturation detection

