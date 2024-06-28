#These test were written by chatgpt, and partly altered
import subprocess
import pytest

def test_valid_shortoptions():
    result = subprocess.run(
        ["./r5mm", "-c", "10", "-b", "1024", "-o", "0", "-s", "16", "-t", "1", "-m", "100", "-f", "tracefile.txt", "Inputfile.csv"],
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True
    )
    assert result.returncode == 0
    output = result.stdout.splitlines()
    assert "cycles=10" in output
    assert "blocksize=1024" in output
    assert "v2bBlockOffset=0" in output
    assert "tlbSize=16" in output
    assert "tlbLatency=1" in output
    assert "memoryLatency=100" in output
    assert "tracefile=tracefile.txt" in output

def longoptions():
    result = subprocess.run(
        ["./r5mm", "--cycles", "10", "--blocksize", "1024", "-v2b-block-offset", "0", "--tlb-size", "16", "--tlb-latency", "1", "--memory-latency", "100", "--tf", "tracefile.txt", "Inputfile.csv"],
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True
    )
    assert result.returncode == 0
    output = result.stdout.splitlines()
    assert "cycles=10" in output
    assert "blocksize=1024" in output
    assert "v2bBlockOffset=0" in output
    assert "tlbSize=16" in output
    assert "tlbLatency=1" in output
    assert "memoryLatency=100" in output
    assert "tracefile=tracefile.txt" in output

def test_invalid_option():
    result = subprocess.run(
        ["./r5mm", "-x", "value"],
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True
    )
    assert result.returncode != 0
    assert "Fehler: es wurde eine unbekannte Option übergeben, bitte entferne: '-x'" in result.stderr

def test_missing_argument():
    result = subprocess.run(
        ["./r5mm", "-c"],
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True
    )
    assert result.returncode != 0
    assert "Fehler: folgende Option benötigt ein Argument: '-c'" in result.stderr

def test_invalid_argument():
    result = subprocess.run(
        ["./r5mm", "-c", "-1"],
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True
    )
    assert result.returncode != 0
    assert "Fehler: ungültiges Argument: 'cycles (-c/--cycles)' muss ein Integer im Interval [0; 2147483647] sein" in result.stderr



# Helper function to run the program
def run_program(args):
    return subprocess.run(
        ["./r5mm"] + args,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True
    )

# Test valid short options
def test_valid_shortoptions():
    result = run_program(["-c", "10", "-b", "1024", "-o", "0", "-s", "16", "-t", "1", "-m", "100", "-f", "tracefile.txt", "Inputfile.csv"])
    assert result.returncode == 0
    output = result.stdout.splitlines()
    assert "cycles=10" in output
    assert "blocksize=1024" in output
    assert "v2bBlockOffset=0" in output
    assert "tlbSize=16" in output
    assert "tlbLatency=1" in output
    assert "memoryLatency=100" in output
    assert "tracefile=tracefile.txt" in output

# Test valid long options
def test_valid_longoptions():
    result = run_program(["--cycles", "10", "--blocksize", "1024", "--v2b-block-offset", "0", "--tlb-size", "16", "--tlb-latency", "1", "--memory-latency", "100", "--tf", "tracefile.txt", "Inputfile.csv"])
    assert result.returncode == 0
    output = result.stdout.splitlines()
    assert "cycles=10" in output
    assert "blocksize=1024" in output
    assert "v2bBlockOffset=0" in output
    assert "tlbSize=16" in output
    assert "tlbLatency=1" in output
    assert "memoryLatency=100" in output
    assert "tracefile=tracefile.txt" in output

# Test invalid option
def test_invalid_option():
    result = run_program(["-x", "value"])
    assert result.returncode != 0
    assert "unknown option '-x'" in result.stderr

# Test missing argument
def test_missing_argument():
    result = run_program(["-c"])
    assert result.returncode != 0
    assert "option requires an argument -- 'c'" in result.stderr

# Test invalid argument
def test_invalid_argument():
    result = run_program(["-c", "-1"])
    assert result.returncode != 0
    assert "invalid option argument: 'cycles'" in result.stderr

# Test help option
def test_help_option():
    result = run_program(["-h"])
    assert result.returncode == 0
    assert "Usage:" in result.stderr

# Test too many arguments
def test_too_many_arguments():
    result = run_program(["-c", "10", "Inputfile.csv", "ExtraArg"])
    assert result.returncode != 0
    assert "too many arguments" in result.stderr

# Test no input file
def test_no_input_file():
    result = run_program(["-c", "10"])
    assert result.returncode != 0
    assert "no input file" in result.stderr

# Test default values
def test_default_values():
    result = run_program(["Inputfile.csv"])
    assert result.returncode == 0
    output = result.stdout.splitlines()
    assert "cycles=10" in output
    assert "blocksize=4" in output
    assert "v2bBlockOffset=5" in output
    assert "tlbSize=4" in output
    assert "tlbLatency=1" in output
    assert "memoryLatency=120" in output
    assert "inputfile=Inputfile.csv" in output

# Test edge values for cycles
@pytest.mark.parametrize("value", [0, 2147483647])
def test_edge_values_cycles(value):
    result = run_program(["-c", str(value), "Inputfile.csv"])
    assert result.returncode == 0
    output = result.stdout.splitlines()
    assert f"cycles={value}" in output

# Test edge values for blocksize
@pytest.mark.parametrize("value", [1, 4294967295])
def test_edge_values_blocksize(value):
    result = run_program(["-b", str(value), "Inputfile.csv"])
    assert result.returncode == 0
    output = result.stdout.splitlines()
    assert f"blocksize={value}" in output

# Test edge values for v2bBlockOffset
@pytest.mark.parametrize("value", [0, 4294967295])
def test_edge_values_v2b_block_offset(value):
    result = run_program(["-o", str(value), "Inputfile.csv"])
    assert result.returncode == 0
    output = result.stdout.splitlines()
    assert f"v2bBlockOffset={value}" in output

# Test edge values for tlbSize
@pytest.mark.parametrize("value", [0, 4294967295])
def test_edge_values_tlb_size(value):
    result = run_program(["-s", str(value), "Inputfile.csv"])
    assert result.returncode == 0
    output = result.stdout.splitlines()
    assert f"tlbSize={value}" in output

