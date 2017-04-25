
# These could be an enum

const int STATE_AUTO_RUNNING = 0;
const int STATE_AUTO_TRANSITIONING = 1;
const int STATE_BASIC = 2;
const int STATE_USERCONTROLLED = 3;

int currentState = STATE_AUTO_RUNNING;

const int SPEED_NORMAL = 75;

# These don't need to be ints

int currentSpeed = SPEED_NORMAL;
int desiredSpeed = 0;

const int DELAY_BETWEEN_TRANSITIONS = 5000;

long lastTransitionTime = 0;
int nextTransitionToNormal = 0;

const int SWITCH_HIGH = 0;
const int SWITCH_MIDDLE = 1;
const int SWITCH_LOW = 2;


// This is some magic for the particular three-way switch we used.
// You should replace this with whatever is appropriate for your hardware.

int getSwitchValue() {

  int pin2 = ! digitalRead(2);
  int pin3 = ! digitalRead(3);

  if (pin2 && pin3) {
    return SWITCH_MIDDLE;
  } else if (pin2) {
    return SWITCH_HIGH;
  }
  return SWITCH_LOW;
}

// the setup function runs once when you press reset or power the board
void setup() {
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(10, HIGH);
  Serial.begin(115200);
}


// the loop function runs over and over again forever
void loop() {

  switch (getSwitchValue()) {
    case SWITCH_HIGH:
      currentState = STATE_BASIC;
      break;
    case SWITCH_MIDDLE:
      currentState = STATE_USERCONTROLLED;
      break;
    case SWITCH_LOW:
      if ( (currentState != STATE_AUTO_TRANSITIONING) && (currentState != STATE_AUTO_RUNNING) ) {
        currentState = STATE_AUTO_RUNNING;
      }
      break;

  }

  int newValue = 0;
  int theValue = 0 ;

  switch (currentState) {

    case STATE_USERCONTROLLED:
      Serial.println("user controlled");

      theValue = analogRead(A0);
      Serial.println(theValue);
      newValue = map(theValue, 0, 1023, 50, 255);

// Do you want to be able to stop the plane entirely?
//      if (theValue < 5) {
//        newValue = 0;
//      }
      currentSpeed = newValue;

      break;

    case STATE_AUTO_RUNNING:
      Serial.print("auto running. current time ");
      Serial.print(millis());
      Serial.print(" and lastTransitionTime ");
      Serial.println(lastTransitionTime);

      if ( millis() - lastTransitionTime > DELAY_BETWEEN_TRANSITIONS) {
        lastTransitionTime = millis();
        currentState = STATE_AUTO_TRANSITIONING;
        digitalWrite(13, HIGH);
        if (nextTransitionToNormal) {
          desiredSpeed = SPEED_NORMAL;
        } else {
          desiredSpeed = random(50, 255);
        }
        nextTransitionToNormal = !nextTransitionToNormal;

      }

      break;
    case STATE_AUTO_TRANSITIONING:
      Serial.print("auto transitioning to ");
      Serial.print(desiredSpeed);
      Serial.print(";  currently at ");
      Serial.println(currentSpeed);

      if (currentSpeed <= desiredSpeed) {
        currentSpeed++;
      } else if (currentSpeed > desiredSpeed) {
        currentSpeed--;
      }

      if ( currentSpeed == desiredSpeed) {
        lastTransitionTime = millis();
        currentState = STATE_AUTO_RUNNING;
        digitalWrite(13, LOW);
      }

      break;

    case STATE_BASIC:
      Serial.println("basic");

      currentSpeed = SPEED_NORMAL;

    default:

      break;

  }
  analogWrite(5, currentSpeed);
  delay(50);                       // 20 updates/second
}
