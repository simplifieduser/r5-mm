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
                requests.append(f"{i}: 0 {int(addr, 0)} 0")
            else:
                addr = random_value()
                data = random_value()
                writer.writerow([mode, addr, data])
                requests.append(f"{i}: 1 {int(addr, 0)} {int(data, 0)}")

    return requests


def test_valid():

    for _ in range(50):

        requests = generate_valid("./test.csv", random.randrange(0, 100))
        process = subprocess.run(["./r5mm", "./test.csv"], stdout=subprocess.PIPE, text=True)
        assert process.returncode == 0
        lines = process.stdout.split('\n')
        for i, req in enumerate(requests):
            assert req == lines[i]


def generate_invalid(filename, reqeust_count):

    requests = []
    command_options = ['r', 'R', 'w', 'W']
    invalid_argument_options = ['mode', 'add', 'data']

    with open(filename, 'w', newline='') as file:

        writer = csv.writer(file, lineterminator="\n")

        for i in range(reqeust_count - 1):
            mode = random.choice(command_options)
            if mode is 'r' or mode is 'R':
                addr = random_value()
                writer.writerow([mode, addr])
                requests.append(f"{i}: 0 {int(addr, 0)} 0")
            else:
                addr = random_value()
                data = random_value()
                writer.writerow([mode, addr, data])
                requests.append(f"{i}: 1 {int(addr, 0)} {int(data, 0)}")

        invalid_argument = random.choice(invalid_argument_options)

        if invalid_argument is 'mode':
            random_mode = random_value(False)
            for i in range(reqeust_count):
                mode = random.choice(command_options)
                if mode is 'r' or mode is 'R':
                    addr = random_value()
                    writer.writerow([random_mode, addr])
                    requests.append("mode")
                else:
                    addr = random_value()
                    data = random_value()
                    writer.writerow([random_mode, addr, data])
                    requests.append("mode")

        elif invalid_argument is 'add':
            random_add = random_value(False)
            for i in range(reqeust_count):
                mode = random.choice(command_options)
                if mode is 'r' or mode is 'R':
                    writer.writerow([mode, random_add])
                    requests.append("mode")
                else:
                    data = random_value()
                    writer.writerow([mode, random_add, data])
                    requests.append("mode")

        elif invalid_argument is 'data':
            random_data = random_value(False)
            for i in range(reqeust_count):
                mode = random.choice(command_options)
                if mode is 'r' or mode is 'R':
                    addr = random_value()
                    writer.writerow([mode, addr, random_data])
                    requests.append("data-read")
                else:
                    addr = random_value()
                    writer.writerow([mode, addr, random_data])
                    requests.append("data")

    return requests

def test_invalid():

    for _ in range(50):
        requests = generate_valid("./test.csv", random.randrange(0, 100))
        requests = generate_invalid("./test.csv", 50)
        process = subprocess.run(["./r5mm", "./test.csv"], stdout=subprocess.PIPE, text=True)
        assert process.returncode == 1


