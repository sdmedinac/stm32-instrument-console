# Project Scope and Version 1.0 Definition

## Project Name

STM32 Instrumentation Platform

## Author

Samuel David Medina Contreras

Electronic Engineering student at Universidad del Magdalena.

## Project Purpose

The purpose of this project is to design, implement, and validate an educational embedded instrumentation platform based on the STM32F446RE microcontroller.

The platform will acquire analog signals using hardware-controlled sampling, transfer samples to memory using DMA, characterize acquired signals, monitor system operation, and support reproducible calibration experiments.

The project combines:

- Embedded firmware
- Embedded C
- ARM Cortex-M microcontrollers
- Analog electronics
- Data acquisition
- Signal characterization
- Calibration
- Electronic monitoring
- Laboratory validation
- Python test automation

The project is conceptually inspired by engineering challenges found in scientific instrumentation and detector readout electronics.

The project does not reproduce or replace the specialized electronics used by DUNE or other scientific experiments.

Any future use within a research group must be defined with academic supervision and adapted to a real instrumentation need.

---

# Main Project Goal

The final platform must demonstrate the following complete engineering workflow:

```text
Generate a known analog signal
        |
        v
Pass the signal through an educational analog front-end
        |
        v
Acquire the signal using STM32 hardware-controlled sampling
        |
        v
Transfer the samples to memory using DMA
        |
        v
Characterize the acquired signal in firmware
        |
        v
Transfer results and samples through UART
        |
        v
Automate calibration and validation tests using Python
        |
        v
Compare the results with laboratory instruments
        |
        v
Generate reproducible test evidence and documentation
```

---

# Final Version 1.0 Architecture

```text
Controlled test signal
        |
        v
Input protection
        |
        v
Educational analog front-end
        |
        +---- Defined gain or attenuation
        |
        +---- Signal offset
        |
        +---- Filtering or pulse shaping
        |
        v
STM32 ADC
        |
        v
TIM2 hardware trigger
        |
        v
DMA transfer
        |
        v
Acquisition buffer
        |
        v
Signal characterization
        |
        +---- Baseline
        |
        +---- RMS noise
        |
        +---- Minimum and maximum
        |
        +---- Average
        |
        +---- Peak-to-peak
        |
        +---- Amplitude
        |
        +---- Peak index
        |
        +---- Peak time
        |
        +---- Saturation
        |
        v
Monitoring and acquisition records
        |
        v
UART communication
        |
        v
Python test automation
        |
        v
CSV files, plots, PASS/FAIL results, and final report
```

---

# Mandatory Version 1.0 Requirements

The project will be considered complete only when all requirements in this document have been implemented, tested, and documented.

---

# 1. Configurable Acquisition Firmware

The firmware must support:

- Configurable sampling frequency
- Configurable number of samples
- Hardware-controlled ADC sampling
- TIM2 TRGO as the ADC trigger source
- ADC1 regular conversions
- DMA-based transfers to memory
- A fixed-capacity acquisition buffer
- Protection against buffer overflow
- Safe acquisition start
- Safe acquisition stop
- Multiple consecutive acquisitions
- Dynamic acquisition timeout
- Acquisition state management
- Acquisition error reporting
- Reconfiguration blocked while an acquisition is running

The required commands are:

```text
acq rate <Hz>
acq size <samples>
acq start
acq stop
acq status
acq result
```

The required acquisition states are:

```text
IDLE
RUNNING
COMPLETE
ERROR
```

The required errors must include:

```text
ADC or DMA start failure
Timer start failure
Timer stop failure
ADC or DMA stop failure
ADC or DMA runtime error
Acquisition timeout
Invalid configuration
```

## Acceptance Criteria

- The requested sampling rate is validated.
- The actual sampling rate is calculated and reported.
- The requested sample count is validated.
- The requested sample count never exceeds the buffer capacity.
- DMA writes only inside the acquisition buffer.
- An active acquisition cannot be reconfigured.
- A manual stop during RUNNING returns the system to IDLE.
- A completed acquisition contains the requested number of valid samples.
- Multiple acquisitions can be performed without resetting the board.
- Timeout duration is calculated from sampling rate and sample count.
- Acquisition errors leave the system in a known state.
- The firmware compiles without errors or relevant warnings.

