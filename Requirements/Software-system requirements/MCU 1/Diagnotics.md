#Diagnostics requierements requirements

## DIAG-SYS-1 The diagnostics will be read from the message 0x14FFF111 (MSG_DiagnosticRequest) and the structure for reading will be the following:
## DIAG-SYS-2 Byte 1: Number of bytes to read
## /*The number of bytes to read are the number of valid values that should be considered when a diagnostic is sent, of example: 03 22 FF FF 00 00 00 00 */
## /*Byte 1 has the value of 0x03 which means, only the following 3 bytes are valid, the rest must be ignored*/

## DIAG-SYS-3 Byte 2: Service requested
## /*The system shall only have 2 types of services currently, read or write until a need of implementing more services are implemented*/

## DIAG-SYS-4 Byte 3-8: Diagnostic data
## /* Depending on the type of request, the read of bytes shall be set to the one used in byte 1, here the DIDs and parameters will be received */
## /* Currently there are no planned DID request or response with more than 6 bytes, but if done, a multiframe shall be implemented*/

##Service 0x22 Read data by ID 
### DID 7272 Log hours
#### DIAG-SYS-4 The software shall respond if the 