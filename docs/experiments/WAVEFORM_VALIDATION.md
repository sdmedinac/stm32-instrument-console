# Controlled Waveform Acquisition Validation

## Purpose

The purpose of these experiments was to validate the ability of the STM32 Instrumentation Platform to acquire controlled time-varying analog signals using hardware-timed ADC sampling and DMA.

The experiments evaluated whether the complete acquisition chain could preserve waveform shape, amplitude, timing, periodicity, and sample order.

The validated acquisition path was:

```text
FNIRSI signal generator
        |
        v
Series current-limiting resistor
        |
        v
PA0 / ADC1_IN0
        |
        v
ADC1 regular conversion
        |
        v
TIM2 TRGO sampling trigger
        |
        v
DMA2 Stream 0
        |
        v
Acquisition buffer
        |
        v
UART data export
```

The tests included:

- Square wave
- Sine wave
- Triangle wave
- Unipolar pulse

All waveforms were observed using the FNIRSI oscilloscope before being connected to the STM32 ADC.

---

# Test Equipment

The experiments used:

- NUCLEO-F446RE
- STM32F446RE microcontroller
- FNIRSI signal generator
- FNIRSI oscilloscope
- Oscilloscope probe
- Breadboard
- Jumper wires
- Series resistor between the generator and PA0
- USB connection for ST-LINK and the Virtual COM Port
- PuTTY serial terminal

---

# Electrical Connection

The signal-generator output was connected to PA0 through a series resistor.

```text
FNIRSI OUT
    |
    v
Series resistor
    |
    +-------- PA0 / A0 / ADC1_IN0
    |
    +-------- Oscilloscope CH1 probe tip

FNIRSI GND
    |
    +-------- Nucleo GND
    |
    +-------- Oscilloscope probe ground
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

---

# Common STM32 Configuration

Unless stated otherwise, the waveform tests used:

```text
Sampling frequency: 1000 Hz
Sampling period:    1 ms
Sample count:       512
Acquisition window: 512 ms
ADC resolution:     12 bits
ADC input:          PA0 / ADC1_IN0
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

---

# Square-Wave Test

## Generator Configuration

```text
Waveform:             Square
Configured frequency: 10 Hz
Duty cycle:           50 percent
Generator amplitude:  1.0 V
```

## Oscilloscope Reference

```text
Measured Vmin:       0.02 V
Measured Vmax:       1.00 V
Measured Vpp:        0.98 V
Measured frequency:  10.08 Hz
Coupling:            DC
```

## STM32 Configuration

```text
Sampling frequency: 1000 Hz
Sample count:       512
Acquisition window: 512 ms
```

## STM32 Results

```text
Valid samples:      512
Minimum:            0 counts, 0.000 V
Maximum:            1232 counts, 0.993 V
Average:            601 counts, 0.484 V
Peak-to-peak:       1232 counts, 0.993 V
Acquisition state:  COMPLETE
Acquisition error:  NONE
```

## Timing Analysis

The main rising edges appeared approximately every 100 samples.

```text
Approximate samples per period: 100
Approximate signal period:      100 ms
Estimated signal frequency:     10 Hz
Approximate captured periods:   5.12
```

The high and low regions lasted approximately 50 samples each, which was consistent with the configured 50 percent duty cycle.

## Observations

- The exported buffer contained two clearly separated voltage levels.
- The high level was close to 1 V.
- The low level was close to 0.02 V.
- The rising and falling transitions repeated every approximately 100 samples.
- The waveform matched the expected square-wave behavior.
- The STM32 peak-to-peak result was close to the oscilloscope measurement.
- The acquisition contained approximately five complete waveform periods.

## Result

```text
Square-wave acquisition: PASS
```

---

# Sine-Wave Test

## Generator Configuration

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

The signal can be interpreted approximately as an AC component superimposed on a positive DC level:

```text
Approximate DC offset: 0.51 V
Approximate peak AC amplitude: 0.49 V
Approximate total range: 0.02 V to 1.00 V
```

## STM32 Configuration

```text
Sampling frequency: 1000 Hz
Sample count:       512
Acquisition window: 512 ms
```

## STM32 Results

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

## Timing Analysis

The waveform maxima appeared approximately every 100 samples.

Representative maximum locations were approximately:

```text
First maximum:    sample 27
Second maximum:   sample 127
Third maximum:    sample 227
Fourth maximum:   sample 327
Fifth maximum:    sample 427
```

Therefore:

```text
Approximate samples per period: 100
Approximate signal period:      100 ms
Estimated signal frequency:     10 Hz
Approximate captured periods:   5.12
```

## Observations

