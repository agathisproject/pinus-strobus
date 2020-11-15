# Introduction

This communication protocol is based on SMBus Block Read/Write protocol (without Packet Error Code) - as defined in **System Management Bus Specification Version 3.0 ** section 6.5.7

## SMBus Block Commands

write: **S** addr+W cmd byte_cnt byte_0 ... byte_N-1 **P**

read: **S** addr+W cmd **S** addr+R byte_cnt byte_0 ... byte_N-1 **P
**

where:

  * **S** - I2C start condition
  * **P** - I2C stop condition
  * addr+W - I2C 7bit slave addressed for write (bit0 = 0)
  * addr+R - I2C 7bit slave addressed for read (bit0 = 1)

# Node Commands

## Summary (0x01)

write: **S** addr+W *0x01* *0x03* byte_0 byte_1 byte_2 **P**

read: **S** addr+W *0x01* **S** addr+R *0x03* byte_0 byte_1 byte_2 **P**

### Byte 0 - Protocol Version

The only allowed value is 0x01.

### Byte 1 - Power/Reset

| bits | power on | description       |
|------|----------|-------------------|
| 7-4  |    0x0   | reserved          |
| 3-1  |    0x0   | power             |
|   0  |    0x1   | reset             |

Power can have the following values:

  * 0x0 (less than ??? W)
  * 0x1 (between ??? and ??? W)

Reset can have the following values:

  * 0x0 reset OFF
  * 0x1 reset ON

### Byte 2 - Alarms

| bits | power on| description        |
|------|---------|--------------------|
| 7-4  |  undef  | number of warnings |
| 3-0  |  undef  | number of errors   |

## Type (0x02)

write: NOT SUPPORTED - this is a hardcoded value

read: **S** addr+W *0x02* **S** addr+R *0x02* byte_0 byte_1 **P**

Type can have the following values:

  * 0x0000 - reserved, only for development
  * 0xFFFF - reserved, only for development

## Manufacturer Specific (0xFE)

write: **S** addr+W *0xFE* byte_cnt byte_0 ... byte_N-1 **P**

read: **S** addr+W *0xFE* **S** addr+R byte_cnt byte_0 ... byte_N-1 **P**

The byte sequence is specific to each node and left for the manufacturer to implement.
