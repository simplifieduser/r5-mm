import subprocess
import pytest
import string
import random
import csv


def random_value(valid=True):
    if valid:
        value = random.randint(0, 0xFFFFFFFF)
        if random.choice([True, False]):
            return hex(value)
        else:
            return str(value)

    else:
        characters = string.ascii_letters + string.digits
        value = ''.join(random.choice(characters) for _ in range(0, 30))
        return str(value)


def generate_valid(filename, reqeust_count):

    requests = []
    command_options = ['r', 'R', 'w', 'W']

    with open(filename, 'w', newline='') as file:

        writer = csv.writer(file, lineterminator="\n")

        for i in range(reqeust_count):
            mode = random.choice(command_options)
            if mode is 'r' or mode is 'R':
                addr = random_value()
                writer.writerow([mode, addr])
                requests.append(str.format("%d: %d %d ", i, mode, addr))
            else:
                addr = random_value()
                data = random_value()
                writer.writerow([mode, addr, data])
                requests.append(str.format("%d: %d %d %d", i, mode, addr, data))

    return requests


def test_random():
    requests = generate_valid("./test.csv", random.randrange(0, 20))
    process = subprocess.run(["./r5mm", "./test.csv"])
    assert True