# Test edge values for tlbLatency
@pytest.mark.parametrize("value", [0, 4294967295])
def test_edge_values_tlb_latency(value):
    result = run_program(["-t", str(value), "Inputfile.csv"])
    assert result.returncode == 0
    output = result.stdout.splitlines()
    assert f"tlbLatency={value}" in output

# Test edge values for memoryLatency
@pytest.mark.parametrize("value", [0, 4294967295])
def test_edge_values_memory_latency(value):
    result = run_program(["-m", str(value), "Inputfile.csv"])
    assert result.returncode == 0
    output = result.stdout.splitlines()
    assert f"memoryLatency={value}" in output

# Test tracefile option without input file
def test_tracefile_without_inputfile():
    result = run_program(["-f", "tracefile.txt"])
    assert result.returncode != 0
    assert "no input file" in result.stderr

# Test all options together
def test_all_options_together():
    result = run_program(["-c", "20", "-b", "2048", "-o", "10", "-s", "32", "-t", "5", "-m", "200", "-f", "tracefile2.txt", "AnotherInputfile.csv"])
    assert result.returncode == 0
    output = result.stdout.splitlines()
    assert "cycles=20" in output
    assert "blocksize=2048" in output
    assert "v2bBlockOffset=10" in output
    assert "tlbSize=32" in output
    assert "tlbLatency=5" in output
    assert "memoryLatency=200" in output
    assert "tracefile=tracefile2.txt" in output
    assert "inputfile=AnotherInputfile.csv" in output

import subprocess
import pytest

# Helper function to run the program
def run_program(args):
    return subprocess.run(
        ["./r5mm"] + args,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True
    )

# Test valid short options
def test_valid_shortoptions():
    result = run_program(["-c", "10", "-b", "1024", "-o", "0", "-s", "16", "-t", "1", "-m", "100", "-f", "tracefile.txt", "Inputfile.csv"])
    assert result.returncode == 0
    output = result.stdout.splitlines()
    assert "cycles=10" in output
    assert "blocksize=1024" in output
    assert "v2bBlockOffset=0" in output
    assert "tlbSize=16" in output
    assert "tlbLatency=1" in output
    assert "memoryLatency=100" in output
    assert "tracefile=tracefile.txt" in output

# Test valid long options
def test_valid_longoptions():
    result = run_program(["--cycles", "10", "--blocksize", "1024", "--v2b-block-offset", "0", "--tlb-size", "16", "--tlb-latency", "1", "--memory-latency", "100", "--tf", "tracefile.txt", "Inputfile.csv"])
    assert result.returncode == 0
    output = result.stdout.splitlines()
    assert "cycles=10" in output
    assert "blocksize=1024" in output
    assert "v2bBlockOffset=0" in output
    assert "tlbSize=16" in output
    assert "tlbLatency=1" in output
    assert "memoryLatency=100" in output
    assert "tracefile=tracefile.txt" in output

# Test invalid option
def test_invalid_option():
    result = run_program(["-x", "value"])
    assert result.returncode != 0
    assert "Fehler: es wurde eine unbekannte Option übergeben, bitte entferne: '-x'" in result.stderr

# Test missing argument
def test_missing_argument():
    result = run_program(["-c"])
    assert result.returncode != 0
    assert "Fehler: folgende Option benötigt ein Argument: '-c'" in result.stderr

# Test invalid argument
def test_invalid_argument():
    result = run_program(["-c", "-1"])
    assert result.returncode != 0
    assert "Fehler: ungültiges Argument: 'cycles (-c/--cycles)' muss ein Integer im Interval [0; 2147483647] sein" in result.stderr

# Test help option
def test_help_option():
    result = run_program(["-h"])
    assert result.returncode == 0
    assert "Verwendung: r5mm" in result.stderr

# Test too many arguments
def test_too_many_arguments():
    result = run_program(["-c", "10", "Inputfile.csv", "ExtraArg"])
    assert result.returncode != 0
    assert "Fehler: es wurden zu viele Optionen übergeben" in result.stderr

# Test no input file
def test_no_input_file():
    result = run_program(["-c", "10"])
    assert result.returncode != 0
    assert "Fehler: ungültiges Argument: Es muss eine Eingabedatei als Positional Argument übergeben werden" in result.stderr

# Test default values
def test_default_values():
    result = run_program(["Inputfile.csv"])
    assert result.returncode == 0
    output = result.stdout.splitlines()
    assert "cycles=10" in output
    assert "blocksize=4" in output
    assert "v2bBlockOffset=5" in output
    assert "tlbSize=4" in output
    assert "tlbLatency=1" in output
    assert "memoryLatency=120" in output
    assert "inputfile=Inputfile.csv" in output

# Test edge values for cycles
@pytest.mark.parametrize("value", [0, 2147483647])
def test_edge_values_cycles(value):
    result = run_program(["-c", str(value), "Inputfile.csv"])
    assert result.returncode == 0
    output = result.stdout.splitlines()
    assert f"cycles={value}" in output

# Test edge values for blocksize
@pytest.mark.parametrize("value", [1, 4294967295])
def test_edge_values_blocksize(value):
    result = run_program(["-b", str(value), "Inputfile.csv"])
    assert result.returncode == 0
    output = result.stdout.splitlines()
    assert f"blocksize={value}" in output

# Test edge values for v2bBlockOffset
@pytest.mark.parametrize("value", [0, 4294967295])
def test_edge_values_v2b_block_offset(value):
    result = run_program(["-o", str(value), "Inputfile.csv"])
    assert result.returncode == 0
    output = result.stdout.splitlines()
    assert f"v2bBlockOffset={value}" in output

# Test edge values for tlbSize
@pytest.mark.parametrize("value", [0, 4294967295])
def test_edge_values_tlb_size(value):
    result = run_program(["-s", str(value), "Inputfile.csv"])
    assert result.returncode == 0
    output = result.stdout.splitlines()
    assert f"tlbSize={value}" in output

# Test edge values for tlbLatency
@pytest.mark.parametrize("value", [0, 4294967295])
def test_edge_values_tlb_latency(value):
    result = run_program(["-t", str(value), "Inputfile.csv"])
    assert result.returncode == 0
    output = result.stdout.splitlines()
    assert f"tlbLatency={value}" in output

# Test edge values for memoryLatency
@pytest.mark.parametrize("value", [0, 4294967295])
def test_edge_values_memory_latency(value):
    result = run_program(["-m", str(value), "Inputfile.csv"])
    assert result.returncode == 0
    output = result.stdout.splitlines()
    assert f"memoryLatency={value}" in output

