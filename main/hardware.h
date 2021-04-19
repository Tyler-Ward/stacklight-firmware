// Hardware pin definitions for v1.0 PCB

// Status LEDS

#define GPIO_LED_LOCATE 4   //!< GPIO pin for the locate indicator
#define GPIO_LED_NETWORK 5  //!< GPIO pin for the network indicator
#define GPIO_LED_STATUS 15  //!< GPIO pin for the status indicator
#define GPIO_LED_ARTNET 16  //!< GPIO pin for the artnet indicator

#define GPIO_LED_ENABLE 2   //!< GPIO pin for the LED enable mosfet

//output channels

#define GPIO_OUTPUT_RED 33     //!< GPIO pin for the red output channel
#define GPIO_OUTPUT_YELLOW 13  //!< GPIO pin for the yellow output channel
#define GPIO_OUTPUT_GREEN 32   //!< GPIO pin for the green output channel
#define GPIO_OUTPUT_BUZZER 14  //!< GPIO pin for the buzzer output channel

// network pins

#define	GPIO_PHY_POWER 12  //!< GPIO pin for Ethernet PHY power enable
#define	GPIO_ETH_MDC 23    //!< GPIO pin for the Ethernet PHY MDC pin
#define	GPIO_ETH_MDIO 18   //!< GPIO pin for the Ethernet PHY MDIO pin
#define	GPIO_ETH_RESET -1 //!< GPIO pin for the Ethernet PHY reset pin (not present)

#define ETH_PHY_ADDR 0 //!< Address of the ethernet PHY