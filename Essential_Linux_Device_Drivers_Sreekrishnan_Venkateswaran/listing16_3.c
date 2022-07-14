#include <net/iw_handler.h>

#include <linux/wireless.h>


/* Populate the iw_handler_def structure with the location and number
   of standard and private handlers, argument details of private
   handlers, and location of get_wireless_stats() */
static struct iw_handler_def mywifi_handler_def = {
    .standard = mywifi_std_handlers,
    .num_standard = sizeof(mywifi_std_handlers) /
    sizeof(iw_handler),
    .private = (iw_handler *) mywifi_pvt_handlers,
    .num_private = sizeof(mywifi_pvt_handlers) /
    sizeof(iw_handler),
    .private_args = (struct iw_priv_args *)mywifi_pvt_args,
    .num_private_args = sizeof(mywifi_pvt_args) /
    sizeof(struct iw_priv_args),
    .get_wireless_stats = mywifi_stats,
};

/* Handlers corresponding to iwconfig */
static iw_handler mywifi_std_handlers[] = {
    NULL,            /* SIOCSIWCOMMIT */
    mywifi_get_name, /* SIOCGIWNAME */
    NULL,            /* SIOCSIWNWID */
    NULL,            /* SIOCGIWNWID */
    mywifi_set_freq, /* SIOCSIWFREQ */
    mywifi_get_freq, /* SIOCGIWFREQ */
    mywifi_set_mode, /* SIOCSIWMODE */
    mywifi_get_mode, /* SIOCGIWMODE */
    /* ... */
};

#define MYWIFI_MYPARAMETER SIOCIWFIRSTPRIV

/* Handlers corresponding to iwpriv */
static iw_handler mywifi_pvt_handlers[] = {
    mywifi_set_myparameter,
    /* ... */
};

/* Argument description of private handlers */
static const struct iw_priv_args mywifi_pvt_args[] = {
    { MYWIFI_MYPARAMATER,
      IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "myparam"},
}

struct iw_statistics mywifi_stats; /* WLAN Statistics */
/* Method to set operational frequency supplied via mywifi_std_handlers.
   Similarly implement the rest of the methods */
mywifi_set_freq()
{
    /* Set frequency as specified in the data sheet */
    /* ... */
}

/* Called when you read /proc/net/wireless */
static struct iw_statistics *
mywifi_stats(struct net_device *dev)
{
    /* Fill the fields in mywifi_stats */
    /* ... */

    return(&mywifi_stats);
}

/*Device initialization. For PCI-based cards, this is called from the
  probe() method. Revisit init_mycard() in Listing 15.1 in Chapter 15
  for a full discussion */
static int
init_mywifi_card()
{
    struct net_device *netdev;

    /* Allocate WiFi network device. Internally calls
       alloc_etherdev() */
    netdev = alloc_ieee80211(sizeof(struct mywifi_priv));

    /* ... */

    /* Register Wireless Extensions support */
    netdev->wireless_handlers = &mywifi_handler_def;

    /* ... */

    register_netdev(netdev);
}