# Test tracefile option without input file
def test_tracefile_without_inputfile():
    result = run_program(["-f", "tracefile.txt"])
    assert result.returncode != 0
    assert "Fehler: ungültiges Argument: Es muss eine Eingabedatei als Positional Argument übergeben werden" in result.stderr

# Test all options together
def test_all_options_together():
    result = run_program(["-c", "20", "-b", "2048", "-o", "10", "-s", "32", "-t", "5", "-m", "200", "-f", "tracefile2.txt", "AnotherInputfile.csv"])
    assert result.returncode == 0
    output = result.stdout.splitlines()
    assert "cycles=20" in output
    assert "blocksize=2048" in output
    assert "v2bBlockOffset=10" in output
    assert "tlbSize=32" in output
    assert "tlbLatency=5" in output
    assert "memoryLatency=200" in output
    assert "tracefile=tracefile2.txt" in output
    assert "inputfile=AnotherInputfile.csv" in output

if __name__ == "__main__":
    pytest.main()

# Helper function to run the program
def run_program(args):
    return subprocess.run(
        ["./r5mm"] + args,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True
    )

@pytest.mark.parametrize("args,expected_output", [
    (["-c", "100", "Inputfile.csv"], ["cycles=100"]),
    (["-b", "512", "Inputfile.csv"], ["blocksize=512"]),
    (["-o", "20", "Inputfile.csv"], ["v2bBlockOffset=20"]),
    (["-s", "64", "Inputfile.csv"], ["tlbSize=64"]),
    (["-t", "2", "Inputfile.csv"], ["tlbLatency=2"]),
    (["-m", "250", "Inputfile.csv"], ["memoryLatency=250"]),
    (["-f", "trace.txt", "Inputfile.csv"], ["tracefile=trace.txt"]),
    (["-c", "100", "-b", "512", "Inputfile.csv"], ["cycles=100", "blocksize=512"]),
    (["-c", "100", "-o", "20", "Inputfile.csv"], ["cycles=100", "v2bBlockOffset=20"]),
    (["-c", "100", "-s", "64", "Inputfile.csv"], ["cycles=100", "tlbSize=64"]),
    (["-c", "100", "-t", "2", "Inputfile.csv"], ["cycles=100", "tlbLatency=2"]),
    (["-c", "100", "-m", "250", "Inputfile.csv"], ["cycles=100", "memoryLatency=250"]),
    (["-c", "100", "-f", "trace.txt", "Inputfile.csv"], ["cycles=100", "tracefile=trace.txt"]),
    (["-b", "512", "-o", "20", "Inputfile.csv"], ["blocksize=512", "v2bBlockOffset=20"]),
    (["-b", "512", "-s", "64", "Inputfile.csv"], ["blocksize=512", "tlbSize=64"]),
    (["-b", "512", "-t", "2", "Inputfile.csv"], ["blocksize=512", "tlbLatency=2"]),
    (["-b", "512", "-m", "250", "Inputfile.csv"], ["blocksize=512", "memoryLatency=250"]),
    (["-b", "512", "-f", "trace.txt", "Inputfile.csv"], ["blocksize=512", "tracefile=trace.txt"]),
    (["-o", "20", "-s", "64", "Inputfile.csv"], ["v2bBlockOffset=20", "tlbSize=64"]),
    (["-o", "20", "-t", "2", "Inputfile.csv"], ["v2bBlockOffset=20", "tlbLatency=2"]),
    (["-o", "20", "-m", "250", "Inputfile.csv"], ["v2bBlockOffset=20", "memoryLatency=250"]),
    (["-o", "20", "-f", "trace.txt", "Inputfile.csv"], ["v2bBlockOffset=20", "tracefile=trace.txt"]),
    (["-s", "64", "-t", "2", "Inputfile.csv"], ["tlbSize=64", "tlbLatency=2"]),
    (["-s", "64", "-m", "250", "Inputfile.csv"], ["tlbSize=64", "memoryLatency=250"]),
    (["-s", "64", "-f", "trace.txt", "Inputfile.csv"], ["tlbSize=64", "tracefile=trace.txt"]),
    (["-t", "2", "-m", "250", "Inputfile.csv"], ["tlbLatency=2", "memoryLatency=250"]),
    (["-t", "2", "-f", "trace.txt", "Inputfile.csv"], ["tlbLatency=2", "tracefile=trace.txt"]),
    (["-m", "250", "-f", "trace.txt", "Inputfile.csv"], ["memoryLatency=250", "tracefile=trace.txt"]),
])
def test_combinations(args, expected_output):
    result = run_program(args)
    assert result.returncode == 0
    output = result.stdout.splitlines()
    for line in expected_output:
        assert line in output

@pytest.mark.parametrize("args,expected_error", [
    (["-c", "2147483648", "Inputfile.csv"], "Fehler: ungültiges Argument: 'cycles (-c/--cycles)' muss ein Integer im Interval [0; 2147483647] sein"),
    (["-b", "0", "Inputfile.csv"], "Fehler: ungültiges Argument: 'blocksize (-b/--blocksize)' muss ein Integer im Interval [1; 4294967295] sein"),
    (["-o", "4294967296", "Inputfile.csv"], "Fehler: ungültiges Argument: 'v2b-block-offset (-o/--v2b-block-offset)' muss ein Integer im Interval [0; 4294967295] sein"),
    (["-s", "4294967296", "Inputfile.csv"], "Fehler: ungültiges Argument: 'tlb-size(-s/--tlb-size)' muss ein Integer im Interval [0; 4294967295] sein"),
    (["-t", "4294967296", "Inputfile.csv"], "Fehler: ungültiges Argument: 'tlb-latency(-t/--tlb-latency)' muss ein Integer im Interval [0; 4294967295] sein"),
    (["-m", "4294967296", "Inputfile.csv"], "Fehler: ungültiges Argument: 'memory-latency(-m/--memory-latency)' muss ein Integer im Interval [0; 4294967295] sein"),
])
def test_invalid_arguments(args, expected_error):
    result = run_program(args)
    assert result.returncode != 0
    assert expected_error in result.stderr

