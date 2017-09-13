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

##### Result :

