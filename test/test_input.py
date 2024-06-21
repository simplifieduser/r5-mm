import subprocess
import pytest

#some of the following test have been created with the help of chatgpt

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