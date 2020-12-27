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

### Byte 1 - Flags

| bit | power-on | description           |
|----:|---------:|-----------------------|
|  7  |   - (RO) | MC is TMC             |
|  6  |   - (RO) | MC has EEPROM         |
| 5-1 |   0 (RO) | TBD                   |
|  0  |   0 (RO) | operation in progress |

Bit 7 - Indicates is the module is a TMC or regular MC. The TMC ends the current management segment.

Bit 6 - The module can have EEPROM for saving the current configuration, but it's not mandatory (for cheaper uCs)

Bit 0 - Indicates if the latest operation requested by the TMC is in progress

### Byte 2 - Error Code

| bit | power-on | description |
|----:|---------:|-------------|
| 7-0 |   - (RO) | error code  |

0 means NO ERROR. The values at power-on are not defined since the module can have errors on boot.

If multiple error codes are present only the most important one will be shown. After clearing the cause of the current error code, the next one will be shown.

### Byte 3 - Commands

| bit | power-on | description        |
|----:|---------:|--------------------|
| 7-1 |   - (RW) | TBD                |
|  0  |   0 (RW) | module reset       |

Bit 0 - Write 1 to reset the module. If the I2C interface is active during the reset additional writes will not cause a reset until the current one is done.

## Type (0x02)

write: NOT SUPPORTED - this is a hardcoded value

read: **S** *addr+W* *0x02* **S** *addr+R* 0x02 byte_0 byte_1 **P**

Type can have the following values:

  * 0x0000 - reserved, only for development
  * 0xFFFF - reserved, only for development

## Capabilities (0x03)

write: **S** *addr+W* *0x03* 0x02 byte_0 byte_1 **P**

read: **S** *addr+W* *0x03* **S** *addr+R* 0x02 byte_0 byte_1 **P**

### Byte 0 - Interfaces

| bit | power on | description        |
|----:|---------:|--------------------|
|  7  |   - (RO) | reserved           |
|  6  |   - (RO) | reserved           |
|  5  |   - (RO) | reserved           |
|  4  |   - (RO) | module has PCIe    |
|  3  |   - (RO) | module has USB     |
|  2  |   - (RO) | module has JTAG    |
|  1  |   - (RO) | module has 1PPS    |
|  0  |   - (RO) | module has CLK100  |

Bit values:

  * 0x0 interface is **NOT** implemented on module
  * 0x1 interface is implemented on module

The values at power-on are not defined since they are module dependent.

### Byte 1 - Enabled Interfaces

| bit | power on | description        |
|----:|---------:|--------------------|
|  7  |   - (RO) | reserved           |
|  6  |   - (RO) | reserved           |
|  5  |   - (RO) | reserved           |
|  4  |   - (RW) | PCIe is enabled    |
|  3  |   - (RW) | USB is enabled     |
|  2  |   - (RW) | JTAG is enabled    |
|  1  |   - (RW) | 1PPS is enabled    |
|  0  |   - (RW) | CLK100 is enabled  |

Bit values:

  * 0x0 interface is **NOT** enabled on module
  * 0x1 interface is enabled on module

The values at power-on are not defined since they can be saved and restored from EEPROM.

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
