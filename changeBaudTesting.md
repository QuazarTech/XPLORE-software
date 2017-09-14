# ``XPLORE Streamer``

### Feature

Version : 2.4.0
================

	Nature       : Feature
	Requested by : Quazar Research

	Status       : Pending
	Created on   : 2017-09-09 by KC
	Closed on    : ---

Description
-----------

The SMU is to stream voltmeter data at a specified frequency. This requires following features to be implemented.

	1) Dynamically change baudrate to suit data stream.

		-- CHANGE_BAUD

	2) Start recording

		-- START_REC : Starts recording of timed data into internal buffer.

	3) Transfer recorded data chunks to computer

		-- REC_SIZE : Queries number of available data points in buffer

		-- REC_DATA : Transfers requested number of data points to driver.

	5) Stop recording

		-- STOP_REC : Stops recording gracefully.

		-- KEEP_ALIVE : Stops recording if keep alive packets are not received, i.e. driver is dead.

### Modifications

##### Firmware
--------------

* Comm.h/Comm.cxx:

	^^ COMM_OPCODE
		++ COMM_OPCODE_CHANGE_BAUD

	++ class CommPacket_ChangeBaud
		++ class CommRequest_ChangeBaud
		++ class CommResponse_ChangeBaud

	^^ COMM_CBCODE
		++ COMM_CBCODE_CHANGE_BAUD

	++ class CommCB_ChangeBaud

	^^ union CommCB_Union
		++ char gen4 [sizeof (CommCB_ChangeBaud)]

	^^ class Comm : public Applet
		++ void changeBaud_CB (void*, uint16_t)
		++ void transmit_changeBaud (uint16_t)
--------------

##### Software
--------------

* virtuaSMU.h/virtuaSMU.cxx:

	^^ class Driver
		++ uint32_t baudRate_
		++ void changeBaud (uint32_t*, float*);
		++ void changeBaudCB (CommCB*);
--------------

* libxsmu.h/libxsmu.cxx/libxsmu.i:

	++ void changeBaud (int, unsigned int, float,
				unsigned int*, float*);

--------------

* Comm.h/Comm.cxx:

	^^ COMM_OPCODE
		++ COMM_OPCODE_CHANGE_BAUD

	++ class CommPacket_ChangeBaud
		++ class CommRequest_ChangeBaud
		++ class CommResponse_ChangeBaud

	^^ COMM_CBCODE
		++ COMM_CBCODE_CHANGE_BAUD

	++ class CommCB_ChangeBaud

	^^ union CommCB_Union
		++ char gen4 [sizeof (CommCB_ChangeBaud)]

	^^ class Comm : public Applet
		++ void changeBaud_CB (void*, uint16_t)
		++ void transmit_changeBaud (uint32_t)

--------------

++ changeBaud.py

### Test-1

- Burn modified Firmware onto the SMU microcontroller (Follow : `XPLORE-firmware` README )
- Install modified software on computer (Follow : `XPLORE-software` README)

- Testing `changeBaud(baudRate)` function
  - Testing : Communication Channel
  - Navigate to `XPLORE-software/XSMU-2.4.0/wrapper/python/tests/`
  - Run `python changeBaud.py`
  - Expected Result : Control should return back to python within specified timeout (1 second)

##### Result : Failed

- Output : Communication Timeout in changeBaud

---

##### Debugging

- Add print statements to XPLORE-software part and use LCD to print from the XPLORE-firmware part

^^ virtuaSMU.cxx
	^^ void Driver::changeBaudCB (const CommCB*)
		++ std::cout << "virtuaSMU : Entering changeBaudCB" << std::endl;
		++ std::cout << "virtuaSMU : Retrieved baudRate" << std::endl;
		++ std::cout << "virtuaSMU : AckBits Set" << std::endl;
		
	^^ void Driver::changeBaud (uint32_t*, float*)
		++ std::cout << "virtuaSMU : AckBits Reset" << std::endl;
		++ std::cout << "virtuaSMU : Transmitted baudRate, Starting wait for response" << std::endl;
		++ std::cout << "virtuaSMU : Recieved Response" << std::endl;
		
^^ Comm.cxx
	^^ void Comm::changeBaudCB (const void* data, uint16_t size)
		++ std::cout << "Comm : Packet Size Smaller than Expected" << std::endl;
		++ std::cout << "Comm : Packet Size Okay" << std::endl;
		++ std::cout << "Comm : Callback Completed" << std::endl;

	^^ void Comm::transmit_changeBaud (uint32_t baudRate)
		++ std::cout << "Comm : QP4_Packet allocated" << std::endl;
		++ std::cout << "Comm : Packet Sealed and Transmitted" << std::endl;
	
	^^ checkRecieveQueue()
		++ std::cout << "Comm : Recieved Data Size : " << rxsize << std::endl;

