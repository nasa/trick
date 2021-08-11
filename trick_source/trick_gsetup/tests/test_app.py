import pytest
from main import App, LandingPage
from tkinter.constants import END
import os

def set_bool(bool, value):
    bool.bool.set(value)
    bool.handler()
    return bool

def set_dir(dir, value):
    dir.directory_entry.delete(0, END)
    dir.directory_entry.insert(0, value)
    dir.handler(None)
    return dir

def test_search():
    my_json = {
        "sections" : {
            "Test" : {
                "options" : {
                    "help" : {
                        "type" : "flag"
                    },
                    "another" : {
                        "type" : "bool"
                    }
                }
            }
        }
    }
    a = App(my_json)
    result = a._search("help", a.sections)
    expected = {
        "sections" : {
            "Test" : {
                "options" : {
                    "help" : {
                        "type" : "flag"
                    }
                }
            }
        }
    }
    a.get_frame().destroy()
    assert expected == result

def test_app():
    my_json = {
            "sections" : {
                "test_cases" : {
                    "size" : 12,
                    "options" : {
                        "option_name0" : {
                            "type" : "dir",
                            "value" : "/home/cherpin",
                        },
                        "option_name1" : {
                            "type" : "dir",
                            "value" : "/home/cherpin",
                            "width" : 20
                        },
                        "option_name2" : {
                            "type" : "dir",
                            "width" : 20
                        },
                        "option_name3" : {
                            "type" : "bool",
                        },
                        "option_name4" : {
                            "type" : "bool",
                            "value" : True
                        },
                        "option_name5" : {
                            "type" : "bool",
                            "value" : "yes"
                        }
                    }
                }, 
            }
        }
    a = App(my_json)
    for key, value in a.sections.items():
        for option, obj, in value.components.items():
            if obj.type == "dir":
                set_dir(obj, "hello")
            elif obj.type == "bool":
                set_bool(obj, True)
            else:
                raise RuntimeError("Unsupported type detected!")

    my_json = {
        "sections" : {
            "test_cases" : {
                "size" : 12,
                "options" : {
                    "option_name0" : {
                        "type" : "dir",
                        "value" : "hello",
                    },
                    "option_name1" : {
                        "type" : "dir",
                        "value" : "hello",
                        "width" : 20
                    },
                    "option_name2" : {
                        "type" : "dir",
                        "width" : 20,
                        "value" : "hello"
                    },
                    "option_name3" : {
                        "type" : "bool",
                        "value" : "yes"
                    },
                    "option_name4" : {
                        "type" : "bool",
                        "value" : "yes"
                    },
                    "option_name5" : {
                        "type" : "bool",
                        "value" : "yes"
                    }
                }
            }, 
        }
    }
    assert a.data._dict_() == my_json
    a.get_frame().destroy()

def test_app_with_file():
    dir_path = os.path.dirname(os.path.realpath(__file__))
    a = App(f"{dir_path}/config_for_test_app_with_file.json")
    for key, value in a.sections.items():
        for option, obj, in value.components.items():
            if obj.type == "dir":
                set_dir(obj, "hello")
            elif obj.type == "bool":
                set_bool(obj, True)
            else:
                raise RuntimeError("Unsupported type detected!")

    my_json = {
        "sections" : {
            "test_cases" : {
                "size" : 12,
                "options" : {
                    "option_name0" : {
                        "type" : "dir",
                        "value" : "hello",
                    },
                    "option_name1" : {
                        "type" : "dir",
                        "value" : "hello",
                        "width" : 20
                    },
                    "option_name2" : {
                        "type" : "dir",
                        "width" : 20,
                        "value" : "hello"
                    },
                    "option_name3" : {
                        "type" : "bool",
                        "value" : "yes"
                    },
                    "option_name4" : {
                        "type" : "bool",
                        "value" : "yes"
                    },
                    "option_name5" : {
                        "type" : "bool",
                        "value" : "yes"
                    }
                }
            }, 
        }
    }
    assert a.data._dict_() == my_json
    a.get_frame().destroy()

def test_unsupported_types():
    my_json = {
        "sections" : {
            "test_cases" : {
                "size" : 12,
                "options" : {
                    "option_name0" : {
                        "type" : "dir",
                        "value" : "hello",
                    },
                    "option_name1" : {
                        "type" : "dir",
                        "value" : "hello",
                        "width" : 20
                    },
                    "option_name2" : {
                        "type" : "dir",
                        "width" : 20,
                        "value" : "hello"
                    },
                    "option_name3" : {
                        "type" : "bool",
                        "value" : "yes"
                    },
                    "option_name4" : {
                        "type" : "envvar",
                        "value" : "yes"
                    },
                    "option_name5" : {
                        "type" : "bad",
                        "value" : "yes"
                    }
                }
            }, 
        }
    }

    with pytest.raises(RuntimeError) as e_info:
        a = App(my_json)
    e_info.value.args[0] == "Option type 'bad' in option_name5 is not implemented yet."

if __name__ == "__main__":
    test_no_config()