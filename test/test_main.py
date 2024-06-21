# import subprocess
# import pytest
#
#
# def run_program(args):
#     return subprocess.run(
#         ["./r5mm"] + args,
#         stdout=subprocess.PIPE,
#         stderr=subprocess.PIPE,
#         text=True
#     )
# @pytest.mark.parametrize("args", [
#     (["-c", "10", "-x", "value", "Inputfile.csv"]),
#     (["-b", "4", "-y", "value", "Inputfile.csv"]),
#     (["-o", "0", "-z", "value", "Inputfile.csv"]),
#     (["-s", "4", "-a", "value", "Inputfile.csv"]),
#     (["-t", "1", "-p", "value", "Inputfile.csv"]),
#     (["-m", "120", "-j", "value", "Inputfile.csv"]),
# ])
# def test_unknown_options(args):
#     result = run_program(args)
#     assert result.returncode != 0
#     assert "Fehler: es wurde eine unbekannte Option übergeben" in result.stderr