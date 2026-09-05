#include <Vector.h>

/*
  Title: IC Chip Tester
  Author: Fabrizio De Lima Bello
  Version: 1.0
  Last Update: 9/5/2026
*/

// --- CONFIGURATION ---

#define BAUD_RATE 31250
#define PAUSE_DELAY 1000
#define TRUTH_TABLE_DELAY 1000
#define TRUTH_TABLE_RETURN_DELAY 10000

// define valid arduino pins for use
#define MIN_PIN 2
#define MAX_PIN 13

// set up storage for user input later on
constexpr size_t MAX_SIZE = 12;

int storage1[MAX_SIZE];
Vector<int> inputs(storage1);

int storage2[MAX_SIZE];
Vector<int> outputs(storage2);

int storage3[MAX_SIZE*2];
Vector<int> pinsUsed(storage3);

// configure states
enum State {
  MAIN_MENU,
  EDIT_INPUT_PINS,
  EDIT_OUTPUT_PINS,
  CHECK_PINS,
  ADD_PINS,
  REMOVE_PINS,
  SORT_PINS,
  RESET_PINS,
  VALIDATE_PINS,
  ADJUST_PIN_MODES,
  CREATE_TRUTH_TABLE
};

// DO NOT CHANGE!! used in moving between states
int currentState = 0;
String parentFunction = "";

void setup() {
  Serial.begin(BAUD_RATE);
  Serial.println();
}

// --- INPUT FUNCTIONS ---

void repeatedRemoveFromVector(Vector<int>* vec, const String& prompt) {
  // prompts the user to enter values to remove from a vector.
  // ends when user inputs 'DONE' or the vector is empty

  Serial.println("Enter values to remove from "+prompt+". (Enter 'DONE' when finished)");

  bool done = false;
  int input = 0;

  while (!done) {

    // show user what is left in the vector at the moment
    Serial.println("Current values:");
    printVector(*vec);
    Serial.println();
    Serial.println();

    input = integerInput();

    // integerInput() returns -1 if user enters 'DONE'
    if (input == -1) {
      return;
    }

    // remove the element if in the vector
    if (vectorContains(*vec, input)) {
      vec->remove(vectorIndexOf(*vec, input));
      pinsUsed.remove(vectorIndexOf(pinsUsed, input));
      
      // if vector is now empty, we cant remove further, so end loop
      if (vec->size() == 0) {
        Serial.println("The list is now empty. Ending removal.");
        done = true;
      }
    }

    else {
      Serial.println("That value is not in the list!");
    }
  }
}

void inputUniqueValuesIntoVector(Vector<int>* vec, const String& prompt) {
  // prompts the user to enter values to insert into a vector.
  // the values must be unique. this function uses the
  // intInputNotInVector() function to ensure unique values
  // are entered. that function utilizes integerInput(),
  // which bounds allowed input such that MIN_PIN <= input <= MAX_PIN

  // intInputNotInVector() returns -1 when user inputs 'DONE' (for when
  // they are done inputting values)

  int input = 0; // temporary default value, will be overriden

  Serial.println("Enter unique values for "+prompt+". (Enter 'DONE' when finished)");
  while (input != -1) { // go until user inputs 'DONE'
    Serial.println("Current values:");
    printVector(*vec);
    Serial.println();
    Serial.println();
    
    input = intInputNotInVector(*vec);

    // if input is valid add to vector
    if (input != -1) {
      vec->push_back(input);
      pinsUsed.push_back(input); // keeps track of all pins used
    }
  }
}

int intInputNotInVector(const Vector<int>& vec) {
  // prompts the user to input an integer that is not
  // present in the given vector 'vec'.

  // note that integerInput() bounds the allowed input
  // such that MIN_PIN <= input <= MAX_PIN

  // returns -1 if user inputs 'DONE'

  int input = -1; // just a default value, will be overriden
  bool valid_input = false;

  // repeat until user enters a valid value
  while (!valid_input) {
    input = integerInput();

    // if input == -1, user inputted 'DONE' in integerInput()
    if (input == -1) {
      return -1;
    }

    // check if input is in vector (or if it has been used in at all already)
    if (vectorContains(vec, input) || vectorContains(pinsUsed, input)) {
      Serial.print("Already entered! Try again! ");
    }

    // value not in vector, so we keep it
    else {
      valid_input = true;
    }
  }
  
  return input;
}