- The samples changed gradually between the minimum and maximum.
- The exported buffer clearly differed from the square-wave buffer.
- The maxima and minima repeated with stable periodicity.
- The acquisition contained approximately five complete waveform periods.
- The measured peak-to-peak voltage was close to the oscilloscope reference.
- Occasional brief downward glitches were observed during some measurements.
- Similar glitches remained visible after the Nucleo board was disconnected.
- The glitches disappeared when the oscilloscope input was connected directly to ground.
- The intermittent disturbance was attributed to the signal-generation or measurement setup rather than to ADC, DMA, or acquisition-firmware behavior.
- The anomalous samples were preserved without filtering.
- A later acquisition produced a cleaner and more consistent sinusoidal waveform.

## Result

```text
Sine-wave acquisition: PASS
```

---

# Triangle-Wave Test

## Generator Configuration

```text
Waveform:             Triangle
Configured frequency: 10 Hz
Generator amplitude:  1.0 V
```

An initial triangular-wave test showed an unstable minimum level and temporary lower clipping.

The final representative acquisition was repeated after the signal-generator output became more stable.

## STM32 Configuration

```text
Sampling frequency: 1000 Hz
Sample count:       512
Acquisition window: 512 ms
```

## STM32 Results

```text
Valid samples:      512
Minimum:            0 counts, 0.000 V
Maximum:            1188 counts, 0.957 V
Average:            598 counts, 0.482 V
Peak-to-peak:       1188 counts, 0.957 V
Acquisition state:  COMPLETE
Acquisition error:  NONE
```

## Timing Analysis

The waveform maxima and minima repeated approximately every 100 samples.

Representative maximum locations were approximately:

```text
First maximum:    sample 27
Second maximum:   sample 127
Third maximum:    sample 227
Fourth maximum:   sample 327
Fifth maximum:    sample 427
```

Representative minimum locations were approximately:

```text
First minimum:    samples 77 to 78
Second minimum:   sample 177
Third minimum:    samples 277 to 278
Fourth minimum:   samples 377 to 378
Fifth minimum:    samples 477 to 478
```

The resulting timing characteristics were:

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

## Average-Value Analysis

For a triangular waveform approximately symmetric between 0 V and 0.957 V, the expected midpoint is:

```text
Expected midpoint =
(0.000 V + 0.957 V) / 2

Expected midpoint =
approximately 0.479 V
```

The STM32 measured:

```text
Measured average =
0.482 V
```

The measured average was therefore close to the expected midpoint of the acquired voltage range.

## Observations

- The exported buffer contained nearly linear rising and falling segments.
- The waveform was clearly distinguishable from the sine and square waves.
- The average result was close to the midpoint of the measured range.
- The final acquisition did not contain the prolonged lower clipping seen during the initial attempt.
- A small number of samples remained close to zero because the signal approached the lower ADC limit.
- The signal-generator stability affected some measurements and was treated as a limitation of the test setup.
- The final capture presented stable periodicity and approximately linear ramps.

## Result

```text
Triangle-wave acquisition: PASS
```

---

# Unipolar-Pulse Test

## Generator Configuration

```text
Waveform:             Square
Configured frequency: 10 Hz
Duty cycle:           10 percent
Generator amplitude:  1.0 V
```

The square wave with a reduced duty cycle was used as a periodic unipolar pulse signal.

## STM32 Configuration

```text
Sampling frequency: 1000 Hz
Sample count:       512
Acquisition window: 512 ms
```

## STM32 Results

```text
Valid samples:      512
Minimum:            0 counts, 0.000 V
Maximum:            1214 counts, 0.978 V
Average:            148 counts, 0.119 V
Peak-to-peak:       1214 counts, 0.978 V
Acquisition state:  COMPLETE
Acquisition error:  NONE
```

## Timing Analysis

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
Pulse period:     approximately 100 ms
Pulse duration:   approximately 10 ms
Signal frequency: approximately 10 Hz
Duty cycle:       approximately 10 percent
```

## Average-Value Analysis

For an ideal pulse between approximately 0 V and 1 V with a 10 percent duty cycle:

```text
Expected average =
1 V x 0.10

Expected average =
approximately 0.10 V
```

The STM32 measured:

```text
Measured average =
0.119 V
```

The difference can be influenced by:

- Nonzero baseline
- Partial pulses at the beginning or end of the acquisition
- Signal-transition samples
- Generator instability
- Nominal ADC reference-voltage conversion
- Electrical noise

## Observations

- The buffer contained periodic high-level pulses over a low baseline.
- The high level was close to 1 V.
- The low baseline was generally between 0 and approximately 30 ADC counts.
- The pulse timing matched the configured generator frequency and duty cycle.
- Samples taken during signal transitions sometimes produced intermediate ADC values.
- The pulse waveform provides a suitable reference for future baseline, amplitude, peak-index, peak-time, duration, and saturation calculations.
- Approximately five complete pulse periods and a partial sixth period were captured.

## Result

```text
Unipolar-pulse acquisition: PASS
```

---

# Instrument and Test-Setup Limitations

The waveform-validation experiments revealed several limitations associated with the test setup:

- The FNIRSI