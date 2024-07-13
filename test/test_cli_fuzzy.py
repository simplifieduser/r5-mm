import pytest
import subprocess
import random

def generate_valid_command():
    cycles = random.randint(0, 2147483647)
    blocksize = random.randint(1, 4294967295)
    offset = random.randint(0, 4294967295)
    cache_size = random.randint(0, 100000)
    cache_latency = random.randint(0, 4294967295)
    memory_latency = random.randint(0, 4294967295)

    return (["./dist/r5mm", "./test/empty.csv", f"-c{cycles}", f"-b{blocksize}", f"-o{offset}", f"-s{cache_size}", f"-t{cache_latency}", f"-m{memory_latency}"],
            f"1 Optionen und ihre jeweiligen Argumente:\n\n'cycles (-c/--cycles)'                      hat den Wert: {cycles}\n'blocksize (-b/--blocksize)'                hat den Wert: {blocksize}\n'v2b-block-offset (-o/--v2b-block-offset)'  hat den Wert: {offset}\n'tlb-size (-s/--tlb-size)'                  hat den Wert: {cache_size}\n'tlb-latency (-t/--tlb-latency)'            hat den Wert: {cache_latency}\n'memory-latency (-m/--memory-latency)'      hat den Wert: {memory_latency}\n'tracefile (-f/--tf)'                       hat den Wert: (null)\n'Eingabedatei'                              hat den Wert: ./test/empty.csv")


@pytest.mark.parametrize("command", [generate_valid_command() for _ in range(1000000)])
def test_cli_fuzzy(command):
    args, values = command
    process = subprocess.run(args, stdout=subprocess.PIPE, text=True)
    assert process.returncode == 0
    lines = process.stdout.split('#')[1]
    lines = lines.strip()
    assert lines == values
