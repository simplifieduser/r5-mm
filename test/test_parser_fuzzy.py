import subprocess
import pytest
import string
import random
import csv


# Generiert (in-) valide Werte für Request
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


# Generiert valide Request Daten
def generate_valid_requests():

    request_count = random.randint(0, 1000)
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


# Generiert invalide Request Daten
def generate_invalid_requests():

    request_count = random.randint(0, 1000)
    requests = []

    # Append valid requests first
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

    # Then append one last invalid request
    command_options = ['r', 'R', 'w', 'W', random_value(False), random_value(False)]
    mode = random.choice(command_options)
    if mode in ['r', 'R']:
        addr = random_value(False)
        requests.append([mode, addr])
    elif mode in ['w', 'W']:
        choice = random.choice([True, False])
        addr = random_value(choice)
        data = random_value(not choice)
        requests.append([mode, addr, data])
    else:
        addr = random_value(True)
        data = random_value(True)
        requests.append([mode, addr, data])

    return requests


# Erstellt eine csv Input-Datei aus Request Daten
def generate_file(filename, requests):

    with open(filename, 'w', newline='') as file:

        arni = ["\r\n", "\n", "\r"]
        writer = csv.writer(file, lineterminator=random.choice(arni))
        for req in requests:
            writer.writerow(req)


# Konvertiert Request Daten zu Konsolenausgabe
def convert_to_result(requests):

    result = []

    for i, req in enumerate(requests):
        if req[0] == "r" or req[0] == "R":
            result.append(f"{i}: 0 {int(req[1], 0)} 0")
        elif req[0] == "w" or req[0] == "W":
            result.append(f"{i}: 1 {int(req[1], 0)} {int(req[2], 0)}")
        else:
            raise Exception()

    return result


@pytest.mark.parametrize("requests", [generate_valid_requests() for _ in range(1000)])
def test_valid(requests):

    random.seed(0)
    generate_file("./temp_test.csv", requests)
    results = convert_to_result(requests)

    process = subprocess.run(["./dist/r5mm", "./temp_test.csv"], stdout=subprocess.PIPE, text=True)
    assert process.returncode == 0

    lines = process.stdout.split('#')[2]
    lines = lines.split("\n")
    lines.pop(0)
    lines.pop(0)

    for i, req in enumerate(results):
        assert lines[i] == req


@pytest.mark.parametrize("requests", [generate_invalid_requests() for _ in range(1000)])
def test_invalid(requests):

    random.seed(0)
    generate_file("./temp_test.csv", requests)

    process = subprocess.run(["./dist/r5mm", "./temp_test.csv"], stdout=subprocess.PIPE, text=True)
    assert process.returncode == 1
