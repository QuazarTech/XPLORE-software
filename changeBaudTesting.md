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

### Testng for keepAlive (wrapper/python/test/keepAlive.py)

- Result : Passed
-Output :
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
	Remaining time: 0.849823951721 sec

	51 Q
	50 P
	34 4
	31 1
	0
	8
	FF �
	C7 �
	0
	2 
	0
	0
	0
	0
	27 '
	10 
	Lease Time:  10000
	Timeout:  4.91435289383
	```


---

### Testing for async thread calling keepAlive() and locking mechanism in Comm
Added :
- PRINT_DEBUG ("Asynch thread launched") in open()
- PRINT_DEBUG ("Inside thread : Keep Alive sent") inside thread()
- PRINT_DEBUG ("Lock Acquired") in keepAlive, changeBaud, VS_setVoltage and VM_Read functions
- Output for `python keepAlive.py`
	```
	libxsmu version: 2.1.2
	Total device: 1
	Seial number: XSMU012A
	libxsmu version: 2.1.2
	Hardware version: 4.0.0
	Firmware version: 255.255.253
	void smu::Driver::open(const char*, float*):622:Asynch thread launched
	Device ID     : 0
	goodID        : 1
	Remaining time: 0.869105100632 sec

	void smu::Driver::keepAlive(uint32_t*, float*):719:Lock Acquired
	Lease Time:  10000
	Timeout:  4.89869022369
	void smu::Driver::keepAlive(uint32_t*, float*):719:Lock Acquired
	void smu::Driver::thread():735:Inside thread : Keep Alive sent
	```

- Output for `python changeBaud.py`
	```
	libxsmu version: 2.1.2
	Total device: 1
	Seial number: XSMU012A
	libxsmu version: 2.1.2
	Hardware version: 4.0.0
	Firmware version: 255.255.253
	void smu::Driver::open(const char*, float*):622:Asynch thread launched
	Device ID     : 0
	goodID        : 1
	Remaining time: 0.869477987289 sec

	void smu::Driver::changeBaud(uint32_t*, float*):1373:Lock Acquired
	Baud Rate:  9600
	Timeout:  4.9146399498
	void smu::Driver::keepAlive(uint32_t*, float*):719:Lock Acquired
	void smu::Driver::thread():735:Inside thread : Keep Alive sent
	```
- Output for `python VS_setVoltage.py 1`
	```
	libxsmu version: 2.1.2
	Total device: 1
	Seial number: XSMU012A
	libxsmu version: 2.1.2
	Hardware version: 4.0.0
	Firmware version: 255.255.253
	void smu::Driver::open(const char*, float*):622:Asynch thread launched
	Device ID     : 0
	goodID        : 1
	Remaining time: 0.850958108902 sec

	void smu::Driver::VS_setVoltage(float*, float*):944:Lock Acquired
	Voltage:  0.999929249287
	Timeout:  4.91469097137
	void smu::Driver::keepAlive(uint32_t*, float*):719:Lock Acquired
	void smu::Driver::thread():735:Inside thread : Keep Alive sent
	```
- Output for `python VM_Read.py` (Ended with `Ctrl+c`)
	```
	libxsmu version: 2.1.2
	Total device: 1
	Seial number: XSMU012A
	libxsmu version: 2.1.2
	Hardware version: 4.0.0
	Firmware version: 255.255.253
	void smu::Driver::open(const char*, float*):622:Asynch thread launched
	Device ID     : 0
	goodID        : 1
	Remaining time: 0.870882987976 sec

	void smu::Driver::VM_read(uint16_t*, float*, float*):1093:Lock Acquired
	voltage               : -0.101999998093
	Remaining time        : 1.13914418221 sec

	void smu::Driver::VM_read(uint16_t*, float*, float*):1093:Lock Acquired
	voltage               : -0.101999998093
	Remaining time        : 1.15525889397 sec

	void smu::Driver::VM_read(uint16_t*, float*, float*):1093:Lock Acquired
	voltage               : -0.101999998093
	Remaining time        : 1.17055797577 sec

	void smu::Driver::VM_read(uint16_t*, float*, float*):1093:Lock Acquired
	voltage               : -0.101999998093
	Remaining time        : 1.1550321579 sec

	void smu::Driver::VM_read(uint16_t*, float*, float*):1093:Lock Acquired
	voltage               : -0.101999998093
	Remaining time        : 1.17103505135 sec

	void smu::Driver::VM_read(uint16_t*, float*, float*):1093:Lock Acquired
	voltage               : -0.101999998093
	Remaining time        : 1.20280408859 sec

	void smu::Driver::VM_read(uint16_t*, float*, float*):1093:Lock Acquired
	voltage               : -0.101999998093
	Remaining time        : 1.15487217903 sec

	void smu::Driver::VM_read(uint16_t*, float*, float*):1093:Lock Acquired
	voltage               : -0.101999998093
	Remaining time        : 1.17105913162 sec

	void smu::Driver::VM_read(uint16_t*, float*, float*):1093:Lock Acquired
	voltage               : -0.101999998093
	Remaining time        : 1.20312213898 sec

	void smu::Driver::VM_read(uint16_t*, float*, float*):1093:Lock Acquired
	voltage               : -0.101999998093
	Remaining time        : 1.20294713974 sec

	void smu::Driver::VM_read(uint16_t*, float*, float*):1093:Lock Acquired
	voltage               : -0.101999998093
	Remaining time        : 1.15506291389 sec

	void smu::Driver::VM_read(uint16_t*, float*, float*):1093:Lock Acquired
	voltage               : -0.101999998093
	Remaining time        : 1.1709561348 sec

	void smu::Driver::VM_read(uint16_t*, float*, float*):1093:Lock Acquired
	voltage               : -0.101999998093
	Remaining time        : 1.20299291611 sec

	void smu::Driver::VM_read(uint16_t*, float*, float*):1093:Lock Acquired
	voltage               : -0.101999998093
	Remaining time        : 1.15482211113 sec

	void smu::Driver::VM_read(uint16_t*, float*, float*):1093:Lock Acquired
	voltage               : -0.101999998093
	Remaining time        : 1.17106795311 sec

	void smu::Driver::VM_read(uint16_t*, float*, float*):1093:Lock Acquired
	voltage               : -0.101999998093
	Remaining time        : 1.20310091972 sec

	void smu::Driver::VM_read(uint16_t*, float*, float*):1093:Lock Acquired
	voltage               : -0.101999998093
	Remaining time        : 1.20291304588 sec

	void smu::Driver::VM_read(uint16_t*, float*, float*):1093:Lock Acquired
	voltage               : -0.101999998093
	Remaining time        : 1.15489888191 sec

	void smu::Driver::VM_read(uint16_t*, float*, float*):1093:Lock Acquired
	voltage               : -0.101999998093
	Remaining time        : 1.17089796066 sec

	void smu::Driver::VM_read(uint16_t*, float*, float*):1093:Lock Acquired
	voltage               : -0.101999998093
	Remaining time        : 1.20286202431 sec

	void smu::Driver::VM_read(uint16_t*, float*, float*):1093:Lock Acquired
	voltage               : -0.101999998093
	Remaining time        : 1.15500307083 sec

	void smu::Driver::VM_read(uint16_t*, float*, float*):1093:Lock Acquired
	voltage               : -0.101999998093
	Remaining time        : 1.17052602768 sec

	void smu::Driver::VM_read(uint16_t*, float*, float*):1093:Lock Acquired
	voltage               : -0.101999998093
	Remaining time        : 1.20300483704 sec

	void smu::Driver::VM_read(uint16_t*, float*, float*):1093:Lock Acquired
	voltage               : -0.101999998093
	Remaining time        : 1.20285892487 sec

	void smu::Driver::VM_read(uint16_t*, float*, float*):1093:Lock Acquired
	voltage               : -0.101999998093
	Remaining time        : 1.15503001213 sec

	void smu::Driver::VM_read(uint16_t*, float*, float*):1093:Lock Acquired
	voltage               : -0.101999998093
	Remaining time        : 1.17059803009 sec

	void smu::Driver::VM_read(uint16_t*, float*, float*):1093:Lock Acquired
	voltage               : -0.101999998093
	Remaining time        : 1.20324611664 sec

	void smu::Driver::VM_read(uint16_t*, float*, float*):1093:Lock Acquired
	voltage               : -0.101999998093
	Remaining time        : 1.15489792824 sec

	void smu::Driver::VM_read(uint16_t*, float*, float*):1093:Lock Acquired
	voltage               : -0.101999998093
	Remaining time        : 1.17091703415 sec

	void smu::Driver::VM_read(uint16_t*, float*, float*):1093:Lock Acquired
	voltage               : -0.101999998093
	Remaining time        : 1.20329713821 sec

	void smu::Driver::VM_read(uint16_t*, float*, float*):1093:Lock Acquired
	voltage               : -0.101999998093
	Remaining time        : 1.20329809189 sec

	void smu::Driver::VM_read(uint16_t*, float*, float*):1093:Lock Acquired
	voltage               : -0.101999998093
	Remaining time        : 1.15476918221 sec

	void smu::Driver::VM_read(uint16_t*, float*, float*):1093:Lock Acquired
	voltage               : -0.101999998093
	Remaining time        : 1.17097902298 sec

	void smu::Driver::VM_read(uint16_t*, float*, float*):1093:Lock Acquired
	voltage               : -0.101999998093
	Remaining time        : 1.20291399956 sec

	void smu::Driver::VM_read(uint16_t*, float*, float*):1093:Lock Acquired
	voltage               : -0.101999998093
	Remaining time        : 1.15500283241 sec

	void smu::Driver::VM_read(uint16_t*, float*, float*):1093:Lock Acquired
	voltage               : -0.101999998093
	Remaining time        : 1.17109894753 sec

	void smu::Driver::VM_read(uint16_t*, float*, float*):1093:Lock Acquired
	voltage               : -0.101999998093
	Remaining time        : 1.20278191566 sec

	void smu::Driver::VM_read(uint16_t*, float*, float*):1093:Lock Acquired
	voltage               : -0.101999998093
	Remaining time        : 1.20280718803 sec

	void smu::Driver::VM_read(uint16_t*, float*, float*):1093:Lock Acquired
	voltage               : -0.101999998093
	Remaining time        : 1.15464901924 sec

	void smu::Driver::VM_read(uint16_t*, float*, float*):1093:Lock Acquired
	voltage               : -0.101999998093
	Remaining time        : 1.17091703415 sec

	void smu::Driver::VM_read(uint16_t*, float*, float*):1093:Lock Acquired
	voltage               : -0.101999998093
	Remaining time        : 1.20296692848 sec

	void smu::Driver::VM_read(uint16_t*, float*, float*):1093:Lock Acquired
	voltage               : -0.101999998093
	Remaining time        : 1.15501594543 sec

	void smu::Driver::VM_read(uint16_t*, float*, float*):1093:Lock Acquired
	^CTraceback (most recent call last):
	File "VM_getReading.py", line 40, in <module>
		voltage, timeout = libxsmu.VM_getReading (deviceID, filter_length, timeout)
	KeyboardInterrupt
	```
The output of this last test is not as expected. Since the keepAlive function is being called in a parallel thread, it should the Lock Acquired for keeAlive should show up in between the Lock Acquired for VM_read.


But if the range is changed from (0, 200) to (0, 1) in `VM_getReading.py`, the output is :

```
libxsmu version: 2.1.2
Total device: 1
Seial number: XSMU012A
libxsmu version: 2.1.2
Hardware version: 4.0.0
Firmware version: 255.255.253
void smu::Driver::open(const char*, float*):622:Async thread launched
Device ID     : 0
goodID        : 1
Remaining time: 0.832612991333 sec

