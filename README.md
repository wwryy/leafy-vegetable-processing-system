# Leafy Vegetable Cleaning and Packaging System

> **Root Trimming · Dry Cleaning · Box Separation · Mechanical Weighing · Intermittent Indexing · Packaging Transfer**

An integrated mechanical system for the preliminary cleaning, portioning, and packaging of leafy vegetables such as spinach.

The project combines six coordinated modules—root trimming and dry cleaning, box separation, mechanical weighing, 90° indexing, box transfer, and final pressing—into a compact processing line intended for small-scale agricultural processing and engineering education.

> **Repository status:** This repository is a public project portfolio and technical overview. Source code, MATLAB simulation files, CAD/SolidWorks models, manufacturing drawings, and complete design documentation are not currently public.

---

## Project Overview

Leafy vegetables are commonly trimmed, cleaned, weighed, and packed by hand before entering supermarkets, community stores, or distribution centers. Manual processing can lead to inconsistent root length, cleanliness, package weight, and product orientation.

This project explores a compact, low-cost mechanical alternative. The design emphasizes reliable mechanism-level coordination, simple maintenance, and reduced dependence on pneumatic, hydraulic, and complex closed-loop control systems.

<p align="center">
  <img src="assets/images/system-cad.png" width="820" alt="CAD overview of the leafy vegetable processing system">
</p>

<p align="center">
  <i>Integrated system architecture and major functional modules.</i>
</p>

---

## System Workflow

```text
Leafy Vegetables
       ↓
Constrained Feeding
       ↓
Root Trimming and Dry Brushing
       ↓
Automatic Box Separation
       ↓
Mechanical Weight-Threshold Detection
       ↓
Box Transfer
       ↓
90° Intermittent Indexing
       ↓
Secondary Transfer and Box Pressing
```

The machine coordinates two material flows:

- a **vegetable-processing line** for feeding, root trimming, and dry cleaning;
- a **package-handling line** for box separation, weighing, orientation, transfer, and pressing.

---

## Key Features

- Integrated processing from root preparation to packaged-box transfer
- Dry root-cleaning method that avoids a dedicated washing stage
- Periodic single-box separation from a stacked supply
- Roberval-type mechanical weighing with an adjustable threshold
- Symmetric Geneva mechanism for 90° intermittent indexing
- Four-bar transfer mechanism designed around an approximately straight output segment
- Standardized motors, gears, bearings, shafts, and aluminum profiles
- Modular architecture for fabrication, assembly, testing, and maintenance

---

# Mechanical Design

## 1. Root Trimming and Dry Cleaning

The front-end module uses constrained belt feeding to guide leafy vegetables through two processing stations. A circular blade removes excess root material, while a rotating brush performs dry soil removal.

The transport and processing mechanisms use independent drives, allowing feed speed and cutting/brushing speed to be adjusted separately.

<p align="center">
  <img src="assets/images/root-processing.png" width="650" alt="Root trimming and cleaning module">
</p>

The main design considerations include:

- lateral positioning of the vegetable stems;
- vertical support near the cutting and brushing zones;
- suppression of vibration through pressure rollers;
- independent speed adjustment for feeding and processing;
- reduced damage to leaves and stems.

---

## 2. Automatic Box Separation

Lightweight plastic boxes are supplied as a vertical stack and released one at a time. The mechanism converts rotary input into a controlled separating motion while supporting the remaining stack.

<p align="center">
  <img src="assets/animations/box-separation.gif" width="560" alt="Box separation mechanism simulation">
</p>

---

## 3. Mechanical Weighing

A Roberval-type linkage is used for mechanical threshold detection. Its parallel-link structure helps the weighing platform remain approximately level as the load changes.

The mechanism is intended to determine whether the package has reached a target quantity rather than to replace a precision electronic scale.

---

## 4. 90° Intermittent Indexing

A symmetric Geneva mechanism rotates the package by 90° between two transfer directions. The intermittent motion provides a defined dwell interval for package entry and exit.

<p align="center">
  <img src="assets/animations/geneva-indexing.gif" width="560" alt="Geneva indexing mechanism animation">
</p>

<p align="center">
  <img src="assets/images/indexing-prototype.png" width="650" alt="Indexing mechanism prototype">
</p>

---

## 5. Box Transfer

Two transfer stages move the filled box through the weighing and indexing stations. A four-bar mechanism is configured so that the end-effector follows an approximately straight segment during the useful part of its trajectory.

<p align="center">
  <img src="assets/animations/four-bar-transfer.gif" width="560" alt="Four-bar box transfer mechanism animation">
</p>

---

## 6. Box Pressing

The final module synchronizes box movement and pressing through a cam-driven linkage. The mechanism coordinates the pressing stroke with conveyor motion while maintaining a compact layout.

<p align="center">
  <img src="assets/animations/box-pressing.gif" width="560" alt="Box pressing mechanism simulation">
</p>

---

# Physical Prototype

A physical prototype was assembled to evaluate module integration, mechanical timing, material transfer, and the accessibility of key adjustment points.

<p align="center">
  <img src="assets/images/prototype.png" width="760" alt="Physical prototype of the integrated system">
</p>

The prototype supports engineering validation of:

- continuous material flow between modules;
- single-box release and positioning;
- mechanism timing and interference checks;
- package orientation and transfer;
- component accessibility and maintainability.

---

## Demonstration Video

The following video presents the integrated prototype, its principal mechanisms, and the overall operating process.

<p align="center">
  <a href="assets/videos/system-demonstration.mp4">
    <img src="assets/images/prototype.png" width="760" alt="Watch the system demonstration video">
  </a>
</p>

<p align="center">
  <a href="assets/videos/system-demonstration.mp4"><strong>▶ Watch the system demonstration video</strong></a>
</p>

---

## Design Targets

| Item | Design target |
|---|---|
| Processing object | Spinach and similar lightweight leafy vegetables |
| Packaging | Stacked lightweight plastic boxes |
| Conveyor speed | Approximately 0.20 m/s |
| Indexing angle | 90° intermittent rotation |
| Weighing principle | Roberval linkage with adjustable counterweight |
| Transfer principle | Four-bar linkage using an approximately straight trajectory segment |
| Design emphasis | Low cost, limited control complexity, ease of fabrication and maintenance |

---

## Project Scope

This public repository presents the system concept, mechanical architecture, representative prototype images, and mechanism animations.

The following materials remain private at this stage:

- control and simulation source code;
- MATLAB models and analysis scripts;
- SolidWorks assemblies and part files;
- DWG, STEP, and manufacturing drawings;
- detailed calculations and complete design documentation;
- unpublished research and competition materials.

For academic discussion or collaboration, please contact the repository owner through GitHub.

---

## Acknowledgements

Developed by a student engineering team at **Beijing University of Technology** with faculty supervision.

This repository is intended as a project portfolio and technical overview. All rights are reserved unless otherwise stated.