@pytest.mark.parametrize("args", [
    (["-c", "0", "Inputfile.csv"]),
    (["-c", "2147483647", "Inputfile.csv"]),
    (["-b", "1", "Inputfile.csv"]),
    (["-b", "4294967295", "Inputfile.csv"]),
    (["-o", "0", "Inputfile.csv"]),
    (["-o", "4294967295", "Inputfile.csv"]),
    (["-s", "0", "Inputfile.csv"]),
    (["-s", "4294967295", "Inputfile.csv"]),
    (["-t", "0", "Inputfile.csv"]),
    (["-t", "4294967295", "Inputfile.csv"]),
    (["-m", "0", "Inputfile.csv"]),
    (["-m", "4294967295", "Inputfile.csv"]),
])
def test_edge_cases(args):
    result = run_program(args)
    assert result.returncode == 0

@pytest.mark.parametrize("args,expected_error", [
    (["-c", "abc", "Inputfile.csv"], "Fehler: ungültiges Argument: 'cycles (-c/--cycles)' muss ein Integer im Interval [0; 2147483647] sein"),
    (["-b", "abc", "Inputfile.csv"], "Fehler: ungültiges Argument: 'blocksize (-b/--blocksize)' muss ein Integer im Interval [1; 4294967295] sein"),
    (["-o", "abc", "Inputfile.csv"], "Fehler: ungültiges Argument: 'v2b-block-offset (-o/--v2b-block-offset)' muss ein Integer im Interval [0; 4294967295] sein"),
    (["-s", "abc", "Inputfile.csv"], "Fehler: ungültiges Argument: 'tlb-size(-s/--tlb-size)' muss ein Integer im Interval [0; 4294967295] sein"),
    (["-t", "abc", "Inputfile.csv"], "Fehler: ungültiges Argument: 'tlb-latency(-t/--tlb-latency)' muss ein Integer im Interval [0; 4294967295] sein"),
    (["-m", "abc", "Inputfile.csv"], "Fehler: ungültiges Argument: 'memory-latency(-m/--memory-latency)' muss ein Integer im Interval [0; 4294967295] sein"),
])
def test_non_numeric_arguments(args, expected_error):
    result = run_program(args)
    assert result.returncode != 0
    assert expected_error in result.stderr

@pytest.mark.parametrize("args", [
    (["-c", "10", "-b", "1024", "-o", "0", "-s", "16", "-t", "1", "-m", "100", "-f", "tracefile.txt", "Inputfile.csv"]),
    (["--cycles", "10", "--blocksize", "1024", "--v2b-block-offset", "0", "--tlb-size", "16", "--tlb-latency", "1", "--memory-latency", "100", "--tf", "tracefile.txt", "Inputfile.csv"]),
    (["-c", "0", "-b", "1", "-o", "0", "-s", "0", "-t", "0", "-m", "0", "Inputfile.csv"]),
    (["-c", "2147483647", "-b", "4294967295", "-o", "4294967295", "-s", "4294967295", "-t", "4294967295", "-m", "4294967295", "Inputfile.csv"]),
    (["--cycles", "0", "--blocksize", "1", "--v2b-block-offset", "0", "--tlb-size", "0", "--tlb-latency", "0", "--memory-latency", "0", "Inputfile.csv"]),
    (["--cycles", "2147483647", "--blocksize", "4294967295", "--v2b-block-offset", "4294967295", "--tlb-size", "4294967295", "--tlb-latency", "4294967295", "--memory-latency", "4294967295", "Inputfile.csv"]),
])
def test_all_valid_combinations(args):
    result = run_program(args)
    assert result.returncode == 0

@pytest.mark.parametrize("args,expected_error", [
    (["-c", "10", "-b", "1024", "-o", "0", "-s", "16", "-t", "1", "-m", "100", "-f", "tracefile.txt"], "Fehler: ungültiges Argument: Es muss eine Eingabedatei als Positional Argument übergeben werden"),
    (["-c", "10", "Inputfile.csv", "ExtraArg"], "Fehler: es wurden zu viele Optionen übergeben"),
    (["-c", "10", "-b"], "Fehler: folgende Option benötigt ein Argument: '-b'"),
    (["--cycles", "10", "--blocksize", "1024", "--v2b-block-offset"], "Fehler: folgende Option benötigt ein Argument: '--v2b-block-offset'"),
    (["--cycles", "10", "--blocksize", "1024", "--v2b-block-offset", "0", "--tlb-size"], "Fehler: folgende Option benötigt ein Argument: '--tlb-size'"),
    (["--cycles", "10", "--blocksize", "1024", "--v2b-block-offset", "0", "--tlb-size", "16", "--tlb-latency"], "Fehler: folgende Option benötigt ein Argument: '--tlb-latency'"),
    (["--cycles", "10", "--blocksize", "1024", "--v2b-block-offset", "0", "--tlb-size", "16", "--tlb-latency", "1", "--memory-latency"], "Fehler: folgende Option benötigt ein Argument: '--memory-latency'"),
    (["--cycles", "10", "--blocksize", "1024", "--v2b-block-offset", "0", "--tlb-size", "16", "--tlb-latency", "1", "--memory-latency", "100", "--tf"], "Fehler: folgende Option benötigt ein Argument: '--tf'"),
])
def test_missing_arguments(args, expected_error):
    result = run_program(args)
    assert result.returncode != 0
    assert expected_error in result.stderr

@pytest.mark.parametrize("args,expected_output", [
    (["-c", "0", "-b", "4", "-o", "0", "-s", "4", "-t", "1", "-m", "120", "Inputfile.csv"], ["cycles=0", "blocksize=4", "v2bBlockOffset=0", "tlbSize=4", "tlbLatency=1", "memoryLatency=120", "inputfile=Inputfile.csv"]),
    (["-c", "2147483647", "-b", "4294967295", "-o", "4294967295", "-s", "4294967295", "-t", "4294967295", "-m", "4294967295", "Inputfile.csv"], ["cycles=2147483647", "blocksize=4294967295", "v2bBlockOffset=4294967295", "tlbSize=4294967295", "tlbLatency=4294967295", "memoryLatency=4294967295", "inputfile=Inputfile.csv"]),
])
def test_edge_values_output(args, expected_output):
    result = run_program(args)
    assert result.returncode == 0
    output = result.stdout.splitlines()
    for line in expected_output:
        assert line in output

