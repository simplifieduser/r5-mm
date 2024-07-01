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


def generate_invalid(filename, request_count):
    command_options = ['r', 'R', 'w', 'W', random_value(False)]
    with open(filename, 'w', newline='') as file:
        writer = csv.writer(file, lineterminator="\n")
        for _ in range(request_count):
            mode = random.choice(command_options)
            if mode in ['r', 'R']:
                addr = random_value(random.choice([True, False]))
                writer.writerow([mode, addr])
            elif mode in ['w', 'W']:
                addr = random_value(random.choice([True, False]))
                data = random_value(random.choice([True, False]))
                writer.writerow([mode, addr, data])
            else:
                addr = random_value(random.choice([True, False]))
                data = random_value(random.choice([True, False]))
                writer.writerow([mode, addr, data])


@pytest.mark.parametrize("request_count", [random.randint(1, 100) for _ in range(50)])
def test_valid(request_count):

    requests = generate_valid("./test.csv", request_count)
    process = subprocess.run(["./r5mm", "./test.csv"], stdout=subprocess.PIPE, text=True)
    assert process.returncode == 0
    lines = process.stdout.split('\n')
    for i, req in enumerate(requests):
        assert lines[i] == req


@pytest.mark.parametrize("request_count", [random.randint(1, 100) for _ in range(50)])
def test_invalid(request_count):
    generate_invalid("./test.csv", request_count)
    process = subprocess.run(["./r5mm", "./test.csv"], stdout=subprocess.PIPE, stderr=subprocess.DEVNULL, text=True)
    assert process.returncode != 0
