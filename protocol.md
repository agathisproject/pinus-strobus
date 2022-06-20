# Introduction

This communication protocol is based on SMBus Block Read/Write protocol (without Packet Error Code) - as defined in **System Management Bus Specification Version 3.1** section 6.5.7

## SMBus Block Commands

write: **S** *addr+W* *cmd* byte_cnt byte_0 ... byte_N-1 **P**

read: **S** *addr+W* *cmd* **S** *addr+R* byte_cnt byte_0 ... byte_N-1 **P**

where:

  * **S** - I2C start condition
  * **P** - I2C stop condition
  * *addr+W* - I2C 7bit slave addressed for write (bit0 = 0)
  * *addr+R* - I2C 7bit slave addressed for read (bit0 = 1)
  * *cmd* - SMBus command

# Module Commands

## Summary (0x01)

write: **S** *addr+W* *0x01* 0x04 byte_0 byte_1 byte_2 byte_3 **P**

read: **S** *addr+W* *0x01* **S** *addr+R* 0x04 byte_0 byte_1 byte_2 byte_3 **P**

### Byte 0 - Protocol Version

The only allowed value is 0x01.

### Byte 1 - HW Capabilities (LSB)

| bit | power on | description             |
|----:|---------:|-------------------------|
|  7  |   - (RO) | reserved                |
|  6  |   - (RO) | module has PCIe         |
|  5  |   - (RO) | module has USB          |
|  4  |   - (RO) | module has JTAG         |
|  3  |   - (RO) | module has 1PPS    	 |
|  2  |   - (RO) | module has CLK100    	 |
|  1  |   - (RO) | module has power supply |
|  0  |   - (RO) | module is TMC           |

Bit values:

  * 0x0 interface is **NOT** implemented on module
  * 0x1 interface is implemented on module

The values at power-on are not defined since they are module dependent.

### Byte 2 - HW Capabilities (MSB)

| bit | power on | description        |
|----:|---------:|--------------------|
|  7  |   - (RO) | reserved           |
|  6  |   - (RO) | reserved           |
|  5  |   - (RO) | reserved           |
|  4  |   - (RO) | reserved           |
|  3  |   - (RO) | reserved           |
|  2  |   - (RO) | reserved           |
|  1  |   - (RO) | reserved           |
|  0  |   - (RO) | reserved           |

Bit values:

  * 0x0 interface is **NOT** implemented on module
  * 0x1 interface is implemented on module

The values at power-on are not defined since they are module dependent.

### Byte 3 - Error Code

| bit | power-on | description            |
|----:|---------:|------------------------|
|  7  |   - (RO) | operation in progress  |
| 6-0 |   - (RO) | error code             |

0 means NO ERROR. The values at power-on are not defined since the module can have errors on boot.

If multiple error codes are present only the most important one will be shown. After clearing the cause of the current error code, the next one will be shown.

## Capabilities / Basic Info (0x02)

write: NOT SUPPORTED - this is a hardcoded value

read: **S** *addr+W* *0x02* **S** *addr+R* 0x04 byte_0 byte_1 byte_2 byte_3 **P**

### Byte 2 - Module Type (LSB)

### Byte 3 - Module Type (MSB)

Type can have the following values:

  * 0x0000 - reserved, only for development
  * 0xFFFF - reserved, only for development

## Manufacturer Name (0xF0)

write: NOT SUPPORTED - this is a hardcoded value

read: **S** *addr+W* *0xF0* **S** *addr+R* 0x10 byte_0 ... byte_15 **P**

Manufacturer name in 16 bytes, ASCII encoded.

## Manufacturer PN (0xF1)

write: NOT SUPPORTED - this is a hardcoded value

read: **S** *addr+W* *0xF1* **S** *addr+R* 0x10 byte_0 ... byte_15 **P**

Manufacturer part number in 16 bytes, ASCII encoded.

## Manufacturer SN (0xF2)

write: NOT SUPPORTED - this is a hardcoded value

read: **S** *addr+W* *0xF2* **S** *addr+R* 0x10 byte_0 ... byte_15 **P**

Manufacturer serial number in 16 bytes, ASCII encoded.

## Manufacturer Specific (0xFE)

write: **S** *addr+W* *0xFE* byte_cnt byte_0 ... byte_N-1 **P**

read: **S** *addr+W* *0xFE* **S** *addr+R* byte_cnt byte_0 ... byte_N-1 **P**

The byte sequence is specific to each module and left for the manufacturer to implement.
