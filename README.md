# 2D Artificial-Mechanical Cardiac Tissue
In Sato Lab, we are researching how to create more realistic machines. One aspect is designing novel actuators that can be used in machines or mechanical systems to reflect more life-like motion and also applying these systems to biomedical applications. In this project we created an artificial-mechanical cardiac tissue using this concept.

## Background
Heart failure is a common occurence, with current solutions varying from heart donors to artificial whole-heart implants (total artificial hearts), which require the replacement of the entirety or most of the heart. We want to create a customizable artificial cardiac tissue that uses a variety of different mechanical components to mimic individual artificial cells and a real-time simulation model to control the electro-physiological behavior of all the cells together. While still in it’s early stages, the overall goal of this project is to create a more realistic artificial cardiac tissue that can be used for tissue implants to replace diseased or failing sections of the heart rather than the current heart failure solutions.

The contraction and relaxation of the cardiac cells occurs via excitation-contraction coupling, a mechanism where an electrical stimulus is converted into mechanical contraction. In this case, the electrical stimulus is the action potential caused by charged ions passing through ion channels. 

<img src="https://github.com/dinalehienpham/2DArtificialMechanicalCardiacTissue/blob/main/images/actionpotential.png">

Using the action potential, we can simulate the electrophysiology of each individual cardiac cell, and therefore the mechanical contraction and relaxation of our artificial-mechanical cardiac cell. Contraction and relaxation of cardiac tissue involves the spreading, or propagation, of this action potential from one cardiac cell to another. This is simulated by the Echebarria-Karma model as a proof of concept.

<img src="https://github.com/dinalehienpham/2DArtificialMechanicalCardiacTissue/blob/main/images/actionpotentialpropagation.gif">
(above) Visualization of the action potential propagation with 4 cells.

## 2-D Artificial-Mechanical Cardiac Tissue
The 2-D artificial-mechanical cardiac tissue utilized linear servo motors and 3-D printed materials. A Raspberry Pi microcontroller was used to run an altered Echebarria-Karma model to simulate the 2-D action potential propagation, starting from a cell in the upper left corner of the tissue.

<img src="https://github.com/dinalehienpham/2DArtificialMechanicalCardiacTissue/blob/main/images/CAD.JPG" width="373" height="316"/> <img src="https://github.com/dinalehienpham/2DArtificialMechanicalCardiacTissue/blob/main/images/CADexploded.JPG" width="363" height="368"/>

<img src="https://github.com/dinalehienpham/2DArtificialMechanicalCardiacTissue/blob/main/images/CADmove.gif">

(above) CAD design of a single artificial-mechanical cell. (below) A 6-cell by 6-cell tissue.

<img src="https://github.com/dinalehienpham/2DArtificialMechanicalCardiacTissue/blob/main/images/6x6cells.gif">

## Authors
### 2-D Artificial Cardiac Muscle
* Dina Pham (dlpham@ucdavis.edu)
* Dr. Daisuke Sato (dsato@ucdavis.edu)

2020 @ Sato Lab - UC Davis School of Medicine/Department of Pharmacology