- Looked at XPLORE-firmware Changelog, and realised that I haven't written functions to interface the firmware's (XPLORE-firmware) Comm.cxx to the driver (XPLORE-software). Adding the required functions in XPLORE-firmware, and updating its Changelog.

- Also changing `timeout` to 5 seconds in `XPLORE-software/wrapper/python/test/changeBaud.py`

- Redoing **Test-1** with the above mentioned modifications.

##### Result : Communication Timeout

- Output : Communication Timeout in changeBaud

---

- Adding print statements increased the execution time, and the default timeout of 1 second is not sufficient. Retrying with the all print statements commited out.

##### Result : Communication Timeout

- Output : Communication Timeout in changeBaud

---

- 	Added LCD print statements to firmware (Application.cxx) inside Application::changeBaudCB() function
	
	```
	freezeLocalDisplay();
	lcd.cursorAt (0, 0);
	lcd << "    Entering    ";

	lcd.cursorAt (1, 0);
	lcd << "  changeBaudCB  ";


	const CommCB_changeBaud* o =
		reinterpret_cast<const CommCB_changeBaud*> (oCB);
	
	baudRate = o->baudRate();
	appComm.transmit_changeBaud (baudRate);
	
	
	freezeLocalDisplay();
	lcd.cursorAt (0, 0);
	lcd << "  Transmitted   ";

	lcd.cursorAt (1, 0);
	lcd << "   changeBaud   ";
	```
}

##### Result : Failed

- Output :

	```
	libxsmu version: 2.1.2
	Total device: 1
	Seial number: XSMU012A
	libxsmu version: 2.1.2
	Hardware version: 4.0.0
	Firmware version: 255.255.253
	Device ID     : 0 
	goodID        : 1 
	Remaining time: 0.887270927429 sec 

	virtuaSMU : AckBits Reset
	Comm : QP4_Packet allocated
	Comm : Packet Sealed and Transmitted
	virtuaSMU : Transmitted baudRate, Starting wait for response
	```

	On LCD -> "Entering changeBaudCB"
	On LCD -> "Transmitted changeBaud"

	```
	Baud Rate:  9600 
	Timeout:  0.0
	Communication timeout in changeBaud
	```
---

- Added print statements to VS_setVoltage functions to see internal working

##### Result : Failed

- Output : timeout error even in setVoltage functions, reason being printing these statements lead to execution time exceeding the timeout.
---

- Added print statement in virtuaSMU.cxx (software), to see the characters and corresponding hex numbers being transmitted.

##### Result : Timeout

- Output :

```
libxsmu version: 2.1.2
Total device: 1
Seial number: XSMU012A
libxsmu version: 2.1.2
51 Q
50 P
34 4
31 1
0 
2C ,
F9 �
12 
0 
1 
0 
0 
58 X
50 P
4C L
4F O
52 R
45 E
20  
53 S
4D M
55 U
0 
0 
0 
0 
0 
0 
0 
0 
0 
0 
0 
0 
0 
0 
0 
0 
0 
0 
0 
0 
0 
0 
0 
4 
0 
0 
FF �
FF �
FF �
FD �
Hardware version: 4.0.0
Firmware version: 255.255.253
Device ID     : 0 
goodID        : 1 
Remaining time: 0.889459848404 sec 

51 Q
50 P
34 4
31 1
0 
8 
FF �
30 0
0 
2B +
0 
0 
0 
0 
25 %
80 �
Baud Rate:  9600 
Timeout:  0.0
Communication timeout in changeBaud
```
---

```
libxsmu version: 2.1.2
Total device: 1
Seial number: XSMU012A
libxsmu version: 2.1.2
51 Q
50 P
34 4
31 1
0 
2C ,
F9 �
12 
0 
1 
0 
0 
58 X
50 P
4C L
4F O
52 R
45 E
20  
53 S
4D M
55 U
0 
0 
0 
0 
0 
0 
0 
0 
0 
0 
0 
0 
0 
0 
0 
0 
0 
0 
0 
0 
0 
0 
0 
4 
0 
0 
FF �
FF �
FF �
FD �
void smu::Comm::interpret(const void*, uint16_t):249:Opcode: 1
Hardware version: 4.0.0
Firmware version: 255.255.253
Device ID     : 0 
goodID        : 1 
Remaining time: 0.882127046585 sec 

void smu::Comm::transmit_changeBaud(uint32_t):1393:Comm : QP4_Packet allocated
void smu::Comm::transmit_changeBaud(uint32_t):1400:Comm : Packet Sealed and Transmitted
51 Q
50 P
34 4
31 1
0 
8 
FF �
30 0
0 
2B +
0 
0 
0 
0 
25 %
80 �
void smu::Comm::interpret(const void*, uint16_t):249:Opcode: 43
void smu::Comm::changeBaudCB(const void*, uint16_t):746:Comm : Packet Size Okay
void smu::Comm::changeBaudCB(const void*, uint16_t):752:Comm : Callback Completed
Baud Rate:  9600 
Timeout:  0.0
Communication timeout in changeBaud
```
---