---

# 2. Immediate ADC Diagnostics

The firmware must retain immediate diagnostic measurements using the ADC injected group.

The required commands are:

```text
adc raw
adc voltage
```

The immediate measurement path must remain separate from the regular DMA acquisition path.

```text
Immediate diagnostic path:

Software trigger
        |
        v
ADC1 injected conversion
        |
        v
Immediate raw or voltage result
```

```text
Timed acquisition path:

TIM2 TRGO
        |
        v
ADC1 regular conversion
        |
        v
DMA
        |
        v
Acquisition buffer
```

## Acceptance Criteria

- Raw ADC measurements are available when no acquisition is running.
- Voltage measurements are available when no acquisition is running.
- Immediate ADC diagnostics are rejected while a DMA acquisition is active.
- The injected ADC group uses a software trigger.
- The regular ADC group uses TIM2 TRGO.
- Measurements are validated using GND.
- Measurements are validated using 3.3 V.
- Measurements are validated using a resistor divider.
- Measurements are validated using a potentiometer.
- Measurements are compared with a multimeter.

---

# 3. Controlled Waveform Acquisition

The STM32 must acquire controlled signals generated using the FNIRSI signal generator.

The minimum required signal types are:

- Sine wave
- Square wave
- Unipolar pulse

The experiments must include:

- At least three signal frequencies
- At least three signal amplitudes
- At least two safe DC offset conditions
- Different STM32 sampling frequencies
- Different acquisition sample counts
- Different acquisition window durations

## Acceptance Criteria

Each experiment must document:

```text
Generator waveform
Generator frequency
Generator amplitude
Generator offset
STM32 sampling frequency
STM32 sample count
Acquisition window duration
Minimum measured value
Maximum measured value
Measured amplitude
Saturation status
Oscilloscope reference measurement
STM32 acquisition result
```

The STM32 data must provide a coherent digital representation of the analog reference signal within the limitations of:

- ADC resolution
- Sampling frequency
- Acquisition window
- Analog bandwidth
- Input range
- Signal offset
- Quantization
- Electrical noise

The experiments must demonstrate the relationship between:

```text
Signal frequency
Sampling frequency
Samples per waveform period
Acquisition window duration
```

At least one experiment must demonstrate the effect of insufficient sampling frequency or aliasing.

---

# 4. Signal Characterization in Firmware

The firmware must calculate signal characteristics from a completed acquisition buffer.

The required metrics are:

- Number of valid samples
- Minimum value
- Maximum value
- Average value
- Peak-to-peak value
- Baseline or pedestal
- RMS noise
- Amplitude relative to baseline
- Peak sample index
- Peak time
- Saturation status

Digital pulse area may be implemented if time permits, but it is not required for version 1.0.

## Acceptance Criteria

- Each metric has a documented mathematical definition.
- Intermediate calculations use data types that avoid overflow.
- Results are reported using documented units.
- Raw quantities are reported in ADC counts.
- Relevant quantities are converted to volts or millivolts.
- Peak time uses the actual configured sampling frequency.
- Baseline calculation uses a documented region of the acquisition.
- RMS noise is calculated relative to the baseline.
- Saturation is detected near the valid ADC limits.
- Characterization results are validated using known signals.
- Characterization does not modify the acquisition buffer.
- Characterization is performed outside interrupt context.

The project will not report noise in electrons or charge in femtocoulombs unless the complete analog chain has been calibrated for those physical units.

---

# 5. Educational Analog Front-End

The project must include one functional educational analog front-end.

The analog front-end must include:

- Input current limiting
- Input protection
- Defined gain or attenuation
- Signal offset when required
- Basic filtering or pulse shaping
- Power-supply decoupling
- A test point for oscilloscope measurements
- An output range compatible with the STM32 ADC

## Required Engineering Work

The front-end development must include:

- Component selection
- Gain calculation
- Input range calculation
- Output range calculation
- Frequency-response analysis
- Protection analysis
- LTspice simulation
- Breadboard or prototype-board implementation
- Oscilloscope validation
- Multimeter validation
- Comparison between simulation and measurement

## Acceptance Criteria

- The circuit does not apply a negative voltage directly to the STM32 ADC.
- The circuit output remains inside the documented safe ADC range.
- The expected gain or attenuation is documented.
- The measured gain is reported.
- The input range is documented.
- The output range is documented.
- The filter or shaping behavior is documented.
- The circuit contains appropriate power-supply decoupling.
- The circuit is tested using the FNIRSI generator.
- The circuit is tested using the FNIRSI oscilloscope.
- The STM32 successfully acquires the front-end output.
- Differences between simulation and measurement are documented.

A custom PCB is not required for version 1.0.

A breadboard or prototype-board implementation is sufficient.

---

# 6. Calibration and Monitoring

The final platform must support a reproducible calibration and monitoring experiment.

The calibration must include:

- At least five known input amplitudes
- Multiple acquisitions for each input amplitude
- Measured gain
- Measured offset
- Absolute error
- Relative error
- Linearity evaluation
- Saturation evaluation
- RMS noise measurement
- Repeatability evaluation

Monitoring must include:

- Acquisition state
- Sampling frequency
- Sample count
- Last acquisition error
- ADC or DMA status information
- Relevant analog reference measurements when available
- Relevant front-end supply measurements when available

## Required Calibration Table

The final documentation must include a calibration table containing:

```text
Input amplitude
Expected output
Measured output
Absolute error
Relative error
RMS noise
Saturation status
PASS or FAIL
```

## Acceptance Criteria

The final calibration and monitoring report must identify:

- Useful input range
- Measured gain
- Measured offset
- Maximum observed absolute error
- Maximum observed relative error
- Approximate saturation point
- RMS noise under stable-input conditions
- Repeatability across multiple acquisitions
- Known limitations of the analog front-end
- Known limitations of the STM32 ADC measurement

---

# 7. UART Acquisition Data Transfer

The firmware must support transferring acquisition information and samples to a computer.

The transferred information must include:

- Capture identifier
- Sampling frequency
- Sample count
- Acquisition status
- Error status
- Characterization results
- Acquisition samples

The data format may be text-based or binary, but it must be:

- Documented
- Machine-readable
- Reproducible
- Detectable as complete or incomplete

A checksum or CRC should be included if practical.

If a checksum or CRC is not implemented, the limitation must be documented.

## Acceptance Criteria

- A computer can verify communication with the STM32.
- A computer can configure the sampling frequency.
- A computer can configure the sample count.
- A computer can request an acquisition.
- The STM32 can report acquisition completion.
- The STM32 can report an acquisition error.
- A computer can retrieve the complete acquisition buffer.
- The receiver can verify the expected sample count.
- Transferred samples can be saved without manual copying.
- The beginning and end of an acquisition record can be identified.

---

# 8. Mandatory Python Test Automation

Python automation is mandatory for version 1.0.

The Python application must be capable of:

- Opening the serial port
- Verifying communication with the STM32
- Configuring the sampling frequency
- Configuring the sample count
- Starting an acquisition
- Waiting for COMPLETE or ERROR
- Applying a host-side acquisition timeout
- Requesting characterization results
- Downloading acquisition samples
- Saving data to CSV
- Repeating acquisitions
- Evaluating acceptance limits
- Producing PASS or FAIL results
- Creating plots
- Generating a final test summary or report

Python will not participate in the real-time sampling path.

The real-time acquisition path remains:

```text
TIM2
    |
    v
ADC1
    |
    v
DMA
    |
    v
Acquisition buffer
```

Python is responsible only for:

- Test configuration
- Test orchestration
- Serial communication
- Data collection
- Data analysis
- Plot generation
- Report generation

## Required Automated Test Groups

### Acquisition Test

The automated acquisition test must verify:

