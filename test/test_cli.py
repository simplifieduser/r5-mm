import subprocess
import random

import pytest

val_cyc = ["-c ", "-c", "--cycles=", "--cycles "]
val_blockS = ["-b ", "-b", "--blocksize=", "--blocksize "]
val_v2b = ["-o ", "-o", "--v2b-block-offset=", "--v2b-block-offset "]
val_tlbS = ["-s ", "-s", "--tlb-size=", "--tlb-size "]
val_tlbL = ["-t ", "-t", "--tlb-latency=", "--tlb-latency "]
val_memL = ["-m ", "-m", "--memory-latency=", "--memory-latency "]
val_tf = ["-f ", "-f", "--tf=", "--tf "]


def build_valid():
    input_txt = []
    # if random.randint(0, 1) == 1:
    #     tmp = val_cyc[random.randint(0, 3)]+str(random.randint(0, 10))
    #
    #     input_txt.append(tmp)

    if random.randint(0, 1) == 1:
        tmp = val_blockS[random.randint(0, 3)]+str(random.randint(1, 4294967295))
        input_txt.append(tmp)

    if random.randint(0, 1) == 1:
        tmp = val_v2b[random.randint(0, 3)]+str(random.randint(0, 4294967295))
        input_txt.append(tmp)

    if random.randint(0, 1) == 1:
        tmp = val_tlbS[random.randint(0, 3)]+str(random.randint(0, 4294967295))
        input_txt.append(tmp)

    if random.randint(0, 1) == 1:
        tmp = val_tlbL[random.randint(0, 3)]+str(random.randint(0, 4294967295))
        input_txt.append(tmp)

    if random.randint(0, 1) == 1:
        tmp = val_memL[random.randint(0, 3)]+str(random.randint(0, 4294967295))
        input_txt.append(tmp)

    if random.randint(0, 1) == 1:
        tmp = val_tf[random.randint(0, 3)]+str("tracefile") #<- only one
        input_txt.append(tmp)

    input_txt.append('empty.csv')

    random.shuffle(input_txt)

def to_string(list):
    val = ''
    for s in list:
        val = val + " " + s
    return s
def make_invalid(valid):
    nextFailure = random.randint(1,1)
    if nextFailure == 1: # no iputfile
        valid.remove('empty.csv')
        return to_string(valid), "Fehler: Ungültiges Argument: Es muss eine Eingabedatei als positional Argument übergeben werden\nHinweis: Verwenden Sie '-h/--help', um weiter Informationen bezüglich möglicher Argumente und ihrer Benutzung zu erhalten\n"
    # elif nextFailure ==2 :
    # elif nextFailure ==3 :
    # elif nextFailure ==4 :
    # elif nextFailure ==5 :
    # elif nextFailure ==6 :
    # elif nextFailure ==7 :

    return valid





val_cyc = ["-c ", "-c", "--cycles=", "--cycles "]
val_blockS = ["-b ", "-b", "--blocksize=", "--blocksize "]
val_v2b = ["-o ", "-o", "--v2b-block-offset=", "--v2b-block-offset "]
val_tlbS = ["-s ", "-s", "--tlb-size=", "--tlb-size "]
val_tlbL = ["-t ", "-t", "--tlb-latency=", "--tlb-latency "]
val_memL = ["-m ", "-m", "--memory-latency=", "--memory-latency "]
val_tf = ["-f ", "-f", "--tf=", "--tf "]


def build_valid():
    input_txt = []
    # if random.randint(0, 1) == 1:
    #     tmp = val_cyc[random.randint(0, 3)]+str(random.randint(0, 10))
    #
    #     input_txt.append(tmp)

    if random.randint(0, 1) == 1:
        tmp = val_blockS[random.randint(0, 3)]+str(random.randint(1, 4294967295))
        input_txt.append(tmp)

    if random.randint(0, 1) == 1:
        tmp = val_v2b[random.randint(0, 3)]+str(random.randint(0, 4294967295))
        input_txt.append(tmp)

    if random.randint(0, 1) == 1:
        tmp = val_tlbS[random.randint(0, 3)]+str(random.randint(0, 4294967295))
        input_txt.append(tmp)

    if random.randint(0, 1) == 1:
        tmp = val_tlbL[random.randint(0, 3)]+str(random.randint(0, 4294967295))
        input_txt.append(tmp)

    if random.randint(0, 1) == 1:
        tmp = val_memL[random.randint(0, 3)]+str(random.randint(0, 4294967295))
        input_txt.append(tmp)

    if random.randint(0, 1) == 1:
        tmp = val_tf[random.randint(0, 3)]+str("tracefile") #<- only one
        input_txt.append(tmp)

    input_txt.append('empty.csv')

    random.shuffle(input_txt)
    return  input_txt


def to_string(list):
    val = ''
    for s in list:
        val = val + " " + s
    return val

def make_invalid(valid):

    valid.remove('empty.csv')
    return (to_string(valid), "Fehler: Ungültiges Argument: Es muss eine Eingabedatei als positional Argument übergeben werden\nHinweis: Verwenden Sie '-h/--help', um weiter Informationen bezüglich möglicher Argumente und ihrer Benutzung zu erhalten\n")
    # elif nextFailure ==2 :
    # elif nextFailure ==3 :
    # elif nextFailure ==4 :
    # elif nextFailure ==5 :
    # elif nextFailure ==6 :
    # elif nextFailure ==7 :


@pytest.mark.parametrize("request_count", [random.randint(1, 100) for _ in range(1000)])
def test_tmp(request_count):
    testing = make_invalid(build_valid())

    string = "../dist/r5mm" + testing[0]

    process = subprocess.run([string], stderr=subprocess.PIPE, text=True, shell=True)
    assert process.returncode == 1
    assert process.stderr == testing[1]