/*
* Copyright (c) 2016-2016 ARM Limited. All rights reserved.
* SPDX-License-Identifier: Apache-2.0
* Licensed under the Apache License, Version 2.0 (the License); you may
* not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an AS IS BASIS, WITHOUT
* WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#include "mbed.h"
#include "rtos.h"
#include "NanostackInterface.h"
#include "greentea-client/test_env.h"

#include "NanostackRfPhyNcs36510.h"
NanostackRfPhyNcs36510 rf_phy;
LoWPANNDInterface mesh;


void http_demo(NetworkInterface *net) {
    TCPSocket socket;

    // Show the network address
    const char *ip = net->get_ip_address();
    printf("IP address is: %s\n", ip ? ip : "No IP");

    // Open a socket on the network interface, and create a TCP connection to www.ipv6forum.com
    socket.open(net);
    int ret = socket.connect("2001:a18:1:20::42", 80);
    if (ret != NSAPI_ERROR_OK) {
        error("Socket connect failed: %i\r\n", ret);
    }

    // Send a simple http request
    char sbuffer[] = "GET / HTTP/1.1\r\nHost: developer.mbed.org\r\n\r\n";
    int scount = socket.send(sbuffer, sizeof sbuffer);
    printf("sent %d [%.*s]\r\n", scount, strstr(sbuffer, "\r\n")-sbuffer, sbuffer);

    // Recieve a simple http response and print out the response line
    char rbuffer[64];
    int rcount = socket.recv(rbuffer, sizeof rbuffer);
    printf("recv %d [%.*s]\r\n", rcount, strstr(rbuffer, "\r\n")-rbuffer, rbuffer);

    // Close the socket to return its memory and bring down the network interface
    socket.close();
}


int main()
{
    GREENTEA_SETUP(60, "default_auto");
    printf("\r\n\r\nConnecting...\r\n");
    mesh.initialize(&rf_phy);

    if (mesh.connect()) {
        printf("Connection failed!\r\n");
        GREENTEA_TESTSUITE_RESULT(false);
        return -1;
    }

    while (NULL == mesh.get_ip_address())
        Thread::wait(500);

    const char *ip = mesh.get_ip_address();
    if (NULL == ip) {
        printf("Connect timed out\r\n");
        GREENTEA_TESTSUITE_RESULT(false);
    }
    printf("connected. IP = %s\r\n", ip);

    http_demo(&mesh);

    GREENTEA_TESTSUITE_RESULT(true);
}