@pytest.mark.parametrize("args,expected_error", [
    (["-c", "10", "-b", "0", "Inputfile.csv"], "Fehler: ungültiges Argument: 'blocksize (-b/--blocksize)' muss ein Integer im Interval [1; 4294967295] sein"),
    (["-c", "10", "-o", "-1", "Inputfile.csv"], "Fehler: ungültiges Argument: 'v2b-block-offset (-o/--v2b-block-offset)' muss ein Integer im Interval [0; 4294967295] sein"),
    (["-c", "10", "-s", "-1", "Inputfile.csv"], "Fehler: ungültiges Argument: 'tlb-size(-s/--tlb-size)' muss ein Integer im Interval [0; 4294967295] sein"),
    (["-c", "10", "-t", "-1", "Inputfile.csv"], "Fehler: ungültiges Argument: 'tlb-latency(-t/--tlb-latency)' muss ein Integer im Interval [0; 4294967295] sein"),
    (["-c", "10", "-m", "-1", "Inputfile.csv"], "Fehler: ungültiges Argument: 'memory-latency(-m/--memory-latency)' muss ein Integer im Interval [0; 4294967295] sein"),
])
def test_invalid_negative_arguments(args, expected_error):
    result = run_program(args)
    assert result.returncode != 0
    assert expected_error in result.stderr

@pytest.mark.parametrize("args", [
    (["-c", "10", "Inputfile.csv"]),
    (["-b", "1024", "Inputfile.csv"]),
    (["-o", "0", "Inputfile.csv"]),
    (["-s", "16", "Inputfile.csv"]),
    (["-t", "1", "Inputfile.csv"]),
    (["-m", "100", "Inputfile.csv"]),
    (["-f", "tracefile.txt", "Inputfile.csv"]),
])
def test_single_option(args):
    result = run_program(args)
    assert result.returncode == 0

@pytest.mark.parametrize("args,expected_output", [
    (["-c", "20", "Inputfile.csv"], ["cycles=20"]),
    (["-b", "2048", "Inputfile.csv"], ["blocksize=2048"]),
    (["-o", "10", "Inputfile.csv"], ["v2bBlockOffset=10"]),
    (["-s", "32", "Inputfile.csv"], ["tlbSize=32"]),
    (["-t", "5", "Inputfile.csv"], ["tlbLatency=5"]),
    (["-m", "200", "Inputfile.csv"], ["memoryLatency=200"]),
    (["-f", "tracefile2.txt", "Inputfile.csv"], ["tracefile=tracefile2.txt"]),
])
def test_individual_arguments(args, expected_output):
    result = run_program(args)
    assert result.returncode == 0
    output = result.stdout.splitlines()
    for line in expected_output:
        assert line in output

@pytest.mark.parametrize("args,expected_error", [
    (["-c", "10", "-b", "0", "Inputfile.csv"], "Fehler: ungültiges Argument: 'blocksize (-b/--blocksize)' muss ein Integer im Interval [1; 4294967295] sein"),
    (["-c", "10", "-o", "4294967296", "Inputfile.csv"], "Fehler: ungültiges Argument: 'v2b-block-offset (-o/--v2b-block-offset)' muss ein Integer im Interval [0; 4294967295] sein"),
    (["-c", "10", "-s", "4294967296", "Inputfile.csv"], "Fehler: ungültiges Argument: 'tlb-size(-s/--tlb-size)' muss ein Integer im Interval [0; 4294967295] sein"),
    (["-c", "10", "-t", "4294967296", "Inputfile.csv"], "Fehler: ungültiges Argument: 'tlb-latency(-t/--tlb-latency)' muss ein Integer im Interval [0; 4294967295] sein"),
    (["-c", "10", "-m", "4294967296", "Inputfile.csv"], "Fehler: ungültiges Argument: 'memory-latency(-m/--memory-latency)' muss ein Integer im Interval [0; 4294967295] sein"),
])
def test_exceeding_max_values(args, expected_error):
    result = run_program(args)
    assert result.returncode != 0
    assert expected_error in result.stderr

@pytest.mark.parametrize("args,expected_error", [
    (["-c", "2147483648", "Inputfile.csv"], "Fehler: ungültiges Argument: 'cycles (-c/--cycles)' muss ein Integer im Interval [0; 2147483647] sein"),
    (["-b", "4294967296", "Inputfile.csv"], "Fehler: ungültiges Argument: 'blocksize (-b/--blocksize)' muss ein Integer im Interval [1; 4294967295] sein"),
    (["-o", "4294967296", "Inputfile.csv"], "Fehler: ungültiges Argument: 'v2b-block-offset (-o/--v2b-block-offset)' muss ein Integer im Interval [0; 4294967295] sein"),
    (["-s", "4294967296", "Inputfile.csv"], "Fehler: ungültiges Argument: 'tlb-size(-s/--tlb-size)' muss ein Integer im Interval [0; 4294967295] sein"),
    (["-t", "4294967296", "Inputfile.csv"], "Fehler: ungültiges Argument: 'tlb-latency(-t/--tlb-latency)' muss ein Integer im Interval [0; 4294967295] sein"),
    (["-m", "4294967296", "Inputfile.csv"], "Fehler: ungültiges Argument: 'memory-latency(-m/--memory-latency)' muss ein Integer im Interval [0; 4294967295] sein"),
])
def test_exceeding_uint32_max_values(args, expected_error):
    result = run_program(args)
    assert result.returncode != 0
    assert expected_error in result.stderr

@pytest.mark.parametrize("args", [
    (["-c", "10", "Inputfile.csv"]),
    (["-c", "10", "-b", "1024", "Inputfile.csv"]),
    (["-c", "10", "-b", "1024", "-o", "5", "Inputfile.csv"]),
    (["-c", "10", "-b", "1024", "-o", "5", "-s", "16", "Inputfile.csv"]),
    (["-c", "10", "-b", "1024", "-o", "5", "-s", "16", "-t", "1", "Inputfile.csv"]),
    (["-c", "10", "-b", "1024", "-o", "5", "-s", "16", "-t", "1", "-m", "100", "Inputfile.csv"]),
    (["-c", "10", "-b", "1024", "-o", "5", "-s", "16", "-t", "1", "-m", "100", "-f", "tracefile.txt", "Inputfile.csv"]),
])
def test_increasing_options(args):
    result = run_program(args)
    assert result.returncode == 0

