#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>

int interfaceNum(int socket_fd, char *interface)
{
  struct ifreq ifr;
  memset(&ifr, 0, sizeof(ifr));
  for (int i = 0; i < sizeof(interface); i++)
  {
    ifr.ifr_name[i] = *(interface + i);
  }
  return ifr.ifr_ifindex;
}

int main()
{
  int pSocket = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
  if (pSocket == -1)
  {
    fprintf(stderr, "CREATION FAILED: %s\n", strerror(errno));
    return -1;
  }
  char *interface = "wlan0";
  int iNum = interfaceNum(pSocket, interface);
  if (iNum == -1)
  {
    printf("FETCHING INTERFACE NUM FAIL");
    return -1;
  }

  struct sockaddr_ll bindAddress;
  memset(&bindAddress, 0, sizeof(bindAddress));

  bindAddress.sll_family = AF_PACKET;
  bindAddress.sll_protocol = htons(ETH_P_ALL);
  bindAddress.sll_ifindex = iNum;

  int bindRes = bind(pSocket, (struct sockaddr *)&bindAddress, sizeof(bindAddress));
  if (bindRes == -1)
  {
    printf("BINDING FAIL");
    return -1;
  }

  unsigned int capLen = 1500;
  char buffer[capLen];

  while (1)
  {
    read(pSocket, buffer, capLen);
    for (int i = 0; i < capLen; i++)
    {
      printf("%d", buffer[i]);
    }
    printf("\n-----PACKET-----\n");
  }
}