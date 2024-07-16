import pytest
import subprocess
import random
import csv


def random_value():
    value = random.randint(0, 0xFFFFFFFF)
    if random.choice([True, False]):
        return hex(value)
    else:
        return str(value)


def generate_valid_requests():

    request_count = random.randint(0, 100)
    requests = []

    for _ in range(request_count):
        command_options = ['r', 'R', 'w', 'W']
        mode = random.choice(command_options)
        if mode == 'r' or mode == 'R':
            addr = random_value()
            requests.append([mode, addr])
        else:
            addr = random_value()
            data = random_value()
            requests.append([mode, addr, data])

    return requests


def generate_valid_file(filename, requests):

    with open(filename, 'w', newline='') as file:

        writer = csv.writer(file, lineterminator="\n")
        for req in requests:
            writer.writerow(req)


def generate_valid_command():

    cycles = random.randint(0, 1000)
    blocksize = random.randint(1, 1000)
    offset = random.randint(0, 1000)
    cache_size = random.randint(0, 1000)
    cache_latency = random.randint(0, 1000)
    memory_latency = random.randint(0, 1000)

    return ["./dist/r5mm", "./test.csv", f"-c{cycles}", f"-b{blocksize}", f"-o{offset}", f"-s{cache_size}", f"-t{cache_latency}", f"-m{memory_latency}"]


def generate_edge_case_command():

    cycles = random.randint(0, 1000)
    offset = random.randint(0, 1000)
    cache_latency = random.randint(0, 1000)
    memory_latency = random.randint(0, 1000)

    return ["./dist/r5mm", "./test.csv", f"-c{cycles}", f"-b1", f"-o{offset}", f"-s0", f"-t{cache_latency}", f"-m{memory_latency}"]


@pytest.mark.parametrize("command", [generate_valid_command() for _ in range(100)])
@pytest.mark.parametrize("requests", [generate_valid_requests() for _ in range(10)])
def test_fuzzy(requests, command):

    generate_valid_file("./test.csv", requests)
    process = subprocess.run(command, stdout=subprocess.PIPE, text=True)
    assert process.returncode == 0


@pytest.mark.parametrize("command", [generate_edge_case_command() for _ in range(100)])
@pytest.mark.parametrize("requests", [generate_valid_requests() for _ in range(19)])
def test_edge_cases(requests, command):

    generate_valid_file("./test.csv", requests)
    process = subprocess.run(command, stdout=subprocess.PIPE, text=True)
    assert process.returncode == 0
