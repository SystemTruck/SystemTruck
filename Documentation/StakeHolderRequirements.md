# Stakeholder requirements

## Actuators:
#### AT-1 The truck must have the ability to drive forward and in reverse
#### AT-10 The truck must have an analogic control system for velocity
#### AT-2 The truck must have the ability to change direction while going forward, reverse or being idle
#### AT-3 The truck must have right and left turn lights. 'Auto' and 'Manual' configurations
#### AT-9 The truck must have headlights. 'On', 'Off' and 'Auto' (based on light sensor) configurations
#### AT-4 The truck must have lights on the roof. 'On', 'Off' and 'Auto' (based on light sensor) configurations
#### AT-5 The truck must have the ability to load and unload the semi trailer
#### AT-6 The truck must have the ability to play audio signals stored in nonvolatile memory
#### AT-9 While reversing, the truck must inform the user of proximity to a rear obstacle using a beeping sound. 'On' and 'Off' configurations
#### AT-7 The truck must stop automatically if an obstacle is detected in its path while driving forward. 'On' and 'Off' configurations
#### AT-8 The truck must stop automatically if the weight of the load exceeds a configurable value


## Communication:
#### COM-1 The truck must communicate via Radiofrequency to a remote controller
#### COM-2 The truck must communicate via Bluetooth to an app used for diagnostics
#### COM-3 The different truck modules must communicate to each other via CAN bus
#### COM-4 The truck firmware must be updateable (either through bluetooth app or physical connection)
#### COM-6 The RF controller must have a touch-screen display with a simple interface to monitor the truck's status and sensors and to control the external LCD display
#### COM-5 The truck must have an external LCD display on the front used to provide insight into the truck's current task or status


## Sensors:
#### SNS-1 The truck must have the ability to take pictures or videos and store them in an SD memory
#### SNS-2 The truck must have the ability to record audio and store it in an SD memory
#### SNS-3 The truck must sense and display the temperature
#### SNS-4 The truck must sense and display current velocity
#### SNS-5 The truck must log its GPS location and store this information in nonvolatile memory for later access through diagnostics
#### SNS-6 The truck must sense acceleration power and should be accessible through diagnostics
#### SNS-7 The truck must log active hours and store this information in nonvolatile memory for later access through diagnostics
#### SNS-8 The truck must have an odometer (meter sensitive) (configureable to display in meters or feet)
#### SNS-9 The truck must sense and communicate the weight of the load
#### SNS-10 The truck must sense ambient lighting through a light sensor on its roof
#### SNS-11 The truck must have a LiDAR sensor on the front AND in the back to detect proximity to obstacles


## Other:
#### OTR-1 The truck must use as little energy as possible
#### OTR-2 The truck must have a rechargeable battery