/* Purpose: This sketch uses ROS as well as MultiStepper, AccelStepper, and Servo libraries to control the
 * BCN3D Moveo robotic arm. In this setup, a Ramps 1.4 shield is used on top of an Arduino Mega 2560.
 * Subscribing to the following ROS topics: 1) joint_steps, 2) gripper_angle
 *    1) joint_steps is computed from the simulation in PC and sent Arduino via rosserial.  It contains
 *       the steps (relative to the starting position) necessary for each motor to move to reach the goal position.
 *    2) gripper_angle contains the necessary gripper angle to grasp the object when the goal state is reached
 *
 * Publishing to the following ROS topics: joint_steps_feedback
 *    1) joint_steps_feedback is a topic used for debugging to make sure the Arduino is receiving the joint_steps data
 *       accurately
 *
 * Author: Jesse Weisberg
 */

#if (ARDUINO >= 100)
  #include <Arduino.h>
#else
  #include <WProgram.h>
#endif
#include <ros.h>

// #include <moveo_moveit/ArmJointState.h>
#include <Servo.h>
#include <std_msgs/Bool.h>
#include <std_msgs/String.h>
#include <math.h>
#include <std_msgs/Int16.h>
#include <std_msgs/UInt16.h>
#include <AccelStepper.h>
#include <MultiStepper.h>
#include <sensor_msgs/JointState.h>
// Joint 1
#define E0_STEP_PIN        26
#define E0_DIR_PIN         28
#define E0_ENABLE_PIN      24

// Joint 2
#define Z_STEP_PIN         46
#define Z_DIR_PIN          48
#define Z_ENABLE_PIN       62
#define Z_MIN_PIN          18
#define Z_MAX_PIN          -1
// Joint 3
#define Y_STEP_PIN         60
#define Y_DIR_PIN          61
#define Y_ENABLE_PIN       56
#define Y_MIN_PIN          14
#define Y_MAX_PIN          -1
// Joint 4
#define X_STEP_PIN         54
#define X_DIR_PIN          55
#define X_ENABLE_PIN       38
#define X_MIN_PIN           3
#define X_MAX_PIN          -1

// Joint 5
#define E1_STEP_PIN        36
#define E1_DIR_PIN         34
#define E1_ENABLE_PIN      30


AccelStepper joint1(1,E0_STEP_PIN, E0_DIR_PIN);
AccelStepper joint2(1,Z_STEP_PIN, Z_DIR_PIN);
AccelStepper joint3(1,Y_STEP_PIN, Y_DIR_PIN);
AccelStepper joint4(1,X_STEP_PIN, X_DIR_PIN);
AccelStepper joint5(1, E1_STEP_PIN, E1_DIR_PIN);

Servo gripper;
MultiStepper steppers;

int joint_step[6];
int joint_status = 0;

ros::NodeHandle nh;
// std_msgs::Int16 msg;

//instantiate publisher (for debugging purposes)
// ros::Publisher steps("joint_steps_feedback",&msg);

// void arm_cb(const moveo_moveit::ArmJointState& arm_steps){
//   joint_status = 1;
//   joint_step[0] = arm_steps.position1;
//   joint_step[1] = arm_steps.position2;
//   joint_step[2] = arm_steps.position3;
//   joint_step[3] = arm_steps.position4;
//   joint_step[4] = arm_steps.position5;
//   joint_step[5] = arm_steps.position6; //gripper position <0-180>
// }
void arm_cb2(const sensor_msgs::JointState& arm_steps){
  
  // joint_step[0] += -10;
  // joint_step[1] = 0;
  // joint_step[2] = 0;
  // joint_step[3] = 0;
  // joint_step[4] = 0;
  long positions[5];  // Array of desired stepper positions must be long
  positions[0] = (int)(arm_steps.position[0]); // negated since the real robot rotates in the opposite direction as ROS
  positions[1] = (int)(arm_steps.position[1]);
  positions[2] = -(int)(arm_steps.position[2]);
  positions[3] = (int)(arm_steps.position[3]);
  positions[4] = (int)(arm_steps.position[4]);
    

    steppers.moveTo(positions);
    nh.spinOnce();
    steppers.runSpeedToPosition(); // 
  // joint_step[5] = arm_steps.position[6]; //gripper position <0-180>
}
void easy_cb(const std_msgs::Int16& hello){
  joint_status=1;
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(1000);                       // wait for a second
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
    delay(1000); 
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(1000);                       // wait for a second
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
    delay(1000); 
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(1000);                       // wait for a second
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
    delay(1000); 
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(1000);                       // wait for a second
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
    delay(1000); 

}
// void gripper_cb( const std_msgs::UInt16& cmd_msg){
//   gripper.write(cmd_msg.data); // Set servo angle, should be from 0-180
//   digitalWrite(13, HIGH-digitalRead(13));  // Toggle led
// }

//instantiate subscribers
// ros::Subscriber<moveo_moveit::ArmJointState> arm_sub("joint_steps",arm_cb); //subscribes to joint_steps on arm
ros::Subscriber<sensor_msgs::JointState> arm_sub2("joint_steps",arm_cb2); //subscribes to joint_steps on arm
ros::Subscriber<std_msgs::Int16> ledsub("led",easy_cb);
// ros::Subscriber<std_msgs::UInt16> gripper_sub("gripper_angle", gripper_cb); //subscribes to gripper position
//to publish from terminal: rostopic pub gripper_angle std_msgs/UInt16 <0-180>

void setup() {
  //put your setup code here, to run once:
  //Serial.begin(57600);

  pinMode(13,OUTPUT);
  pinMode(E0_ENABLE_PIN,OUTPUT);
  pinMode(X_ENABLE_PIN,OUTPUT);
  pinMode(Y_ENABLE_PIN,OUTPUT);
  pinMode(Z_ENABLE_PIN,OUTPUT);
  pinMode(E1_ENABLE_PIN,OUTPUT);
  joint_status = 1;

  nh.initNode();
  // nh.subscribe(arm_sub);
  nh.subscribe(arm_sub2);
  nh.subscribe(ledsub);
  // nh.subscribe(gripper_sub);
  // nh.advertise(steps);

  // Configure each stepper
  joint1.setMaxSpeed(100);
  joint2.setMaxSpeed(500);
  joint3.setMaxSpeed(10000);
  joint4.setMaxSpeed(1000);
  joint5.setMaxSpeed(1000);

  // Then give them to MultiStepper to manage
  steppers.addStepper(joint1);
  steppers.addStepper(joint2);
  steppers.addStepper(joint3);
  steppers.addStepper(joint4);
  steppers.addStepper(joint5);

  // Configure gripper servo
  // gripper.attach(11);
 
  //toggle led
}

void loop() {
  if (joint_status == 1) // If command callback (arm_cb) is being called, execute stepper command
  { 
    // long positions[5];  // Array of desired stepper positions must be long
    // positions[0] = joint_step[0]; // negated since the real robot rotates in the opposite direction as ROS
    // positions[1] = -joint_step[1]; 
    // positions[2] = joint_step[2]; 
    // positions[3] = joint_step[3]; 
    // positions[4] = -joint_step[4]; 
    

    // steppers.moveTo(positions);
    nh.spinOnce();
    // steppers.runSpeedToPosition(); // Blocks until all are in position
    // gripper.write(joint_step[5]);  // move gripper after manipulator reaches goal   
  }
     
  joint_status = 0;
  
  nh.spinOnce();
  delay(1);
  
}