import subprocess
import pytest

# Erstellung eigener Testcases: in main.c die kommentierten Zeilen am Ende aktivieren und den Aufruf der Methode
# auskommentieren, da in der Methode weitere Fehler entstehen können, die in diesen Testern nicht vorgesehen sind.
# Diese Methode kann über die anderen Test alleine oder in kombination getestet werden

# expected: der erwartete Output aufgeteilt in
#       returncode: 0 bzw. 1
#       values: die werte die als Input übergeben werden sollen, diese werden automatisch als Sollwerte gespeichert
#               "INPUT", ist für die eingabe Datei, das INPUT wird vor dem Ausführen rausgefiltert
#                um leere Argumente zu übergeben "" einfügen, wird rausgefiltert
#       error: die erwartete Fehlernachricht


default_values = {
    "cycles": 10000,
    "blocksize": 4,
    "v2bBlockOffset": 4,
    "tlbSize": 64,
    "tlbLatency": 1,
    "memoryLatency": 100,
    "tracefile": None,
    "inputfile": None
}

# Testfälle
test_cases = [
    # Erfolgreiche Fälle
    {
        "expected": {
            "returncode": 0,
            "values": {
                "-c": 10,
                "-b": 1024,
                "-o": 0,
                "-s": 16,
                "-t": 1,
                "-m": 100,
                "-f": "temp_tracefile.txt",
                "INPUT": "test/empty.csv"
            }
        }
    },
    {
        "expected": {
            "returncode": 0,
            "values": {
                "--cycles": 10,
                "--blocksize": 1024,
                "--v2b-block-offset": 0,
                "--tlb-size": 16,
                "--tlb-latency": 1,
                "--memory-latency": 100,
                "--tf": "temp_tracefile.txt",
                "INPUT": "test/empty.csv"
            }
        }
    },
    {
        "expected": {
            "returncode": 0,
            "values": {
                "-c": 75,
                "-b": 64,
                "-o": 9,
                "-s": 512,
                "-t": 12,
                "-m": 400,
                "-f": "temp_tracefile7.txt",
                "INPUT": "test/empty.csv"
            }
        }
    },
    {
        "expected": {
            "returncode": 0,
            "values": {
                "--cycles": 35,
                "--blocksize": 256,
                "--v2b-block-offset": 10,
                "--tlb-size": 1024,
                "--tlb-latency": 6,
                "--memory-latency": 450,
                "--tf": "temp_tracefile8.txt",
                "INPUT": "test/empty.csv"
            }
        }
    },
    {
        "expected": {
            "returncode": 0,
            "values": {
                "-c": 5,
                "-b": 512,
                "-o": 2,
                "-s": 8,
                "-t": 3,
                "-m": 50,
                "-f": "temp_tracefile1.txt",
                "INPUT": "test/empty.csv"
            }
        }
    },
    {
        "expected": {
            "returncode": 0,
            "values": {
                "--cycles": 20,
                "--blocksize": 2048,
                "--v2b-block-offset": 1,
                "--tlb-size": 32,
                "--tlb-latency": 2,
                "--memory-latency": 150,
                "--tf": "temp_tracefile2.txt",
                "INPUT": "test/empty.csv"
            }
        }
    },
    {
        "expected": {
            "returncode": 0,
            "values": {
                "-c": 30,
                "-b": 4096,
                "-o": 3,
                "-s": 64,
                "-t": 4,
                "-m": 200,
                "-f": "temp_tracefile3.txt",
                "INPUT": "test/empty.csv"
            }
        }
    },
    {
        "expected": {
            "returncode": 0,
            "values": {
                "--cycles": 15,
                "--blocksize": 1024,
                "--v2b-block-offset": 5,
                "--tlb-size": 128,
                "--tlb-latency": 5,
                "--memory-latency": 250,
                "--tf": "temp_tracefile4.txt",
                "INPUT": "test/empty.csv"
            }
        }
    },
    {
        "expected": {
            "returncode": 0,
            "values": {
                "-c": 50,
                "-b": 256,
                "-o": 1,
                "-s": 32,
                "-t": 10,
                "-m": 300,
                "-f": "temp_tracefile5.txt",
                "INPUT": "test/empty.csv"
            }
        }
    },
    {
        "expected": {
            "returncode": 0,
            "values": {
                "--cycles": 25,
                "--blocksize": 128,
                "--v2b-block-offset": 7,
                "--tlb-size": 256,
                "--tlb-latency": 8,
                "--memory-latency": 350,
                "--tf": "temp_tracefile6.txt",
                "INPUT": "test/empty.csv"
            }
        }
    },{
        "expected": {
            "returncode": 0,
            "values": {
                "-c": 100,
                "-b": 8192,
                "-o": 15,
                "-s": 128,
                "-t": 15,
                "-m": 500,
                "-f": "temp_tracefile9.txt",
                "INPUT": "test/empty.csv"
            }
        }
    },
    {
        "expected": {
            "returncode": 0,
            "values": {
                "--cycles": 50,
                "--blocksize": 512,
                "--v2b-block-offset": 20,
                "--tlb-size": 2048,
                "--tlb-latency": 20,
                "--memory-latency": 550,
                "--tf": "temp_tracefile10.txt",
                "INPUT": "test/empty.csv"
            }
        }
    },
    {
        "expected": {
            "returncode": 0,
            "values": {
                "-c": 150,
                "-b": 1024,
                "-o": 5,
                "-s": 64,
                "-t": 3,
                "-m": 600,
                "-f": "temp_tracefile11.txt",
                "INPUT": "test/empty.csv"
            }
        }
    },
    {
        "expected": {
            "returncode": 0,
            "values": {
                "--cycles": 75,
                "--blocksize": 2048,
                "--v2b-block-offset": 0,
                "--tlb-size": 16,
                "--tlb-latency": 1,
                "--memory-latency": 700,
                "--tf": "temp_tracefile12.txt",
                "INPUT": "test/empty.csv"
            }
        }
    },
    # Fehlerfälle
    {
        "expected": {
            "returncode": 1,
            "values": {
                "--cycles": 10,
                "-x": 12,
                "--blocksize": 1024,
                "--v2b-block-offset": 0,
            },
            "error": "Fehler: Es wurde eine unbekannte Option übergeben, bitte entferne: '-x'"
        }
    },
    {
        "expected": {
            "returncode": 1,
            "values": {
                "-c": "",
            },
            "error": "Fehler: Folgende Option benötigt ein Argument: '-c'"
        }
    },
    {
        "expected": {
            "returncode": 1,
            "values": {
                "-c": -1,
            },
            "error": "Fehler: Ungültiges Argument: 'cycles (-c/--cycles)' muss ein Integer im Interval [0; 2147483647] sein"
        }
    },
    {
        "expected": {
            "returncode": 1,
            "values": {
                "--blocksize": -1,
            },
            "error": "Fehler: Ungültiges Argument: 'blocksize (-b/--blocksize)' muss ein Integer im Interval [1; 4294967295] sein"
        }
    },
    {
        "expected": {
            "returncode": 1,
            "values": {
                "-s": 4294967296,
            },
            "error": "Fehler: Ungültiges Argument: 'tlb-size (-s/--tlb-size)' muss ein Integer im Interval [0; 4294967295] sein"
        }
    },
    {
        "expected": {
            "returncode": 1,
            "values": {
                "--tlb-latency": "abc",
            },
            "error": "Fehler: Ungültiges Argument: 'tlb-latency (-t/--tlb-latency)' muss ein Integer im Interval [0; 4294967295] sein"
        }
    },
    {
        "expected": {
            "returncode": 1,
            "values": {
                "-f": "",
            },
            "error": "Fehler: Folgende Option benötigt ein Argument: '-f'"
        }
    },
    {
        "expected": {
            "returncode": 1,
            "values": {
                "--blocksize": "notanumber",
            },
            "error": "Fehler: Ungültiges Argument: 'blocksize (-b/--blocksize)' muss ein Integer im Interval [1; 4294967295] sein"
        }
    },
    {
        "expected": {
            "returncode": 1,
            "values": {
                "-x": 50,
            },
            "error": "Fehler: Es wurde eine unbekannte Option übergeben, bitte entferne: '-x'"
        }
    },
    {
        "expected": {
            "returncode": 1,
            "values": {
                "-t": -5,
            },
            "error": "Fehler: Ungültiges Argument: 'tlb-latency (-t/--tlb-latency)' muss ein Integer im Interval [0; 4294967295] sein"
        }
    },
    {
        "expected": {
            "returncode": 1,
            "values": {
                "-c": 2147483648,
            },
            "error": "Fehler: Ungültiges Argument: 'cycles (-c/--cycles)' muss ein Integer im Interval [0; 2147483647] sein"
        }
    },
    {
        "expected": {
            "returncode": 1,
            "values": {
                "--blocksize": 0,
            },
            "error": "Fehler: Ungültiges Argument: 'blocksize (-b/--blocksize)' muss ein Integer im Interval [1; 4294967295] sein"
        }
    },
    {
        "expected": {
            "returncode": 1,
            "values": {
                "-m": -100,
            },
            "error": "Fehler: Ungültiges Argument: 'memory-latency (-m/--memory-latency)' muss ein Integer im Interval [0; 4294967295] sein"
        }
    },
    {
        "expected": {
            "returncode": 1,
            "values": {
                "--tlb-size": 4294967296,
            },
            "error": "Fehler: Ungültiges Argument: 'tlb-size (-s/--tlb-size)' muss ein Integer im Interval [0; 4294967295] sein"
        }
    },

]


