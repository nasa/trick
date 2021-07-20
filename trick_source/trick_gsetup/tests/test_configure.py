import pytest
import os

from main import string_to_bool, bool_to_string, run, get_configure_command



def test_configure_flag_true():
    program = "/home/cherpin/git/trick/configure"
    expected = program + " --help"
    my_json = {
        "sections" : {
            "test" : {
                "options" : {
                    "help" : {
                        "value" : True,
                        "type" : "flag"
                    }
                }
            }
        }
    }
    
    cmd = get_configure_command(program, my_json)
    assert cmd == expected

    my_json["sections"]["test"]["options"]["help"]["value"] = "true"
    cmd = get_configure_command(program, my_json)
    assert cmd == expected

    my_json["sections"]["test"]["options"]["help"]["value"] = "True"
    cmd = get_configure_command(program, my_json)
    assert cmd == expected

    my_json["sections"]["test"]["options"]["help"]["value"] = "yes"
    cmd = get_configure_command(program, my_json)
    assert cmd == expected

    my_json["sections"]["test"]["options"]["help"]["value"] = "YES"
    cmd = get_configure_command(program, my_json)
    assert cmd == expected

def test_configure_flag_false():
    program = "/home/cherpin/git/trick/configure"
    expected = program
    my_json = {
        "sections" : {
            "test" : {
                "options" : {
                    "help" : {
                        "value" : False,
                        "type" : "flag"
                    }
                }
            }
        }
    }
    
    cmd = get_configure_command(program, my_json)
    assert cmd == expected

    my_json["sections"]["test"]["options"]["help"]["value"] = "false"
    cmd = get_configure_command(program, my_json)
    assert cmd == expected

    my_json["sections"]["test"]["options"]["help"]["value"] = "False"
    cmd = get_configure_command(program, my_json)
    assert cmd == expected

    my_json["sections"]["test"]["options"]["help"]["value"] = "no"
    cmd = get_configure_command(program, my_json)
    assert cmd == expected

    my_json["sections"]["test"]["options"]["help"]["value"] = "NO"
    cmd = get_configure_command(program, my_json)
    assert cmd == expected

def test_configure_bool():
    program = "/home/cherpin/git/trick/configure"
    expected = program + " --help='yes'"
    my_json = {
        "sections" : {
            "test" : {
                "options" : {
                    "help" : {
                        "value" : True,
                        "type" : "bool"
                    }
                }
            }
        }
    }
    
    cmd = get_configure_command(program, my_json)
    assert cmd == expected

    my_json["sections"]["test"]["options"]["help"]["value"] = "true"
    cmd = get_configure_command(program, my_json)
    assert cmd == expected

    my_json["sections"]["test"]["options"]["help"]["value"] = "True"
    cmd = get_configure_command(program, my_json)
    assert cmd == expected

    my_json["sections"]["test"]["options"]["help"]["value"] = "yes"
    cmd = get_configure_command(program, my_json)
    assert cmd == expected

    my_json["sections"]["test"]["options"]["help"]["value"] = "YES"
    cmd = get_configure_command(program, my_json)
    assert cmd == expected

def test_configure_empty_string():
    program = "/home/cherpin/git/trick/configure"
    expected = program + " --help='yes'"
    my_json = {
        "sections" : {
            "test" : {
                "options" : {
                    "test_option" : {
                        "value" : "",
                        "type" : "dir"
                    }
                }
            }
        }
    }
    
    cmd = get_configure_command(program, my_json)
    assert cmd == program 

def test_string_to_bool():
    test_cases_pass_fail_true = ("true", "True", "TRUE", "yes")
    test_cases_pass_fail_false = ("false", "False", "FALSE", "no")
    for test_case in test_cases_pass_fail_true:
        assert string_to_bool(test_case) == True
    for test_case in test_cases_pass_fail_false:
        assert string_to_bool(test_case) == False



def test_bool_to_sting():
    assert bool_to_string(True) == "yes"
    assert bool_to_string(False) == "no"

def test_run():
    stdout = run('echo Hello World!')
    assert stdout == "Hello World!\n"
    stdout = run("configure")
    assert stdout == "/bin/sh: 1: configure: not found\n"
    stdout = run("python3 check_env.py", "hello")
    assert stdout == "Hello World!\n"

def test_envvar():
    my_json = {
        "sections" : {
            "Test" : {
                "options" : {
                    "help" : {
                        "type" : "flag",
                        "value" : "True"
                    },
                    "CC1" : {
                        "type" : "envvar",
                        "value" : ""
                    },
                    "CC2" : {
                        "type" : "envvar",
                        "value" : "hello"
                    },
                    "CC3" : {
                        "type" : "envvar",
                        "value" : "hello world"
                    }
                }
            }
        }
    }
    cmd = get_configure_command("test", my_json, include_vars=True)
    assert os.environ.get("CC1") == None
    assert os.environ.get("CC2") == "hello"
    assert os.environ.get("CC3") == "hello world"
    assert cmd == "CC2 = hello\nCC3 = hello world\ntest --help"

def test_unsupported_type():
    my_json = {
        "sections" : {
            "Test" : {
                "options" : {
                    "help" : {
                        "type" : "flag",
                        "value" : "True"
                    },
                    "CC1" : {
                        "type" : "envvar",
                        "value" : ""
                    },
                    "unsupported" : {
                        "type" : "bad",
                        "value" : ""
                    }
                }
            }
        }
    }
    with pytest.raises(RuntimeError) as e_info:
        cmd = get_configure_command("test", my_json)
    assert e_info.value.args[0] == "In function call get_configure_command: Option type 'bad' in {'type': 'bad', 'value': ''} is not implemented yet."