int boundedIntegerInput(int min, int max) {
  // prompts the user to enter an integer, then waits until input
  // is available in the serial. the input is read as a string and
  // the program checks whether it is an integer or not. if it is not,
  // the user must reinput.

  // for the purpose of this program, it is assumed that an input of
  // 0 is invalid. .toInt() returns 0 if an invalid string is passed in.

  // input is bounded such that MIN_PIN <= input <= MAX_PIN

  // returns -1 if user inputs 'DONE'

  int int_input;
  bool valid_input = false;

  Serial.print("Enter an integer ("+String(min)+"-"+String(max)+"): ");
  while (!valid_input) {

    while (Serial.available() == 0) {
      // wait until something is entered
    }

    // read serial input as a string and give feedback to user
    String inputString = Serial.readString();
    Serial.println(inputString);

    // check if user inputted 'DONE'
    if (inputString == "DONE") {
      return -1;
    }

    // check if input is an integer
    if (inputString.toInt() != 0 || inputString == "0") {
      int_input = inputString.toInt();

      // ensure that the input is in a valid range
      if (min <= int_input && int_input <= max)
        valid_input = true;

      else
        Serial.print("Value not in range! Try again: ");
    }
    else {
      Serial.print("Invalid! Try again: ");
    }

  }

  return int_input;
}

int integerInput() {
  // prompts the user to enter an integer, then waits until input
  // is available in the serial. the input is read as a string and
  // the program checks whether it is an integer or not. if it is not,
  // the user must reinput.

  // for the purpose of this program, it is assumed that an input of
  // 0 is invalid. .toInt() returns 0 if an invalid string is passed in.

  // input is bounded such that MIN_PIN <= input <= MAX_PIN

  // returns -1 if user inputs 'DONE'

  int int_input;
  bool valid_input = false;

  Serial.print("Enter an integer ("+String(MIN_PIN)+"-"+String(MAX_PIN)+"): ");
  while (!valid_input) {

    while (Serial.available() == 0) {
      // wait until something is entered
    }

    // read serial input as a string and give feedback to user
    String inputString = Serial.readString();
    Serial.println(inputString);

    // check if user inputted 'DONE'
    if (inputString == "DONE") {
      return -1;
    }

    // check if input is an integer
    if (inputString.toInt() != 0) {
      int_input = inputString.toInt();

      // ensure that the input is in a valid range
      if (MIN_PIN <= int_input && int_input <= MAX_PIN)
        valid_input = true;

      else
        Serial.print("Value not in range! Try again: ");
    }
    else {
      Serial.print("Invalid! Try again: ");
    }

  }

  return int_input;
}

// --- VECTOR FUNCTIONS ---

bool vectorContains(const Vector<int>& vec, int target) {
  // performs linear search on a vector 'vec' to ensure the
  // value 'target' exists in the vector. returns true if the value
  // is found, false otherwise.

  for (int i = 0; i < vec.size(); i++) {
    if (vec.at(i) == target) {
      return true;
    }
  }
  return false;
}

void printVector(const Vector<int>& vec) {
  // prints a vector using the format 'vec[0], vec[1], ... , and vec[size-1]'

  for (int i = 0; i < vec.size(); i++) {
    Serial.print(vec.at(i));

    // add commas between elements (and avoid placing one after last element)
    if (i < vec.size()-2 && vec.size() > 2) {
      Serial.print(", ");
    }
    else if (i == vec.size()-2) { // add and before last element
      
      // add comma before and if theres at least 3 elements
      if (vec.size() >= 3) { 
        Serial.print(",");

      }

      Serial.print(" and ");
    }
  }

}

int vectorIndexOf(const Vector<int>& vec, int target) {
  // returns first index of 'target' in vector 'vec'.
  // returns -1 if 'target' was not found in 'vec'

  for (size_t i = 0; i < vec.size(); i++) {
    if (vec.at(i) == target) {
      return i;
    }
  }

  return -1;
}