def run_program(args):
    print(' '.join(args))
    return subprocess.run(
        args,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True
    )


@pytest.mark.parametrize("test_case", test_cases)
def test_cases_runner(test_case):
    expected = test_case["expected"]

    expected_values = default_values.copy()
    args = ["./dist/r5mm"]

    for key, value in expected["values"].items():  # filtert und erstellt den Input
        key_str = str(key)
        if key_str == "INPUT":  # die Inputfile ist ein Partial argument, deswegen ohne key
            args.append(str(value))
        elif str(value) == "":  # falls eine Option ohne Argument übergeben werden soll
            args.append(key_str)
        else:
            args.append(key_str)
            args.append(str(value))

        if key_str == "--cycles" or key_str == "-c":
            expected_values["cycles"] = value
        elif key_str == "--blocksize" or key_str == "-b":
            expected_values["blocksize"] = value
        elif key_str == "--v2b-block-offset" or key_str == "-o":
            expected_values["v2bBlockOffset"] = value
        elif key_str == "--tlb-size" or key_str == "-s":
            expected_values["tlbSize"] = value
        elif key_str == "--tlb-latency" or key_str == "-t":
            expected_values["tlbLatency"] = value
        elif key_str == "--memory-latency" or key_str == "-m":
            expected_values["memoryLatency"] = value
        elif key_str == "--tf" or key_str == "-f":
            expected_values["tracefile"] = value
        elif key_str == "INPUT":
            expected_values["inputfile"] = value

    result = run_program(args)

    assert result.returncode == expected["returncode"]

    if expected["returncode"] == 0:
        output = result.stdout.splitlines()
        for key, value in expected_values.items():
            if key == "tracefile" and value == "":
                continue  # Ignore tracefile check if not set
            assert f"{key}={value}" in output
    else:
        assert expected["error"] in result.stderr


if __name__ == "__main__":
    pytest.main()

#dieser Test basiert auf einer Vorlage von ChatGPT, wurde jedoch stark angepasst und verändert