- Initial IDLE state
- Sampling-rate configuration
- Sample-count configuration
- Acquisition start
- Transition to RUNNING
- Transition to COMPLETE
- Correct sample count
- Absence of acquisition errors
- Reasonable acquisition duration
- Ability to repeat acquisitions

An example result may contain:

```text
Test name: Acquisition at 1000 Hz
Requested samples: 512
Expected duration: 512 ms
Measured duration: 514 ms
Samples received: 512
Final state: COMPLETE
Acquisition error: NONE
Result: PASS
```

### Linearity Test

The linearity test must support:

- Multiple known amplitudes
- Multiple acquisitions per amplitude
- Average response calculation
- Absolute error calculation
- Relative error calculation
- Saturation evaluation
- PASS or FAIL result

The FNIRSI amplitude may be changed manually.

Python may prompt the user with instructions such as:

```text
Set the signal generator amplitude to 0.50 V.
Press Enter when ready.
```

Automatic control of the FNIRSI is not required.

### Noise and Stability Test

The noise and stability test must:

- Perform repeated acquisitions with a stable input.
- Collect baseline measurements.
- Collect RMS noise measurements.
- Collect peak-to-peak measurements.
- Compare repeated acquisitions.
- Detect values outside configured limits.
- Save all test parameters.
- Produce a PASS or FAIL result.

### Waveform Test

The waveform test must support:

- Sine-wave acquisitions
- Square-wave acquisitions
- Pulse acquisitions
- CSV storage
- Time-axis generation
- Waveform plots
- Acquisition metadata storage
- Comparison of different sampling frequencies

## Python Acceptance Criteria

- Tests can be repeated without editing the Python source code for every acquisition.
- Serial communication errors are handled.
- Missing data is detected.
- Incomplete acquisition records are detected.
- Incorrect sample counts are detected.
- Results are saved with identifiable filenames.
- Test parameters are stored with the data.
- PASS or FAIL criteria are documented.
- Generated plots include units.
- Generated plots include acquisition configuration.
- The automation can run multiple acquisitions in one session.
- The final report summarizes the performed tests.

A graphical user interface is not required.

A command-line Python application is sufficient.

---

# 9. Experimental Validation

The final system must be validated using:

- Multimeter
- FNIRSI signal generator
- FNIRSI oscilloscope
- Known DC input levels
- Controlled periodic waveforms
- Controlled pulse signals

## Acceptance Criteria

- STM32 measurements are compared with instrument measurements.
- Differences are quantified.
- Absolute error is reported.
- Relative error is reported.
- Known limitations are documented.
- All external signals remain inside the safe ADC voltage range.
- Experimental procedures can be repeated.
- Test configurations are recorded.
- Results are stored.
- Representative results are included in the final documentation.

---

# 10. Final Documentation

The repository must include:

- Updated README
- Project scope document
- Firmware architecture diagram
- ADC architecture explanation
- DMA acquisition explanation
- Command reference
- UART data-format documentation
- Analog front-end schematic
- LTspice simulation files
- Front-end design calculations
- Experimental test procedure
- Calibration table
- Noise results
- Linearity results
- Saturation results
- Python automation instructions
- Example CSV files
- Example plots
- Known limitations
- Future work
- Hardware photographs
- Short demonstration video

## Final Release

When all mandatory requirements and acceptance criteria are complete, the project will be tagged as:

```text
v1.0
```

The version 1.0 release must contain:

- Stable firmware
- Stable Python test application
- Final front-end design
- Experimental results
- Final documentation

---

# Version 1.0 Completion Checklist

## Firmware

- [x] Sampling frequency configurable
- [ ] Sample count configurable
- [x] TIM2 TRGO acquisition
- [x] ADC1 regular conversion
- [x] DMA buffer transfer
- [x] Immediate injected ADC diagnostics
- [x] Acquisition state machine
- [x] Dynamic timeout
- [x] Manual stop
- [x] Consecutive acquisitions
- [x] Basic acquisition error handling
- [ ] Invalid configuration error
- [ ] Buffer transfer through UART
- [ ] Signal characterization
- [ ] Baseline estimation
- [ ] RMS noise calculation
- [ ] Peak index calculation
- [ ] Peak time calculation
- [ ] Saturation detection