```
libxsmu version: 2.1.2
Total device: 1
Seial number: XSMU012A
libxsmu version: 2.1.2
51 Q
50 P
34 4
31 1
0 
2C ,
F9 �
12 
0 
1 
0 
0 
58 X
50 P
4C L
4F O
52 R
45 E
20  
53 S
4D M
55 U
0 
0 
0 
0 
0 
0 
0 
0 
0 
0 
0 
0 
0 
0 
0 
0 
0 
0 
0 
0 
0 
0 
0 
4 
0 
0 
FF �
FF �
FF �
FD �
void smu::Comm::interpret(const void*, uint16_t):249:Opcode: 1
Hardware version: 4.0.0
Firmware version: 255.255.253
Device ID     : 0 
goodID        : 1 
Remaining time: 0.884266853333 sec 

libxsmu changeBaud 9600 5

void smu::Driver::changeBaud(uint32_t*, float*):1244:virtuaSMU : AckBits Reset
void smu::Comm::transmit_changeBaud(uint32_t):1393:Comm : QP4_Packet allocated
void smu::Comm::transmit_changeBaud(uint32_t):1400:Comm : Packet Sealed and Transmitted
void smu::Driver::changeBaud(uint32_t*, float*):1247:virtuaSMU : Transmitted baudRate, Starting wait for response
51 Q
50 P
34 4
31 1
0 
8 
FF �
30 0
0 
2B +
0 
0 
0 
0 
25 %
80 �
void smu::Comm::interpret(const void*, uint16_t):249:Opcode: 43
void smu::Comm::changeBaudCB(const void*, uint16_t):746:Comm : Packet Size Okay
void smu::Comm::changeBaudCB(const void*, uint16_t):752:Comm : Callback Completed

*Did not print lines in virtuaSMU.cxx changeBaudCB()*
- conclusion : that function is not being executed
- bug found : in virtuaSMU.cxx,, changeBaudCB() not in Callback list

*Waits here*


libxsmu timeout 9600 0
Baud Rate:  9600 
Timeout:  0.0
Communication timeout in changeBaud
```
---


```
libxsmu version: 2.1.2
Total device: 1
Seial number: XSMU012A
libxsmu version: 2.1.2
51 Q
50 P
34 4
31 1
0 
2C ,
F9 �
12 
0 
1 
0 
0 
58 X
50 P
4C L
4F O
52 R
45 E
20  
53 S
4D M
55 U
0 
0 
0 
0 
0 
0 
0 
0 
0 
0 
0 
0 
0 
0 
0 
0 
0 
0 
0 
0 
0 
0 
0 
4 
0 
0 
FF �
FF �
FF �
FD �
void smu::Comm::interpret(const void*, uint16_t):249:Opcode: 1
Hardware version: 4.0.0
Firmware version: 255.255.253
Device ID     : 0 
goodID        : 1 
Remaining time: 0.884332180023 sec 

libxsmu changeBaud 9600 5
void smu::Driver::changeBaud(uint32_t*, float*):1246:virtuaSMU : AckBits Reset
void smu::Comm::transmit_changeBaud(uint32_t):1393:Comm : QP4_Packet allocated
void smu::Comm::transmit_changeBaud(uint32_t):1400:Comm : Packet Sealed and Transmitted
void smu::Driver::changeBaud(uint32_t*, float*):1249:virtuaSMU : Transmitted baudRate, Starting wait for response
51 Q
50 P
34 4
31 1
0 
8 
FF �
30 0
0 
2B +
0 
0 
0 
0 
25 %
80 �
void smu::Comm::interpret(const void*, uint16_t):249:Opcode: 43
void smu::Comm::changeBaudCB(const void*, uint16_t):746:Comm : Packet Size Okay
void smu::Driver::changeBaudCB(const smu::CommCB*):581:virtuaSMU : Entering changeBaudCB
void smu::Driver::changeBaudCB(const smu::CommCB*):587:virtuaSMU : Retrieved baudRate
void smu::Driver::changeBaudCB(const smu::CommCB*):590:virtuaSMU : AckBits Set
void smu::Comm::changeBaudCB(const void*, uint16_t):752:Comm : Callback Completed
void smu::Driver::changeBaud(uint32_t*, float*):1253:virtuaSMU : Recieved Response
libxsmu success9600 4.91463
Baud Rate:  9600 
Timeout:  4.9146270752

```