void smu::Driver::VM_read(uint16_t*, float*, float*):1094:Lock Acquired
voltage               : -2.67028808594e-05
Remaining time        : 1.18702793121 sec

void smu::Driver::keepAlive(uint32_t*, float*):718:Trying to Acquire Lock
void smu::Driver::keepAlive(uint32_t*, float*):720:Lock Acquired
void smu::Driver::thread():736:Inside thread : Keep Alive sent
```
- Added std::launch::async as argument to std::async to always set asynchronous operation of thread.
- Fixed bug in `Driver::thread()` to convert ms into seconds for comparison inside while look
- Result : Passed
- Output :

	```
	libxsmu version: 2.1.2
	Total device: 1
	Seial number: XSMU012A
	void smu::Driver::open(const char*, float*):598:Opening Device
	libxsmu version: 2.1.2
	Hardware version: 4.0.0
	Firmware version: 255.255.253
	void smu::Driver::open(const char*, float*):624:Async thread launched
	Device ID     : 0
	goodID        : 1
	Remaining time: void smu::Driver::keepAlive(uint32_t*, float*)0.88267493248 sec :

	721:Trying to Acquire Lock
	void smu::Driver::keepAlive(uint32_t*, float*):723:Lock Acquired
	void smu::Driver::VM_read(uint16_t*, float*, float*):1096:Trying to Acquire Lock
	void smu::Driver::thread():void smu::Driver::VM_read(uint16_t*, float*, float*):739:1098:Lock Acquired
	Inside thread : Keep Alive sent
	void smu::Driver::thread():743:Inside thread : Alive 139
	voltage               : -2.47955322266e-05
	Remaining time        : 1.17120504379 sec

	void smu::Driver::VM_read(uint16_t*, float*, float*):1096:Trying to Acquire Lock
	void smu::Driver::VM_read(uint16_t*, float*, float*):1098:Lock Acquired
	voltage               : -2.86102294922e-05
	Remaining time        : 1.17108106613 sec

	void smu::Driver::VM_read(uint16_t*, float*, float*):1096:Trying to Acquire Lock
	void smu::Driver::VM_read(uint16_t*, float*, float*):1098:Lock Acquired
	voltage               : -2.86102294922e-05
	Remaining time        : 1.15503096581 sec

	void smu::Driver::VM_read(uint16_t*, float*, float*):1096:Trying to Acquire Lock
	void smu::Driver::VM_read(uint16_t*, float*, float*):1098:Lock Acquired
	void smu::Driver::keepAlive(uint32_t*, float*):721:Trying to Acquire Lock
	voltage               : -2.67028808594e-05
	Remaining time        : void smu::Driver::keepAlive(uint32_t*, float*)1.17121624947 sec

	:723:Lock Acquired
	void smu::Driver::VM_read(uint16_t*, float*, float*):1096:Trying to Acquire Lock
	void smu::Driver::thread():751:Inside thread : Keep Alive sent
	void smu::Driver::VM_read(uint16_t*, float*, float*):1098:Lock Acquired
	voltage               : -2.5749206543e-05
	Remaining time        : 1.16077613831 sec

	void smu::Driver::VM_read(uint16_t*, float*, float*):1096:Trying to Acquire Lock
	void smu::Driver::VM_read(uint16_t*, float*, float*):1098:Lock Acquired
	voltage               : -2.95639038086e-05
	Remaining time        : 1.17108201981 sec

	void smu::Driver::VM_read(uint16_t*, float*, float*):1096:Trying to Acquire Lock
	void smu::Driver::VM_read(uint16_t*, float*, float*):1098:Lock Acquired
	voltage               : -2.5749206543e-05
	Remaining time        : 1.1545920372 sec

	void smu::Driver::VM_read(uint16_t*, float*, float*):1096:Trying to Acquire Lock
	void smu::Driver::VM_read(uint16_t*, float*, float*):1098:Lock Acquired
	void smu::Driver::keepAlive(uint32_t*, float*):721:Trying to Acquire Lock
	voltage               : void smu::Driver::keepAlive(uint32_t*, float*):-2.86102294922e-05723
	Remaining time        ::Lock Acquired
	1.17104387283 sec

	void smu::Driver::VM_read(uint16_t*, float*, float*):1096:Trying to Acquire Lock
	void smu::Driver::thread():751:Inside thread : Keep Alive sentvoid smu::Driver::VM_read(uint16_t*, float*, float*):1098:
	Lock Acquired
	voltage               : -2.5749206543e-05
	Remaining time        : 1.16069293022 sec

	void smu::Driver::VM_read(uint16_t*, float*, float*):1096:Trying to Acquire Lock
	void smu::Driver::VM_read(uint16_t*, float*, float*):1098:Lock Acquired
	voltage               : -2.67028808594e-05
	Remaining time        : 1.17099499702 sec

	void smu::Driver::VM_read(uint16_t*, float*, float*):1096:Trying to Acquire Lock
	void smu::Driver::VM_read(uint16_t*, float*, float*):1098:Lock Acquired
	voltage               : -2.67028808594e-05
	Remaining time        : 1.15505194664 sec

	void smu::Driver::VM_read(uint16_t*, float*, float*):1096:Trying to Acquire Lock
	void smu::Driver::VM_read(uint16_t*, float*, float*):1098:Lock Acquired
	void smu::Driver::keepAlive(uint32_t*, float*):721:Trying to Acquire Lock
	voltage               : -2.47955322266e-05
	Remaining time        : 1.1710100174 secvoid smu::Driver::keepAlive(uint32_t*, float*)

	:723:Lock Acquired
	void smu::Driver::VM_read(uint16_t*, float*, float*):1096:Trying to Acquire Lock
	void smu::Driver::thread():751:Inside thread : Keep Alive sent
	void smu::Driver::VM_read(uint16_t*, float*, float*):1098:Lock Acquired
	voltage               : -2.5749206543e-05
	Remaining time        : 1.16075015068 sec

	void smu::Driver::VM_read(uint16_t*, float*, float*):1096:Trying to Acquire Lock
	void smu::Driver::VM_read(uint16_t*, float*, float*):1098:Lock Acquired
	voltage               : -2.5749206543e-05
	Remaining time        : 1.1713449955 sec

	void smu::Driver::VM_read(uint16_t*, float*, float*):1096:Trying to Acquire Lock
	void smu::Driver::VM_read(uint16_t*, float*, float*):1098:Lock Acquired
	voltage               : -2.5749206543e-05
	Remaining time        : 1.15498399734 sec

	void smu::Driver::VM_read(uint16_t*, float*, float*):1096:Trying to Acquire Lock
	void smu::Driver::VM_read(uint16_t*, float*, float*):1098:Lock Acquired
	^Cvoid smu::Driver::keepAlive(uint32_t*, float*):721:Trying to Acquire Lock
	void smu::Driver::keepAlive(uint32_t*, float*):723:Lock Acquired
	Traceback (most recent call last):
	File "VM_getReading.py", line 40, in <module>
		voltage, timeout = libxsmu.VM_getReading (deviceID, filter_length, timeout)
	KeyboardInterrupt
	```

---
- Testing for REC_SIZE
- ``python wrapper/test/python/recSize.py`
- Result : Passed
- Output :

	```
	libxsmu version: 2.1.2
	Total device: 1
	Seial number: XSMU012A
	void smu::Driver::open(const char*, float*):632:Opening Device
	libxsmu version: 2.1.2
	Hardware version: 4.0.0
	Firmware version: 255.255.253
	void smu::Driver::open(const char*, float*):658:Async thread launched
	Device ID     : 0
	goodID        : 1
	Remaining time: 0.873402833939 sec

	void smu::Driver::keepAlive(uint32_t*, float*):755:Trying to Acquire Lock
	void smu::Driver::keepAlive(uint32_t*, float*):757:Lock Acquired
	void smu::Driver::recSize(uint16_t*, float*):1424:Lock Acquired
	recSize:  0
	Timeout:  4.91455888748
	void smu::Driver::close():664:Closing Device
	```