void sortVectorIncrementally(Vector<int>* vec) {
  // sorts a vector to be in increasing order.
  // this function uses the bubble sort algorithm.
  // as such, it has a time complexity of O(n^2) and space complexity O(1).
  // due to the small vector size (size <= 11), a simple sorting algorithm
  // will do for this project.

  bool swapMade = false;

  for (int j = 0; j < vec->size() - 1; j++) {

    swapMade = false; // reset swaps because we are starting a new pass of bubble sort

    // perform a pass of bubble sort
    for (int i = 0; i < vec->size() - 1 - j; i++) {
      int firstElem = vec->at(i);
      int secElem = vec->at(i+1);

      if (firstElem > secElem) {
        vec->at(i+1) = firstElem;
        vec->at(i) = secElem;
        swapMade = true;
      }

    }
    if (!swapMade) // return early if no swap was made (small optimization)
      return;
  }
}

// --- STATE MACHINE STATES ---

int mainMenu() {
  // prompts the user to advance to one of the following 3 states:
  // 1. edit input pins
  // 2. edit output pins
  // 3. attempt to create truth table

  Serial.println("Welcome to IC Tester");
  Serial.println("========================================");
  Serial.println("Select an option by entering its number:");
  Serial.println("  1. Edit Input Pins");
  Serial.println("  2. Edit Output Pins");
  Serial.println("  3. Create Truth Table");

  // get the input from the selection above
  int input = boundedIntegerInput(1, 3);

  // map input to next state
  int nextState = 0;
  switch(input) {
    case 1:
      nextState = EDIT_INPUT_PINS;
      break;
    case 2:
      nextState = EDIT_OUTPUT_PINS;
      break;
    case 3:
      nextState = VALIDATE_PINS;
      break;
  }

  return nextState;
}

int editInputPins() {
  // prompts the user to advance to one of the following 4 states:
  // 1. add pins
  // 2. remove pins
  // 3. sort pins
  // 4. reset pins

  // NOTE: editOutputPins() sends user to same states

  parentFunction = "Input";

  Serial.println("You selected 'Edit Input Pins'");
  Serial.println("========================================");
  Serial.println("Select an option by entering its number:");
  Serial.println("  0. Return to Main Menu");
  Serial.println("  1. Check Pins");
  Serial.println("  2. Add Pins");
  Serial.println("  3. Remove Pins");
  Serial.println("  4. Sort Pins");
  Serial.println("  5. Reset Pins");

  // get the input from the selection above
  int input = boundedIntegerInput(0, 5);

  // map input to next state
  int nextState = 0;
  switch(input) {
    case 0:
      nextState = MAIN_MENU;
      break;
    case 1:
      nextState = CHECK_PINS;
      break;
    case 2:
      nextState = ADD_PINS;
      break;
    case 3:
      nextState = REMOVE_PINS;
      break;
    case 4:
      nextState = SORT_PINS;
      break;
    case 5:
      nextState = RESET_PINS;
      break;
  }

  return nextState;
}

int editOutputPins() {
  // prompts the user to advance to one of the following 4 states:
  // 1. add pins
  // 2. remove pins
  // 3. sort pins
  // 4. reset pins

  // NOTE: editInputPins() sends user to same states

  parentFunction = "Output";

  Serial.println("You selected 'Edit Output Pins'");
  Serial.println("========================================");
  Serial.println("Select an option by entering its number:");
  Serial.println("  0. Return to Main Menu");
  Serial.println("  1. Check Pins");
  Serial.println("  2. Add Pins");
  Serial.println("  3. Remove Pins");
  Serial.println("  4. Sort Pins");
  Serial.println("  5. Reset Pins");

  // get the input from the selection above
  int input = boundedIntegerInput(0, 5);

  // map input to next state
  int nextState = 0;
  switch(input) {
    case 0:
      nextState = MAIN_MENU;
      break;
    case 1:
      nextState = CHECK_PINS;
      break;
    case 2:
      nextState = ADD_PINS;
      break;
    case 3:
      nextState = REMOVE_PINS;
      break;
    case 4:
      nextState = SORT_PINS;
      break;
    case 5:
      nextState = RESET_PINS;
      break;
  }

  return nextState;
}

