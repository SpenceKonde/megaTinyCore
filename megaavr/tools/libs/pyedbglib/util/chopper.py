"""Data chopper for preparing data for transport over USB"""

class DataChopper(object):
    """This class is implemented to prepare data for transfers over USB"""

    def __init__(self, size, padding=None):
        # Make sure the size is valid
        if not isinstance(size, int):
            raise TypeError("Chopper size '{}' is not of type int".format(size))

        self.package_size = size

        # Make sure the padding is valid
        if not isinstance(padding, int) and padding is not None:
            raise TypeError("Padding '{}' is not of type int".format(padding))
        if padding is not None and padding > 255:
            raise TypeError("Padding '{}' is larger than 255, only 8-bit values are supported here!".format(padding))

        self.padding = padding

    @staticmethod
    def fix_data_type(data):
        """
        Checks if the incoming data is OK to send

        :param data: Input data
        :return: list of valid data to send
        """

        # Make a list that is returned
        array = []

        # If the data is an int:
        if isinstance(data, int):
            if data > 255:
                raise ValueError("Value {} is larger than 255, only 8-bit values are supported here!".format(data))
            array.append(data)
        else:
            # loop through each element and convert them accordingly
            for item in data:
                if isinstance(item, int):
                    if item > 255:
                        raise ValueError("Value {} is larger than 255, only 8-bit values are supported here!"
                                         .format(item))
                    array.append(item)
                elif isinstance(item, str):
                    array.append(ord(item))
                else:
                    try:
                        # Try clause for catching unicode in PY2
                        if isinstance(item, unicode):
                            array.append(ord(item))
                        else:
                            raise TypeError("Unsupported data sent to fix_data_type(): {}".format(type(item)))
                    except NameError:
                        raise TypeError("Unsupported data sent to fix_data_type(): {}".format(type(item)))

        return array

    def chopper(self, data):
        """
        This function chops a large list of data into smaller chunks based on packageSize

        If padding is specified any list is padded to packageSize

        :param data:
        :return: list of chopped lists
        """

        # Verify that the data is a valid list / Convert data if not
        data = self.fix_data_type(data)
        data_len = len(data)

        # calculate how many chunks to chop the data into, division will floor with "//" operator
        num_chunks = data_len // self.package_size
        if data_len % self.package_size:
            num_chunks += 1

        # Offset to keep track of how much data we have chopped
        offset = 0

        # Create a list to store data lists
        chopped = []

        for i in range(0, num_chunks):
            # How much data is left to write?
            remaining = data_len - (i * self.package_size)

            if remaining > self.package_size:
                stop = self.package_size
            else:
                stop = remaining

            # Create a chunk of data
            chunk = data[offset:offset + stop]

            # Does the chunk need padding?
            if self.padding is not None:
                # Only pad the last package
                if i == num_chunks - 1:
                    chunk.extend([self.padding] * (self.package_size - len(chunk)))

            # Calculate the new offset
            offset += len(chunk)

            # Append the data to the list
            chopped.append(chunk)

        return chopped