---

- Testing for START_REC and STOP_REC
- `python wrapper/test/python/startRec.py`
- Result : Passed
- Output :
	```
	libxsmu version: 2.1.2
	Total device: 1
	Seial number: XSMU012A
	void smu::Driver::open(const char*, float*):632:Opening Device
	libxsmu version: 2.1.2
	Hardware version: 4.0.0
	Firmware version: 255.255.253
	void smu::Driver::open(const char*, float*):658:Async thread launched
	void smu::Driver::keepAlive(uint32_t*, float*)Device ID     : :0
	goodID        : 1
	Remaining time: 7550.869968891144:Trying to Acquire Lock sec


	void smu::Driver::keepAlive(uint32_t*, float*):757:Lock Acquired
	void smu::Driver::startRec():1472:REC : 1
	Started Recording Streamed Data
	void smu::Driver::stopRec():1478:REC : 0
	Stopped Recording Streamed Data
	void smu::Driver::close():664:Closing Device
	```
---

- Testing for REC_DATA
- `python wrapper/test`
- Result : Failed
- Output :

	```
	libxsmu version: 2.1.2
	Total device: 1
	Seial number: XSMU012A
	void smu::Driver::open(const char*, float*):632:Opening Device
	libxsmu version: 2.1.2
	Hardware version: 4.0.0
	Firmware version: 255.255.253
	void smu::Driver::open(const char*, float*):658:Async thread launched
	Device ID     : 0
	goodID        : 1void smu::Driver::keepAlive(uint32_t*, float*):755:Trying to Acquire Lock
	void smu::Driver::keepAlive(uint32_t*, float*)
	Remaining time: :757:0.866751909256Lock Acquired
	sec

	void smu::Driver::recData(uint16_t*, float*):1454:Lock Acquired
	recSize:  0
	Timeout:  0.0
	Communication timeout in recData
	```