// sub menus
int checkPins() {
  // shows the user the selected pins for the menu they
  // just came from

  // these values depend on the previous state
  Vector<int>* pointer;
  String prompt;
  int nextState;

  // parentFunction was set in the previous state so we use that
  // to determine the behavior of this function
  if (parentFunction == "Input") {
    pointer = &inputs;
    prompt = "input pins";
    nextState = EDIT_INPUT_PINS;
  }

  else {
    pointer = &outputs;
    prompt = "output pins";
    nextState = EDIT_OUTPUT_PINS;
  }

  // show user selected pins
  Serial.println("Selected "+parentFunction+" pins:");
  printVector(*pointer);
  Serial.println();

  delay(PAUSE_DELAY); // delay before going back to menu

  // return user back to the menu where they came from
  return nextState;
}

int addPins() {
  // depending on which branch this function was called from,
  // this will add pins to the input pins vector or the output
  // pins vector until the user enters DONE

  // these values depend on the previous state
  Vector<int>* pointer;
  String prompt;
  int nextState;

  // parentFunction was set in the previous state so we use that
  // to determine the behavior of this function
  if (parentFunction == "Input") {
    pointer = &inputs;
    prompt = "input pins";
    nextState = EDIT_INPUT_PINS;
  }

  else {
    pointer = &outputs;
    prompt = "output pins";
    nextState = EDIT_OUTPUT_PINS;
  }
  
  // redirect user to the inputting function
  inputUniqueValuesIntoVector(pointer, prompt);
  Serial.println();

  // show user selected pins
  Serial.println("Selected "+parentFunction+" pins:");
  printVector(*pointer);
  Serial.println();

  delay(PAUSE_DELAY); // delay before going back to menu

  // return user back to the menu where they came from
  return nextState;
}

int removePins() {
  // depending on which branch this function was called from,
  // this will remove pins from the input pins vector or the output
  // pins vector until the user enters DONE (or the vector is empty)

  // these values depend on the previous state
  Vector<int>* pointer;
  String prompt;
  int nextState;

  // parentFunction was set in the previous state so we use that
  // to determine the behavior of this function
  if (parentFunction == "Input") {
    pointer = &inputs;
    prompt = "input pins";
    nextState = EDIT_INPUT_PINS;
  }

  else {
    pointer = &outputs;
    prompt = "output pins";
    nextState = EDIT_OUTPUT_PINS;
  }

  // redirect user to the inputting function
  repeatedRemoveFromVector(pointer, prompt);
  Serial.println();

  // show user selected pins
  Serial.println("Selected "+parentFunction+" pins:");
  printVector(*pointer);
  Serial.println();

  delay(PAUSE_DELAY); // delay before going back to menu

  // return user back to the menu where they came from
  return nextState;
}

int sortPins() {
  // to make connections easier, this function allows the user to sort
  // their pins. the sorting algorithm is an optimized bubble sort.

  // these values depend on the previous state
  Vector<int>* pointer;
  int nextState;

  // parentFunction was set in the previous state so we use that
  // to determine the behavior of this function
  if (parentFunction == "Input") {
    pointer = &inputs;
    nextState = EDIT_INPUT_PINS;
  }

  else {
    pointer = &outputs;
    nextState = EDIT_OUTPUT_PINS;
  }

  // pass the respective vector into the sorting algorithm
  sortVectorIncrementally(pointer);

  // show user sorted pins
  Serial.println("Sorted "+parentFunction+" pins:");
  printVector(*pointer);
  Serial.println();

  delay(PAUSE_DELAY); // delay before going back to menu

  // return user back to the menu where they came from
  return nextState;
}

int resetPins() {
  // wipes all pins selected in the respective pin vector

  // these values depend on the previous state
  Vector<int>* pointer;
  int nextState;

  // parentFunction was set in the previous state so we use that
  // to determine the behavior of this function
  if (parentFunction == "Input") {
    pointer = &inputs;
    nextState = EDIT_INPUT_PINS;
  }

  else {
    pointer = &outputs;
    nextState = EDIT_OUTPUT_PINS;
  }

  // clear the vector
  pointer->clear();
  Serial.println("Cleared "+parentFunction+" pins");

  delay(PAUSE_DELAY); // delay before going back to menu

  // return user back to the menu where they came from
  return nextState;
}