@pytest.mark.parametrize("args,expected_output", [
    (["-c", "1", "Inputfile.csv"], ["cycles=1"]),
    (["-c", "2", "Inputfile.csv"], ["cycles=2"]),
    (["-c", "3", "Inputfile.csv"], ["cycles=3"]),
    (["-c", "4", "Inputfile.csv"], ["cycles=4"]),
    (["-c", "5", "Inputfile.csv"], ["cycles=5"]),
    (["-c", "6", "Inputfile.csv"], ["cycles=6"]),
    (["-c", "7", "Inputfile.csv"], ["cycles=7"]),
    (["-c", "8", "Inputfile.csv"], ["cycles=8"]),
    (["-c", "9", "Inputfile.csv"], ["cycles=9"]),
    (["-c", "10", "Inputfile.csv"], ["cycles=10"]),
])
def test_sequential_cycles(args, expected_output):
    result = run_program(args)
    assert result.returncode == 0
    output = result.stdout.splitlines()
    for line in expected_output:
        assert line in output

@pytest.mark.parametrize("args,expected_output", [
    (["-b", "1", "Inputfile.csv"], ["blocksize=1"]),
    (["-b", "2", "Inputfile.csv"], ["blocksize=2"]),
    (["-b", "3", "Inputfile.csv"], ["blocksize=3"]),
    (["-b", "4", "Inputfile.csv"], ["blocksize=4"]),
    (["-b", "5", "Inputfile.csv"], ["blocksize=5"]),
    (["-b", "6", "Inputfile.csv"], ["blocksize=6"]),
    (["-b", "7", "Inputfile.csv"], ["blocksize=7"]),
    (["-b", "8", "Inputfile.csv"], ["blocksize=8"]),
    (["-b", "9", "Inputfile.csv"], ["blocksize=9"]),
    (["-b", "10", "Inputfile.csv"], ["blocksize=10"]),
])
def test_sequential_blocksize(args, expected_output):
    result = run_program(args)
    assert result.returncode == 0
    output = result.stdout.splitlines()
    for line in expected_output:
        assert line in output

@pytest.mark.parametrize("args,expected_output", [
    (["-o", "0", "Inputfile.csv"], ["v2bBlockOffset=0"]),
    (["-o", "1", "Inputfile.csv"], ["v2bBlockOffset=1"]),
    (["-o", "2", "Inputfile.csv"], ["v2bBlockOffset=2"]),
    (["-o", "3", "Inputfile.csv"], ["v2bBlockOffset=3"]),
    (["-o", "4", "Inputfile.csv"], ["v2bBlockOffset=4"]),
    (["-o", "5", "Inputfile.csv"], ["v2bBlockOffset=5"]),
    (["-o", "6", "Inputfile.csv"], ["v2bBlockOffset=6"]),
    (["-o", "7", "Inputfile.csv"], ["v2bBlockOffset=7"]),
    (["-o", "8", "Inputfile.csv"], ["v2bBlockOffset=8"]),
    (["-o", "9", "Inputfile.csv"], ["v2bBlockOffset=9"]),
])
def test_sequential_v2b_block_offset(args, expected_output):
    result = run_program(args)
    assert result.returncode == 0
    output = result.stdout.splitlines()
    for line in expected_output:
        assert line in output

@pytest.mark.parametrize("args,expected_output", [
    (["-s", "0", "Inputfile.csv"], ["tlbSize=0"]),
    (["-s", "1", "Inputfile.csv"], ["tlbSize=1"]),
    (["-s", "2", "Inputfile.csv"], ["tlbSize=2"]),
    (["-s", "3", "Inputfile.csv"], ["tlbSize=3"]),
    (["-s", "4", "Inputfile.csv"], ["tlbSize=4"]),
    (["-s", "5", "Inputfile.csv"], ["tlbSize=5"]),
    (["-s", "6", "Inputfile.csv"], ["tlbSize=6"]),
    (["-s", "7", "Inputfile.csv"], ["tlbSize=7"]),
    (["-s", "8", "Inputfile.csv"], ["tlbSize=8"]),
    (["-s", "9", "Inputfile.csv"], ["tlbSize=9"]),
])
def test_sequential_tlb_size(args, expected_output):
    result = run_program(args)
    assert result.returncode == 0
    output = result.stdout.splitlines()
    for line in expected_output:
        assert line in output

@pytest.mark.parametrize("args,expected_output", [
    (["-t", "0", "Inputfile.csv"], ["tlbLatency=0"]),
    (["-t", "1", "Inputfile.csv"], ["tlbLatency=1"]),
    (["-t", "2", "Inputfile.csv"], ["tlbLatency=2"]),
    (["-t", "3", "Inputfile.csv"], ["tlbLatency=3"]),
    (["-t", "4", "Inputfile.csv"], ["tlbLatency=4"]),
    (["-t", "5", "Inputfile.csv"], ["tlbLatency=5"]),
    (["-t", "6", "Inputfile.csv"], ["tlbLatency=6"]),
    (["-t", "7", "Inputfile.csv"], ["tlbLatency=7"]),
    (["-t", "8", "Inputfile.csv"], ["tlbLatency=8"]),
    (["-t", "9", "Inputfile.csv"], ["tlbLatency=9"]),
])
def test_sequential_tlb_latency(args, expected_output):
    result = run_program(args)
    assert result.returncode == 0
    output = result.stdout.splitlines()
    for line in expected_output:
        assert line in output

@pytest.mark.parametrize("args,expected_output", [
    (["-m", "0", "Inputfile.csv"], ["memoryLatency=0"]),    (["-m", "1", "Inputfile.csv"], ["memoryLatency=1"]),
    (["-m", "2", "Inputfile.csv"], ["memoryLatency=2"]),
    (["-m", "3", "Inputfile.csv"], ["memoryLatency=3"]),
    (["-m", "4", "Inputfile.csv"], ["memoryLatency=4"]),
    (["-m", "5", "Inputfile.csv"], ["memoryLatency=5"]),
    (["-m", "6", "Inputfile.csv"], ["memoryLatency=6"]),
    (["-m", "7", "Inputfile.csv"], ["memoryLatency=7"]),
    (["-m", "8", "Inputfile.csv"], ["memoryLatency=8"]),
    (["-m", "9", "Inputfile.csv"], ["memoryLatency=9"]),
    (["-m", "10", "Inputfile.csv"], ["memoryLatency=10"]),
])
def test_sequential_memory_latency(args, expected_output):
    result = run_program(args)
    assert result.returncode == 0
    output = result.stdout.splitlines()
    for line in expected_output:
        assert line in output