## Analog Electronics

- [ ] Input protection
- [ ] Defined gain or attenuation
- [ ] Signal offset if required
- [ ] Filtering or pulse shaping
- [ ] Power-supply decoupling
- [ ] LTspice simulation
- [ ] Breadboard implementation
- [ ] Oscilloscope validation
- [ ] Multimeter validation
- [ ] Safe connection to the STM32 ADC

## Calibration and Monitoring

- [ ] Gain measurement
- [ ] Offset measurement
- [ ] Linearity evaluation
- [ ] Saturation evaluation
- [ ] RMS noise evaluation
- [ ] Repeatability evaluation
- [ ] Monitoring information available
- [ ] Calibration procedure documented
- [ ] Calibration results documented

## Python Automation

- [ ] Serial connection
- [ ] Firmware communication check
- [ ] Sampling-rate configuration
- [ ] Sample-count configuration
- [ ] Automated acquisition
- [ ] Acquisition-status polling
- [ ] Error detection
- [ ] Sample download
- [ ] CSV storage
- [ ] Repeated tests
- [ ] PASS or FAIL evaluation
- [ ] Plot generation
- [ ] Final report generation

## Documentation

- [ ] README complete
- [x] Project scope created
- [ ] Architecture documented
- [ ] Commands documented
- [ ] UART data format documented
- [ ] Front-end schematic included
- [ ] LTspice simulation included
- [ ] Experimental procedure included
- [ ] Calibration results included
- [ ] Known limitations included
- [ ] Future work separated from version 1.0
- [ ] Hardware photographs included
- [ ] Demonstration video recorded
- [ ] Release v1.0 created

---

# Explicitly Out of Scope for Version 1.0

The following features are not required for version 1.0:

- FreeRTOS
- Zephyr
- Embedded Linux
- DMA circular mode
- Double buffering
- Pre-trigger capture
- Post-trigger capture
- Adaptive trigger algorithms
- Complex event detection
- Wi-Fi
- Bluetooth
- CAN
- Ethernet
- USB device implementation
- Custom PCB
- External ADC
- Multiple simultaneous analog channels
- Machine learning
- Mobile application
- Web application
- Database
- Complex graphical interface
- Automatic FNIRSI control
- Cryogenic operation
- Reproduction of DUNE ASICs
- Replacement of DUNE electronics

These features may only be included under:

```text
Future Work
```

These features cannot block the version 1.0 release.

---

# Change-Control Rule

Before adding a new feature, answer the following questions:

1. Is the feature explicitly required by this document?
2. Does the feature directly support acquisition, characterization, calibration, monitoring, validation, or automation?
3. Does the feature have a specific and measurable acceptance test?
4. Is the feature necessary to complete version 1.0?

If the answer is no, the feature must be placed in Future Work.

---

# Development Rule

Each completed feature must satisfy the following checklist:

```text
[ ] Requirement defined
[ ] Design explained
[ ] Implementation completed
[ ] Firmware compiles
[ ] Hardware test completed
[ ] Normal path tested
[ ] Error path considered
[ ] Results documented
[ ] Git commit created
[ ] Project checklist updated
```

Code that only compiles is not considered fully validated.

A feature is considered complete only after its required behavior has been tested and documented.

---

# Definition of Done

The project is complete when the following statement is true:

A known analog signal passes through an educational analog front-end designed and validated by the project author. The STM32 acquires the signal using configurable timer-controlled ADC sampling and DMA. The firmware stores and characterizes the signal, reports system status and errors, and transfers acquisition data through UART. A Python application configures the platform, performs repeatable tests, saves data, evaluates acceptance limits, generates plots, and produces a final calibration report. The complete system is validated using laboratory instruments and documented in a reproducible way.

When this definition has been demonstrated and all mandatory sections are complete, development of version 1.0 stops.

All additional ideas are moved to Future Work.