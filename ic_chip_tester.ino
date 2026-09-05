#include <Vector.h>

// --- CONFIGURATION ---

#define BAUD_RATE 31250

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

void inputUniqueValuesIntoVector(Vector<int>* vec, String prompt) {
  // prompts the user to enter values to insert into a vector.
  // the values must be unique. this function uses the
  // intInputNotInVector() function to ensure unique values
  // are entered. that function utilizes integerInput(),
  // which bounds allowed input such that MIN_PIN <= input <= MAX_PIN

  // intInputNotInVector() returns -1 when user inputs 'DONE' (for when
  // they are done inputting values)

  int input = 0; // temporary default value, will be overriden

  Serial.println("Enter unique values for "+prompt);
  while (input != -1) { // go until user inputs 'DONE'
    input = intInputNotInVector(*vec);

    if (input != -1)
      vec->push_back(input);
      pinsUsed.push_back(input);
  }
}

int intInputNotInVector(Vector<int> vec) {
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

bool vectorContains(Vector<int> vec, int target) {
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

void printVector(Vector<int> vec) {
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
  Serial.println("  1. Add Pins");
  Serial.println("  2. Remove Pins");
  Serial.println("  3. Sort Pins");
  Serial.println("  4. Reset Pins");

  // get the input from the selection above
  int input = boundedIntegerInput(0, 4);

  // map input to next state
  int nextState = 0;
  switch(input) {
    case 0:
      nextState = MAIN_MENU;
    case 1:
      nextState = ADD_PINS;
      break;
    case 2:
      nextState = REMOVE_PINS;
      break;
    case 3:
      nextState = SORT_PINS;
      break;
    case 4:
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
  Serial.println("  1. Add Pins");
  Serial.println("  2. Remove Pins");
  Serial.println("  3. Sort Pins");
  Serial.println("  4. Reset Pins");

  // get the input from the selection above
  int input = boundedIntegerInput(0, 4);

  // map input to next state
  int nextState = 0;
  switch(input) {
    case 0:
      nextState = MAIN_MENU;
    case 1:
      nextState = ADD_PINS;
      break;
    case 2:
      nextState = REMOVE_PINS;
      break;
    case 3:
      nextState = SORT_PINS;
      break;
    case 4:
      nextState = RESET_PINS;
      break;
  }

  return nextState;
}

// sub menus
int addPins() {
  Serial.println("add pins");
  return 0;
}

int removePins() {
  Serial.println("remove pins");
  return 0;
}

int sortPins() {
  Serial.println("sort pins");
  return 0;
}

int resetPins() {
  Serial.println("reset pins");
  return 0;
}

// execution states
int validatePins() {
  Serial.println("validate pins");
  return 0;
}

int adjustPinModes() {
  Serial.println("adjust pin modes");
  return 0;
}

int createTruthTable() {
  Serial.println("create truth table");
  return 0;
}



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
}