// execution states
int validatePins() {
  // performs some validations that could become problematic if not resolved
  // before going onto the truth table. for example, ensures at least 1 input
  // and 1 output pin have been selected

  int nextState = -1; // temporary value, will be overriden

  if (inputs.size() == 0) {
    Serial.println("You must have selected at least 1 input pin!");
    nextState = MAIN_MENU;
  }
  if (outputs.size() == 0) {
    Serial.println("You must have selected at least 1 output pin!");
    nextState = MAIN_MENU;
  }

  if (nextState == -1) { // if no validation checks failed and nextState didnt change
    Serial.println("Configuration passed verification.");
    nextState = ADJUST_PIN_MODES;
  }

  return nextState;
}

int adjustPinModes() {
  // goes through all selected pins and assigns their pinMode() to either
  // input or output

  // inputs
  for (size_t i = 0; i < inputs.size(); i++) {
    pinMode(inputs.at(i), OUTPUT);
  }

  // outputs
  for (size_t i = 0; i < outputs.size(); i++) {
    pinMode(outputs.at(i), INPUT);
  }

  return CREATE_TRUTH_TABLE;
}

int createTruthTable() {
  // passes in all binary combinations from 0 -> 2^n-1 (where
  // n is the number of input pins selected) and samples the output
  // of the IC chip for each binary combination

  // FOR TESTING
  // printVector(inputs);
  // Serial.println();
  // printVector(outputs);
  // Serial.println();

  // print first row of truth table
  // this loop shows the input pins
  for (size_t i = 0; i < inputs.size(); i++) {
    Serial.print(" "+String(inputs.at(i)));
  }

  Serial.print(" |"); // division between inputs and outputs

  // this loop shows the output pins
  for (size_t i = 0; i < outputs.size(); i++) {
    Serial.print(" "+String(outputs.at(i)));
  }

  Serial.println(); // new line between lines

  // second line: 1 '-' for each character in line 1 + the 2 intermediate characters
  for (size_t i = 0; i < (inputs.size() + outputs.size() + 1) * 2; i++) {
    Serial.print("-");
  }

  Serial.println("-"); // add an extra one for symmetry

  // rest of table

  int max = 1 << inputs.size(); // 2^n
  for (size_t combination = 0; combination < max; combination++) { // 0 -> 2^n-1
    
    // take integer and split into bits, then power the specific input pins
    for (size_t i = 0; i < inputs.size(); i++) {
      int bit = (combination >> i) & 1;
      digitalWrite(inputs.at(i), bit ? HIGH : LOW);

      // print input part of row in truth table
      Serial.print(" "+String(bit));
    }

    // give some time for IC to settle
    delayMicroseconds(TRUTH_TABLE_DELAY);

    // division line between input and output
    Serial.print(" |");

    // sample output pins
    for (size_t i = 0; i < outputs.size(); i++) {
      int out = digitalRead(outputs.at(i));
      
      // print output part of row in truth table
      Serial.print(" "+String(out));
    }

    Serial.println(); // add a new line
  }

  Serial.println();

  delay(TRUTH_TABLE_RETURN_DELAY); // delay before returning to the main menu

  return MAIN_MENU;
}

// --- EXECUTION LOOP ---

void loop() {
  
  switch (currentState) {
    case MAIN_MENU:
      currentState = mainMenu();
      break;
    case EDIT_INPUT_PINS:
      currentState = editInputPins();
      break;
    case EDIT_OUTPUT_PINS:
      currentState = editOutputPins();
      break;
    case CHECK_PINS:
      currentState = checkPins();
      break;
    case ADD_PINS:
      currentState = addPins();
      break;
    case REMOVE_PINS:
      currentState = removePins();
      break;
    case SORT_PINS:
      currentState = sortPins();
      break;
    case RESET_PINS:
      currentState = resetPins();
      break;
    case VALIDATE_PINS:
      currentState = validatePins();
      break;
    case ADJUST_PIN_MODES:
      currentState = adjustPinModes();
      break;
    case CREATE_TRUTH_TABLE:
      currentState = createTruthTable();
      break;
  }

  Serial.println(); // new line to help separate output of each state
  //printVector(pinsUsed);
}

