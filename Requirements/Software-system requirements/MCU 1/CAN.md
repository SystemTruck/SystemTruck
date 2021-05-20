# CAN requirements


## Wakeup:
#### CAN-SYS-5 The Signal_WakeupFrame shall have the CAN-ID of 0x140. 
#### CAN-SYS-6 The software shall be able to handle 2 WakeupMode modes:
####### - Accesory_WakeupMode (0x10)
####### - Full_WakeupMode (0x20)
#### CAN-SYS-1 The software shall wakeup after the Signal_WakeupFrame is received.
#### CAN-SYS-2 The software shall remain in Normal_State if Signal_WakeupFrame is being received with a frecuency of 100 ms.
#### CAN-SYS-3 The software shall go to Sleep_State if Signal_WakeupFrame is not being received after 100 ms.
#### CAN-SYS-4 The software shall detect the Signal_WakeupFrame as valid if the following is met:
####### - Byte_1 is equal to 0x62.
####### - Byte_2 is equal to 0x52.
####### - Byte_3 has a valid value of WakeupMode.

## TX CAN Messages

#### Message 0x128 MSG_EngineState.
#### CAN-SYS-8 The software shall send the message MSG_EngineState each 100 ms with the following information:
###### - Byte 1 Signal_EngineRunning. 
###### - Byte 2 Signal_EngineRunning_Inv.
###### - Byte 3 Signal_EngineTemp
#### CAN-SYS-9 The software shall set the signal signal_EngineRunning to true if:
##### CAN-SYS-11 - Signal_PwmMotorFoward has a value different from 0 OR
##### CAN-SYS-10 - Signal_PwmMotorBackwards has a value different from 0
#### CAN-SYS-12 The software shall set the signal Signal_EngineRunning_Inv if:
##### CAN-SYS-13 - Signal_VehicleSpeed has a value higher than 0 AND
##### CAN-SYS-14 - Signal_PwmMotorFoward OR Signal_PwmMotorBackwards has a value different from 0.
#### CAN-SYS-15 The software shall set the signal Signal_EngineTemp with the value of the internal signal Signal_ADCMotorTemp

#### Message 0x530 MSG_SteeringInfo
#### CAN-SYS-18 The software shall send the message MSG_EngineState each 50 ms with the following information:
###### - Byte 1 Signal_Steering_Direction.
###### - Byte 2 /*to do, add more signals for this message*/
#### CAN-SYS-17 The software shall set the signal Signal_Steering_Direction to DirLeft(1) if the Signal_PwmDirectionLeft has a value different from 0
#### CAN-SYS-16 The software shall set the Signal_Steering_Direction to DirRight(2) if the Signal_PwmDirectionRight has a value different from 0
#### CAN-SYS-19 /*to do, add more signals for this message*/

#### Message 0x14FF11F1 MSG_Diagnosticresponse
#### CAN-SYS-20 The message MSG_Diagnosticresponse shall be send as a reponse when receiving message MSG_DiagnosticRequest
#### NOTE: Supported diagnostics response structure shall be mentioned in the diagnostic module.


## RX CAN Messages
#### CAN-SYS-22 



#### Message 0x14FFF111 MSG_DiagnosticRequest
#### CAN-SYS-21 The message MSG_Diagnosticresponse shall received as an event when a diagnostic is requested.
#### NOTE: Supported diagnostics response structure shall be mentioned in the diagnostic module.