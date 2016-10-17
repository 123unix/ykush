/****************************************************************************
 FileName:      commandParser.cpp
 Dependencies:  See INCLUDES section
 Compiler:      Visual Studio Community 2015
 Company:       Yepkit, Lda.

 Software License Agreement:

 Copyright (c) 2015 Yepkit Lda

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.

*****************************************************************************
 File Description:

    Change History:
        Rev     Date            Description
        ----    -----------     -----------------------------------------
        1.0     2015-09-11      First Release


 ****************************************************************************
 *  Summary:
 *      Main program function
 *
 *
 *
*****************************************************************************/


// INCLUDES ---------------------------------------------------------------

#include <cstdlib>
#include <iostream>
#include <stdio.h>

#include "stdafx.h"
#include "commandParser.h"
#include "usbcom.h"

using namespace std;



enum cmdAction {
    PORT_UP,
    PORT_DOWN,
    LIST_DEVICES,
    DISPLAY_SERIAL_NUMBER,
    GET_PORT_STATUS,
    PRINT_HELP,
};

bool bySerial = false;



int commandParser(int argc, char** argv) {

  	char choice;
  	char cmd = 0x00;
	enum cmdAction action = PRINT_HELP;
	char *iSerial=NULL;
    char response;
    char port = 0;

  	if ( argc <= 1){
		printUsage(argv[0]);
		return 0;
    	}

	
  	//Parse input options and define action
	switch (argc) {
		case 2:
			if ((argv[1][0]=='-') && (argv[1][1]=='l')) {
				action = LIST_DEVICES;
			} else {
				action = PRINT_HELP;
			}
			break;
		case 3:
			// Single Option
			if ((argv[1][0] == '-') && (argv[1][1]=='d')) {
				action = PORT_DOWN;
				port = argv[2][0];
			} else if ((argv[1][0] == '-') && (argv[1][1]=='u')) {
				action = PORT_UP;
				port = argv[2][0];
			} else if ((argv[1][0] == '-') && (argv[1][1]=='g')) {
				action = GET_PORT_STATUS;
				port = argv[2][0];
			} else {
				action = PRINT_HELP;
			} 	
			break;

		case 5:
			// Two Options
			if ((argv[1][0] == '-') && (argv[1][1]=='s')) {
				bySerial = true;
				iSerial = argv[2];	
			}
			if ((argv[3][0] == '-') && (argv[3][1]=='d')) {
				action = PORT_DOWN;
				port = argv[4][0];
			} else if ((argv[3][0] == '-') && (argv[3][1]=='u')) {
				action = PORT_UP;
				port = argv[4][0];
			} else if ((argv[3][0] == '-') && (argv[3][1]=='g')) {
				action = GET_PORT_STATUS;
				port = argv[4][0];
			} else {
				action = PRINT_HELP;
			}
			break;

		default:
			printUsage(argv[0]);
			break;
	}



	//Get options values and execute action
	
	if ( action == PORT_DOWN || action == PORT_UP ) {
		switch(port) {
			case '1':
				// Downstream 1 down
				cmd = 0x01;
				break;

			case '2':
				// Downstream 2 down
				cmd = 0x02;
				break;

			case '3':
				// Downstream 3 down
				cmd = 0x03;
				break;

			case 'a':
				// All downstreams down
				cmd = 0x0a;
				break;

			default:
				printUsage(argv[0]);
				return -1;
				break;
		}

		// PORT_UP has 0x11 - 0x1a, while PORT_DOWN has 0x01 - 0x0a
		if ( action == PORT_UP )
			cmd += 0x10;

		if (bySerial)
			commandBySerial(iSerial, cmd);
		else
			command(cmd);
	}

	if (action == GET_PORT_STATUS && port == 'a') {
		char cmds[3] = { 0x21, 0x22, 0x23 };
		char resps[3];
		int i;

		if (bySerial)
			commandsBySerial(iSerial, cmds, resps, 3);
		else
			commands(cmds, resps, 3);

		for (i = 0; i < 3; i++) {
			response = resps[i] + 0x20 - cmds[i];
			if (response == 0x10 + cmd) {
				printf("Downstream port %d is: UP\n", i);
			} else if (response == cmd) {
				printf("Downstream port %d is: DOWN\n", i);
			} else {
				printf("Downstream port %d is: UNKNOWN\n", i);
			}
		}
	} else if (action == GET_PORT_STATUS) {
		switch (port) {
			case '1':
				//downstream 1 status
				cmd = 0x1;
				break;
			case '2':
				//downstream 2 status
				cmd = 0x2;
				break;
			case '3':
				//downstream 3 status
				cmd = 0x3;
				break;
			default:
				printUsage(argv[0]);
				return -1;
				break;
		}

		if (bySerial) //target board specified by serial number
			response = commandBySerial(iSerial,cmd + 0x20);
		else
			response = command(cmd + 0x20);

		if (response == 0x10 + cmd) {
			printf("\nDownstream port %c is: UP\n", port);
		} else if (response == cmd) {
			printf("\nDownstream port %c is: DOWN\n", port);
		} else {
			printf("\nUnable to get port status for port %c\n", port);
		}
	}


	
	if ( action == LIST_DEVICES ) {
		printf("\nAttached YKUSH Boards\n");
        	printf("\n---------------------\n");
        	listDevices();
	}


	if ( action == PRINT_HELP ) {
		printUsage(argv[0]);
	}


    return 0;
}



int printUsage(char* execName){

    printf("\n-------------------");
    printf("\n\tUsage:\n");
    printf("-------------------\n");
    printf("\n%s -d downstream_number \t\tTurns DOWN the downstream port with the number downstream_number\n", execName);
    printf("\n%s -u downstream_number \t\tTurns UP the downstream port with the number downstream_number\n", execName);
    printf("\n%s -g downstream_number \t\tObtains the switching status of port with the number downstream_number\n", execName);
    printf("\n%s -l \t\t\t\tLists all currently attached YKUSH boards\n", execName);
    printf("\n%s -s serial_number -d downstream_number \tTurns DOWN the downstream port with the number downstream_number for the board with the specified serial number\n", execName);
    printf("\n%s -s serial_number -u downstream_number \tTurns UP the downstream port with the number downstream_number for the board with the specified serial number\n\n\n", execName);
    printf("\n%s -s serial_number -g downstream_number \tObtains the switching status of port with the number downstream_number for the board with the specified serial number\n\n\n", execName);

    return 0;
}
