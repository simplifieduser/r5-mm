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
        characters = string.digits
        value = ''.join(random.choice(characters) for _ in range(10, 30))
        return str(value)


def generate_valid_request(i, writer, requests):
    command_options = ['r', 'R', 'w', 'W']
    mode = random.choice(command_options)
    if mode == 'r' or mode == 'R':
        addr = random_value()
        writer.writerow([mode, addr])
        requests.append(f"{i}: 0 {int(addr, 0)} 0")
    else:
        addr = random_value()
        data = random_value()
        writer.writerow([mode, addr, data])
        requests.append(f"{i}: 1 {int(addr, 0)} {int(data, 0)}")


def generate_valid_file(filename, reqeust_count):

    requests = []

    with open(filename, 'w', newline='') as file:

        writer = csv.writer(file, lineterminator="\n")
        for i in range(reqeust_count):
            generate_valid_request(i, writer, requests)

    return requests


def generate_invalid_request(writer):
    command_options = ['r', 'R', 'w', 'W', random_value(False), random_value(False)]
    mode = random.choice(command_options)
    if mode in ['r', 'R']:
        addr = random_value(False)
        writer.writerow([mode, addr])
    elif mode in ['w', 'W']:
        choice = random.choice([True, False])
        addr = random_value(choice)
        data = random_value(not choice)
        writer.writerow([mode, addr, data])
    else:
        addr = random_value(True)
        data = random_value(True)
        writer.writerow([mode, addr, data])


def generate_invalid_file(filename, request_count):
    with open(filename, 'w', newline='') as file:
        writer = csv.writer(file, lineterminator="\n")
        for i in range(request_count - 1):
            generate_valid_request(i, writer, [])
        generate_invalid_request(writer)


@pytest.mark.parametrize("request_count", [random.randint(1, 100) for _ in range(50)])
def test_valid(request_count):

    requests = generate_valid_file("./temp_test.csv", request_count)
    process = subprocess.run(["./dist/r5mm", "./temp_test.csv"], stdout=subprocess.PIPE, text=True)
    assert process.returncode == 0
    lines = process.stdout.split('-')[1]
    lines = lines.split('\n')
    lines.pop(0)
    for i, req in enumerate(requests):
        assert lines[i] == req


@pytest.mark.parametrize("request_count", [random.randint(1, 100) for _ in range(50)])
def test_invalid(request_count):
    generate_invalid_file("./temp_test.csv", request_count)
    process = subprocess.run(["./dist/r5mm", "./temp_test.csv"], stdout=subprocess.PIPE, stderr=subprocess.DEVNULL, text=True)
    assert process.returncode == 1
