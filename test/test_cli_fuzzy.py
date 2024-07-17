import pytest
import subprocess
import random


def to_string(list):
    random.shuffle(list)
    converted_string = ''
    for str in list:
        converted_string = converted_string + " " + str
    return "./dist/r5mm" + converted_string


def generate_valid_command():
    cycles = random.randint(0, 2147483647)
    blocksize = random.randint(1, 4294967295)
    offset = random.randint(0, 4294967295)
    cache_size = random.randint(0, 100000)
    cache_latency = random.randint(0, 4294967295)
    memory_latency = random.randint(0, 4294967295)

    return (
        ["./dist/r5mm", "./test/empty.csv", f"-c{cycles}", f"-b{blocksize}", f"-o{offset}", f"-s{cache_size}", f"-t{cache_latency}", f"-m{memory_latency}"],
        [f" {cycles}", f" {blocksize}", f" {offset}", f" {cache_size}", f" {cache_latency}", f" {memory_latency}"]
    )


@pytest.mark.parametrize("command", [generate_valid_command() for _ in range(1000)])
def test_valid(command):
    random.seed(0)
    args, values = command
    process = subprocess.run(args, stdout=subprocess.PIPE, text=True)
    assert process.returncode == 0
    lines = process.stdout.split('#')[1]
    lines = lines.split("\n")
    lines.pop(0)
    lines.pop(0)
    for i, v in enumerate(values):
        line = lines[i].split(":")[1]
        assert line == v


inputs = [["-c ", "-c", "--cycles=", "--cycles "],
          ["-b ", "-b", "--blocksize=", "--blocksize "],
          ["-o ", "-o", "--v2b-block-offset=", "--v2b-block-offset "],
          ["-s ", "-s", "--tlb-size=", "--tlb-size "],
          ["-t ", "-t", "--tlb-latency=", "--tlb-latency "],
          ["-m ", "-m", "--memory-latency=", "--memory-latency "],
          ["-f ", "-f", "--tf=", "--tf "]]
randWrong = ["-w", "-ü", "--was", "-k", "--juz", "--cyclist", "-p"]


def template_valid_command(cycle_bool, blocksize_bool, v2b_block_offset_bool, tlbsize_bool, tlblatency_bool, memory_latency_bool, tracefile_bool, inputfile_bool):
    input_txt = []
    if random.randint(0, 1) == 1 and cycle_bool:
        tmp = inputs[0][random.randint(0, 3)] + str(random.randint(0, 2147483647))
        input_txt.append(tmp)

    if random.randint(0, 1) == 1 and blocksize_bool:
        tmp = inputs[1][random.randint(0, 3)] + str(random.randint(1, 4294967295))
        input_txt.append(tmp)

    if random.randint(0, 1) == 1 and v2b_block_offset_bool:
        tmp = inputs[2][random.randint(0, 3)] + str(random.randint(0, 4294967295))
        input_txt.append(tmp)

    if random.randint(0, 1) == 1 and tlbsize_bool:
        tmp = inputs[3][random.randint(0, 3)] + str(random.randint(0, 4294967295))
        input_txt.append(tmp)

    if random.randint(0, 1) == 1 and tlblatency_bool:
        tmp = inputs[4][random.randint(0, 3)] + str(random.randint(0, 4294967295))
        input_txt.append(tmp)

    if random.randint(0, 1) == 1 and memory_latency_bool:
        tmp = inputs[5][random.randint(0, 3)] + str(random.randint(0, 4294967295))
        input_txt.append(tmp)

    if random.randint(0, 1) == 1 and tracefile_bool:
        tmp = inputs[6][random.randint(0, 3)] + str("tracefile")  # <- only one
        input_txt.append(tmp)

    if inputfile_bool:
        input_txt.append('./test/empty.csv')

    return input_txt


def generate_invalid_command():
    ran = random.randint(0, 11)
    if ran == 0:
        arg = template_valid_command(1, 1, 1, 1, 1, 1, 1, 0)
        return to_string(arg)
    if ran == 1:
        arg = template_valid_command(1, 1, 1, 1, 1, 1, 1, 1)
        arg.append('./test/empty.csv')
        return to_string(arg)
    if ran == 2:
        arg = template_valid_command(1, 1, 1, 1, 1, 1, 1, 1)
        tmp = random.randint(0, len(randWrong) - 1)
        arg.append(randWrong[tmp])
        return to_string(arg)
    if ran == 3:
        arg = template_valid_command(0, 1, 1, 1, 1, 1, 1, 1)
        arg.append(inputs[0][random.randint(0, 3)] + str(random.randint(0, 2147483647)))
        arg.append(inputs[0][random.randint(0, 3)] + str(random.randint(0, 2147483647)))
        return to_string(arg)
    if ran == 4:
        arg = template_valid_command(1, 0, 1, 1, 1, 1, 1, 1)
        arg.append(inputs[1][random.randint(0, 3)] + str(random.randint(1, 4294967295)))
        arg.append(inputs[1][random.randint(0, 3)] + str(random.randint(1, 4294967295)))
        return to_string(arg)
    if ran == 5:
        arg = template_valid_command(1, 1, 0, 1, 1, 1, 1, 1)
        arg.append(inputs[2][random.randint(0, 3)] + str(random.randint(-2678268043, -1)))
        return to_string(arg)
    if ran == 6:
        arg = template_valid_command(1, 1, 1, 0, 1, 1, 1, 1)
        arg.append(inputs[3][random.randint(0, 3)] + str(random.randint(-3789456, -1)))
        return to_string(arg)
    if ran == 7:
        arg = template_valid_command(1, 1, 1, 1, 0, 1, 1, 1)
        arg.append(inputs[4][random.randint(0, 3)] + str(random.randint(-1563786817, -1)))
        return to_string(arg)
    if ran == 8:
        arg = template_valid_command(1, 1, 1, 1, 1, 0, 1, 1)
        arg.append(inputs[5][random.randint(0, 3)] + str(random.randint(-17963548, -1)))
        return to_string(arg)
    if ran == 9:
        arg = template_valid_command(1, 1, 1, 1, 1, 1, 1, 1)
        arg.append(inputs[3][random.randint(0, 3)] + "guten Morgen")
        return to_string(arg)
    if ran == 10:
        arg = template_valid_command(1, 1, 1, 1, 1, 1, 1, 1)
        arg.append(inputs[4][random.randint(0, 3)] + "%")
        return to_string(arg)
    if ran == 11:
        arg = template_valid_command(1, 1, 1, 1, 1, 1, 1, 1)
        arg.append(inputs[5][random.randint(0, 3)] + "hello")
        return to_string(arg)


@pytest.mark.parametrize("command", [generate_invalid_command() for _ in range(1000)])
def test_invalid(command):
    random.seed(0)
    process = subprocess.run(command, text=True, shell=True)
    assert process.returncode == 1
