import subprocess
import pytest
from parse_errors import error_messages  # Import the parsed error messages

def run_program(args):
    return subprocess.run(
        ["./r5mm"] + args,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True
    )

# Parametrize tests with different arguments
@pytest.mark.parametrize("args", [
    (["-c", "-2", "-x", "value", "Inputfile.csv"]),
    (["-b", "hallo", "-y", "value", "Inputfile.csv"]),
    (["-o", "-g", "-z", "value", "Inputfile.csv"]),
    (["-s", "aaww", "-a", "value", "Inputfile.csv"]),
    (["-t", "999999999999999", "-p", "value", "Inputfile.csv"]),
    (["-m", "-120", "-j", "value", "Inputfile.csv"]),
])
def test_unknown_options(args):
    result = run_program(args)
    assert result.returncode != 0
    assert error_messages["ILLEGAL_ARGUMENT_PASSING"] in result.stderr