@pytest.mark.parametrize("args,expected_output", [
    (["-f", "tracefile0.txt", "Inputfile.csv"], ["tracefile=tracefile0.txt"]),
    (["-f", "tracefile1.txt", "Inputfile.csv"], ["tracefile=tracefile1.txt"]),
    (["-f", "tracefile2.txt", "Inputfile.csv"], ["tracefile=tracefile2.txt"]),
    (["-f", "tracefile3.txt", "Inputfile.csv"], ["tracefile=tracefile3.txt"]),
    (["-f", "tracefile4.txt", "Inputfile.csv"], ["tracefile=tracefile4.txt"]),
    (["-f", "tracefile5.txt", "Inputfile.csv"], ["tracefile=tracefile5.txt"]),
    (["-f", "tracefile6.txt", "Inputfile.csv"], ["tracefile=tracefile6.txt"]),
    (["-f", "tracefile7.txt", "Inputfile.csv"], ["tracefile=tracefile7.txt"]),
    (["-f", "tracefile8.txt", "Inputfile.csv"], ["tracefile=tracefile8.txt"]),
    (["-f", "tracefile9.txt", "Inputfile.csv"], ["tracefile=tracefile9.txt"]),
])
def test_sequential_tracefile(args, expected_output):
    result = run_program(args)
    assert result.returncode == 0
    output = result.stdout.splitlines()
    for line in expected_output:
        assert line in output

@pytest.mark.parametrize("args", [
    (["-c", "0", "-b", "4", "-o", "0", "-s", "4", "-t", "1", "-m", "120", "-f", "tracefile.txt", "Inputfile.csv"]),
    (["-c", "2147483647", "-b", "4294967295", "-o", "4294967295", "-s", "4294967295", "-t", "4294967295", "-m", "4294967295", "-f", "tracefile2.txt", "Inputfile.csv"]),
    (["--cycles", "0", "--blocksize", "4", "--v2b-block-offset", "0", "--tlb-size", "4", "--tlb-latency", "1", "--memory-latency", "120", "--tf", "tracefile.txt", "Inputfile.csv"]),
    (["--cycles", "2147483647", "--blocksize", "4294967295", "--v2b-block-offset", "4294967295", "--tlb-size", "4294967295", "--tlb-latency", "4294967295", "--memory-latency", "4294967295", "--tf", "tracefile2.txt", "Inputfile.csv"]),
])
def test_full_valid_combinations(args):
    result = run_program(args)
    assert result.returncode == 0

@pytest.mark.parametrize("args,expected_error", [
    (["-c", "2147483648", "-b", "4", "Inputfile.csv"], "Fehler: ungültiges Argument: 'cycles (-c/--cycles)' muss ein Integer im Interval [0; 2147483647] sein"),
    (["-c", "10", "-b", "0", "Inputfile.csv"], "Fehler: ungültiges Argument: 'blocksize (-b/--blocksize)' muss ein Integer im Interval [1; 4294967295] sein"),
    (["-c", "10", "-o", "4294967296", "Inputfile.csv"], "Fehler: ungültiges Argument: 'v2b-block-offset (-o/--v2b-block-offset)' muss ein Integer im Interval [0; 4294967295] sein"),
    (["-c", "10", "-s", "4294967296", "Inputfile.csv"], "Fehler: ungültiges Argument: 'tlb-size(-s/--tlb-size)' muss ein Integer im Interval [0; 4294967295] sein"),
    (["-c", "10", "-t", "4294967296", "Inputfile.csv"], "Fehler: ungültiges Argument: 'tlb-latency(-t/--tlb-latency)' muss ein Integer im Interval [0; 4294967295] sein"),
    (["-c", "10", "-m", "4294967296", "Inputfile.csv"], "Fehler: ungültiges Argument: 'memory-latency(-m/--memory-latency)' muss ein Integer im Interval [0; 4294967295] sein"),
])
def test_invalid_large_arguments(args, expected_error):
    result = run_program(args)
    assert result.returncode != 0
    assert expected_error in result.stderr

@pytest.mark.parametrize("args,expected_error", [
    (["-c", "10", "-b", "4"], "Fehler: ungültiges Argument: Es muss eine Eingabedatei als Positional Argument übergeben werden"),
    (["-c", "10", "-b", "4", "-o", "0"], "Fehler: ungültiges Argument: Es muss eine Eingabedatei als Positional Argument übergeben werden"),
    (["-c", "10", "-b", "4", "-o", "0", "-s", "4"], "Fehler: ungültiges Argument: Es muss eine Eingabedatei als Positional Argument übergeben werden"),
    (["-c", "10", "-b", "4", "-o", "0", "-s", "4", "-t", "1"], "Fehler: ungültiges Argument: Es muss eine Eingabedatei als Positional Argument übergeben werden"),
    (["-c", "10", "-b", "4", "-o", "0", "-s", "4", "-t", "1", "-m", "120"], "Fehler: ungültiges Argument: Es muss eine Eingabedatei als Positional Argument übergeben werden"),
    (["-c", "10", "-b", "4", "-o", "0", "-s", "4", "-t", "1", "-m", "120", "-f", "tracefile.txt"], "Fehler: ungültiges Argument: Es muss eine Eingabedatei als Positional Argument übergeben werden"),
])
def test_missing_inputfile(args, expected_error):
    result = run_program(args)
    assert result.returncode != 0
    assert expected_error in result.stderr

@pytest.mark.parametrize("args", [
    (["-h"]),
    (["--help"]),
])
def test_help_message(args):
    result = run_program(args)
    assert result.returncode == 0
    assert "Verwendung: r5mm" in result.stderr

@pytest.mark.parametrize("args", [
    (["-c", "10", "-x", "value", "Inputfile.csv"]),
    (["-b", "4", "-y", "value", "Inputfile.csv"]),
    (["-o", "0", "-z", "value", "Inputfile.csv"]),
    (["-s", "4", "-a", "value", "Inputfile.csv"]),
    (["-t", "1", "-q", "value", "Inputfile.csv"]),
    (["-m", "120", "-i", "value", "Inputfile.csv"]),
])
def test_unknown_options(args):
    result = run_program(args)
    assert result.returncode != 0
    assert "Fehler: es wurde eine unbekannte Option übergeben" in result.stderr

