import struct


def readline_and_decode(ser, num_zones):
    """
    Given a serial port object denoting a port connected to an MCU with a VL3L8CH connected,
    read a line of data from the port and decode it

    Args:
        ser (serial.Serial): Serial port object

    Returns:
        int: Zone index
        list: List of floats representing the data from the zone
        int: Length of the binary data read
        int: Reading counter since last configuration swap (0 or 1)
        int: Current startBin value (0 or 64)
    """

    eol = b"\xff\xff\xff\xff"
    distances = None
    reading_counter = None
    start_bin = None

    c = ser.read_until(expected=eol)
    byte_listing = bytearray(c)
    length = len(byte_listing) - len(byte_listing) % 4
    line_as_floats = [
        struct.unpack("<f", bytes(byte_listing[i : i + 4]))[0] for i in range(0, length, 4)
    ]

    # extract data, see MCU code for ordering, line_as_floats[-1] is always thrown away
    try:
        zone_index = int(line_as_floats[0])  # zone index
        hist = line_as_floats[1:-1]  # histogram data

    except Exception as e:
        print(f"Error parsing data: {e}")
        raise ValueError(f"Failed to decode serial data: {e}")

    # grab ending metadata
    if zone_index == num_zones - 1:
        c = ser.read_until(expected=eol)
        byte_listing = bytearray(c)
        length2 = len(byte_listing) - len(byte_listing) % 4
        line_as_floats = [
            struct.unpack("<f", bytes(byte_listing[i : i + 4]))[0] for i in range(0, length2, 4)
        ]
        try:
            distances = line_as_floats[:-3]  # proprietary distances
            reading_counter = int(line_as_floats[-3])  # readings_since_swap
            start_bin = int(line_as_floats[-2])  # startBin value
        except Exception as e:
            print(f"Error reading metadata: {e}")
            raise ValueError(f"Failed to decode metadata from serial: {e}")

    return zone_index, hist, length, distances, reading_counter, start_bin
