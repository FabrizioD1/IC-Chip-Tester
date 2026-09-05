# IC Chip Tester
An Arduino-based project which allows users to test IC chips by sending various discrete input combinations to the chip and generates a truth table from the resulting outputs.

# Overview
The project allows the user to configure Arduino pins to where certain pins are assigned to the input pins of a selected IC chip, and others are assigned to the output pins of that IC. After configuring the inputs and outputs, the Arduino runs every combination from 0 to $2^{n}-1$ in binary (where n in the number of assigned input pins) through the input pins, and the output of each combination is sampled. The input combinations and the sampled outputs are then put together in a truth table that verifies the behavior of the chosen IC chip.

# Features
- Ability to adjust input/output pins through the Arduino terminal
- Pin configuration offers sorting selected pins
- Input validation for user input, ensuring inputs are within specific bounds
- Configurable delays in the ```.ino``` file

# Hardware
- Arduino UNO R3 Microcontroller with USB Cable
- Breadboard and jumper wires
- IC Chip to be tested

# Software
- Arduino IDE
- C++
- Arduino Vector Library (https://github.com/janelia-arduino/Vector)

# How It Works
### 1. Configuration of Pins
Before being able to test an IC Chip, you must configure which Arduino pins are connected to your chip's inputs and outputs. When first running the program, you are presented with a menu in the Arduino terminal, prompting you to make a selection.

<img width="340" height="137" alt="image" src="https://github.com/user-attachments/assets/2dcfc323-cc3d-4a14-8bb8-a8622a432280" />

Entering either 1 or 2 will take the user to a submenu to allow configuration of the specific type of pin. The submenu is shown below for option 1 (Edit Input Pins), but the submenu is nearly identical for 'Edit Output Pins'.

<img width="342" height="190" alt="image" src="https://github.com/user-attachments/assets/da8eef1b-59e7-4048-8e57-844566690da6" />

The submenu contains various options which may be of use when configuring pins.

0. **Return to Main Menu**: Brings the user back to the main menu to configure other parts or to run the truth table generator.
1. **Check Pins**: Displays which pins are currently configured in the given submenu. For example, if the user is currently configuring input pins, the input pins will be displayed by selecting this option.
2. **Add Pins**: Allows the user to input pins until they enter the word 'DONE'. Input validation ensures duplicates cannot be entered and that only valid Arduino pins can be selected.
3. **Remove Pins**: Allows the user to remove pins from the selected assortment until they either enter the world 'DONE', or the vector of pins is empty (since there is nothing else to remove at this point).
4. **Sort Pins**: In the event that a mix of adding and removing pins results in a mess of unsorted pins, this option automatically will sort the user's inputted pins to be in incremental order. The sorting algorithm for this sort is an optimized Bubble Sort, which has a time complexity of $O(n^{2})$. While there are faster sorting algorithms out there, a simpler algorithm was chosen due to the relatively small size of vector that would ever need to be sorted at a time.
5. **Reset Pins**: Clears the vector that stores the inputted pins, allowing the user to input new pins.

Utilizing this submenu, the user can configure both the input pins and the output pins for the IC to be tested.

### 2. Validation
Attempting to create a truth table through the main menu will first put the user through a validation stage. This stage is to ensure that everything is valid before attempting to test the IC Chip. As of now, the only validation check ensures that there is at least 1 input pin and at least 1 output pin configured, so that a valid truth table is able to be generated. If the validation stage fails, the user is sent back to the main menu, and feedback lets them know what they must correct before reattempting to generate a truth table.

### 3. Truth Table Generation
If the user passes the validation stage, they will advance to this stage, where a binary combination is made for all integers between 0 and $2^{n}-1$ (where n is the number of inputs), and each of these binary combinations is passed into the IC chip through the configured input pins. As each combination is entered into the IC chip, the output pins that were configured will sample the outputs of the IC chip. These values are then put together in a truth table, showing what the IC chip outputted for every possible input combination. 

Example generated truth table from testing a 74LS32 chip (OR gate), where the input pins are 6 and 7, and the output pin is 4:

<img width="81" height="120" alt="image" src="https://github.com/user-attachments/assets/8aba5216-bfed-4e3d-bbd9-26b40a32a1d3" />

After generating a truth table, the user is taken back to the main menu shortly after. There, they may modify the program's settings to test a different chip.

# Schematic
An exact schematic for a project like this cannot be made since the hardware aspect of the project is variable. Depending on the IC chip tested, the number of wires coming out of Arduino pins will vary. Additionally, the IC chip to be tested will vary. An example schematic is available in the ```schematic/``` directory, which shows a possible example of utilizing this project. The example shows what the schematic of testing a 74LS08 chip (AND gate) could look like. A preview is shown below:

<img width="648" height="465" alt="image" src="https://github.com/user-attachments/assets/ba2df349-4ada-4c77-a2ac-fe47c9334221" />

# What I Learned From This Project
- Compared to my previous project, this project utilizes a state machine model which does not follow a straight path. I had to adapt my implementation of the state machine to ensure the program executed correctly.
- When sending larger objects into functions, it is more efficient to send a ```const reference``` of the object to avoid copying a large object.
- I learned how to use the Arduino implementation of the Vector class.

# Potential Future Improvements
- Connecting a database of known truth tables to the project to allow classification of the generated truth table would allow for easier checking of the appropriate behavior of the tested chip.
