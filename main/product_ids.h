/*
 * Product IDs 
 *
 * This header file contains definitions of varuious product ID numbers used in the deivice
 */

/* ESTA manufacturer ID, used for RDM addressing and also reported over Art-Net
 * 
 * This is set to a Manufacturer ID in the prototypeing and experimental use range
 * 
 * Manufacturer IDs 7FF0h through 7FFFh are reserved for prototyping and experimental use
 * If you are building this device for personal use then you can use these IDs. They are not permitted to be used in product that is shipping.
 * 
 * If you are building the device to release into the wild e.g through sale or hire then you will need to apply for a manufacturer ID for yourself.
 * See https://tsp.esta.org/tsp/working_groups/CP/mfctrIDs.php for more information on how to register one.
 * You should not commit your manufacturer ID back into the project as this should only be used on hardware you produce.
 * 
 * If you do not need Art-Net and RDM functionality and just need the HTTP interface the other option is to remove these components
 */
#define ESTA_ID 0x7FF0

/* ARTNET OEM code, used Art-Net device identification
 * 
 * The issued ID has MNEMONIC: Oem_tyler_ward_poe_stack_light_1
 * 
 * See https://art-net.org.uk/join-the-club/oem-code-listing/ for more information
 * 
 * If building your own deices you may also wish to replace this with your own code
 */
#define ARTNET_OEM_CODE 0x2B72