@pytest.mark.parametrize("args", [
    (["-c", "10", "-b", "1024", "-o", "5", "-s", "16", "-t", "1", "-m", "100", "-f", "tracefile.txt", "Inputfile.csv"]),
    (["--cycles", "10", "--blocksize", "1024", "--v2b-block-offset", "5", "--tlb-size", "16", "--tlb-latency", "1", "--memory-latency", "100", "--tf", "tracefile.txt", "Inputfile.csv"]),
])
def test_combined_valid_options(args):
    result = run_program(args)
    assert result.returncode == 0
    output = result.stdout.splitlines()
    assert "cycles=10" in output
    assert "blocksize=1024" in output
    assert "v2bBlockOffset=5" in output
    assert "tlbSize=16" in output
    assert "tlbLatency=1" in output
    assert "memoryLatency=100" in output
    assert "tracefile=tracefile.txt" in output

@pytest.mark.parametrize("args,expected_error", [
    (["-c", "", "Inputfile.csv"], "Fehler: ungültiges Argument: "),
    (["-b", "", "Inputfile.csv"], "Fehler: ungültiges Argument: "),
    (["-o", "", "Inputfile.csv"], "Fehler: ungültiges Argument: "),
    (["-s", "", "Inputfile.csv"], "Fehler: ungültiges Argument: "),
    (["-t", "", "Inputfile.csv"], "Fehler: ungültiges Argument: "),
    (["-m", "", "Inputfile.csv"], "Fehler: ungültiges Argument: "),
])
def test_empty_arguments(args, expected_error):
    result = run_program(args)
    assert result.returncode != 0
    assert expected_error in result.stderr

@pytest.mark.parametrize("args", [
    (["-c", "abc", "Inputfile.csv"]),
    (["-b", "xyz", "Inputfile.csv"]),
    (["-o", "def", "Inputfile.csv"]),
    (["-s", "uvw", "Inputfile.csv"]),
    (["-t", "lmn", "Inputfile.csv"]),
    (["-m", "pqr", "Inputfile.csv"]),
])
def test_non_numeric_invalid_arguments(args):
    result = run_program(args)
    assert result.returncode != 0
    assert "Fehler: ungültiges Argument" in result.stderr

@pytest.mark.parametrize("args,expected_output", [
    (["-c", "0", "Inputfile.csv"], ["cycles=0"]),
    (["-b", "1", "Inputfile.csv"], ["blocksize=1"]),
    (["-o", "0", "Inputfile.csv"], ["v2bBlockOffset=0"]),
    (["-s", "0", "Inputfile.csv"], ["tlbSize=0"]),
    (["-t", "0", "Inputfile.csv"], ["tlbLatency=0"]),
    (["-m", "0", "Inputfile.csv"], ["memoryLatency=0"]),
])
def test_min_edge_values(args, expected_output):
    result = run_program(args)
    assert result.returncode == 0
    output = result.stdout.splitlines()
    for line in expected_output:
        assert line in output

@pytest.mark.parametrize("args,expected_output", [
    (["-c", "2147483647", "Inputfile.csv"], ["cycles=2147483647"]),
    (["-b", "4294967295", "Inputfile.csv"], ["blocksize=4294967295"]),
    (["-o", "4294967295", "Inputfile.csv"], ["v2bBlockOffset=4294967295"]),
    (["-s", "4294967295", "Inputfile.csv"], ["tlbSize=4294967295"]),
    (["-t", "4294967295", "Inputfile.csv"], ["tlbLatency=4294967295"]),
    (["-m", "4294967295", "Inputfile.csv"], ["memoryLatency=4294967295"]),
])
def test_max_edge_values(args, expected_output):
    result = run_program(args)
    assert result.returncode == 0
    output = result.stdout.splitlines()
    for line in expected_output:
        assert line in output

@pytest.mark.parametrize("args", [
    (["-c", "2147483647", "Inputfile.csv"]),
    (["-b", "4294967295", "Inputfile.csv"]),
    (["-o", "4294967295", "Inputfile.csv"]),
    (["-s", "4294967295", "Inputfile.csv"]),
    (["-t", "4294967295", "Inputfile.csv"]),
    (["-m", "4294967295", "Inputfile.csv"]),
])
def test_max_values(args):
    result = run_program(args)
    assert result.returncode == 0

@pytest.mark.parametrize("args", [
    (["-c", "-1", "Inputfile.csv"]),
    (["-b", "0", "Inputfile.csv"]),
    (["-o", "-1", "Inputfile.csv"]),
    (["-s", "-1", "Inputfile.csv"]),
    (["-t", "-1", "Inputfile.csv"]),
    (["-m", "-1", "Inputfile.csv"]),
])
def test_invalid_negative_values(args):
    result = run_program(args)
    assert result.returncode != 0
    assert "Fehler: ungültiges Argument" in result.stderr

@pytest.mark.parametrize("args,expected_output", [
    (["--cycles", "10", "--blocksize", "1024", "--v2b-block-offset", "0", "--tlb-size", "16", "--tlb-latency", "1", "--memory-latency", "100", "--tf", "tracefile.txt", "Inputfile.csv"], [
        "cycles=10",
        "blocksize=1024",
        "v2bBlockOffset=0",
        "tlbSize=16",
        "tlbLatency=1",
        "memoryLatency=100",
        "tracefile=tracefile.txt",
        "inputfile=Inputfile.csv"
    ]),
])
def test_combined_long_options(args, expected_output):
    result = run_program(args)
    assert result.returncode == 0
    output = result.stdout.splitlines()
    for line in expected_output:
        assert line in output

@pytest.mark.parametrize("args", [
    (["-h"]),
    (["--help"]),
])
def test_help_option(args):
    result = run_program(args)
    assert result.returncode == 0
    assert "Verwendung: r5mm" in result.stderr

@pytest.mark.parametrize("args,expected_output", [
    (["-c", "20", "-b", "2048", "-o", "10", "-s", "32", "-t", "5", "-m", "200", "-f", "tracefile2.txt", "AnotherInputfile.csv"], [
        "cycles=20",
        "blocksize=2048",
        "v2bBlockOffset=10",
        "tlbSize=32",
        "tlbLatency=5",
        "memoryLatency=200",
        "tracefile=tracefile2.txt",
        "inputfile=AnotherInputfile.csv"
    ]),
])
def test_full_argument_set(args, expected_output):
    result = run_program(args)
    assert result.returncode == 0
    output = result.stdout.splitlines()
    for line in expected_output:
        assert line in output

if __name__ == "__main__":
    pytest.main()