#include <sys/socket.h>

#include <bluetooth/rfcomm.h> /* For struct sockaddr_rc */


void
sense_dispenser()
{
    int pillfd;
    struct sockaddr_rc pill_rfcomm;
    char buffer[1024];

    /* ... */

    /* Create a Bluetooth RFCOMM socket */
    if ((pillfd = socket(PF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM))
        < 0) {
        printf("Bad Bluetooth RFCOMM socket");
        exit(1);
    }

    /* Connect to the pill dispenser */
    pill_rfcomm.rc_family  = AF_BLUETOOTH;
    pill_rfcomm.rc_bdaddr  = PILL_DISPENSER_BLUETOOTH_ADDR;
    pill_rfcomm.rc_channel = PILL_DISPENSER_RFCOMM_CHANNEL;
    if (connect(pillfd, (struct sockaddr *)&pill_rfcomm,
                sizeof(pill_rfcomm))) {
        printf("Cannot connect to Pill Dispenser\n");
        exit(1);
    }
    printf("Connection established to Pill Dispenser\n");

    /* Poll until data is ready */
    select(pillfd, &fds, NULL, NULL, &timeout);

    /* Data is available on this RFCOMM channel */
    if (FD_ISSET(pillfd, fds)) {
        /* Read pill removal alerts from the dispenser */
        read(pillfd, buffer, sizeof(buffer));

        /* Take suitable action; e.g., send a message to the health
           care provider's server on the Internet via the GPRS
           interface */
        /* ... */
    }

    /* ... */
}
