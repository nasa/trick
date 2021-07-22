import pytest
import os
import tkinter as tk
from tkinter.constants import END
from main import Data, OptionDir, OptionBool, Section, App

def set_bool(bool, value):
    bool.bool.set(value)
    bool.handler()
    return bool

def set_dir(dir, value):
    dir.directory_entry.delete(0, END)
    dir.directory_entry.insert(0, value)
    dir.handler(None)
    return dir

def test_data():
    name = "option 1"
    obj = {
        "type" : "dir",
        "desc" : "test desc"
    }
    o = Data(**obj)
    assert obj == o._dict_()

def test_dir_option():
    json = {
        "sections" : {
            "section1" : {
                "options" : {
                    "test1" : {
                        "type" : "dir"
                    }
                }
            }
        }
    }

    test_phrase = "hello"

    data = Data(**json)
    root = tk.Tk()
    o = OptionDir(root, "section1", "test1", data)
    o.get_frame().pack()
    set_dir(o, test_phrase)
    
    json["sections"]["section1"]["options"]["test1"]["value"] = test_phrase
    assert data._dict_() == json

def test_bool_option():
    json = {
        "sections" : {
            "section1" : {
                "options" : {
                    "bool_test_case" : {
                        "type" : "bool",
                        "value" : False
                    }
                }
            }
        }
    }

    data = Data(**json)
    root = tk.Tk()
    o = OptionBool(root, "section1", "bool_test_case", data)
    o.get_frame().pack()
    set_bool(o, True)
    
    json["sections"]["section1"]["options"]["bool_test_case"]["value"] = "yes"
    assert data._dict_() == json


def test_section():
    json = {
        "sections" : {
            "section1" : {
                "options" : {
                    "bool_test_case" : {
                        "type" : "bool",
                        "value" : False
                    },
                    "dir_test_case1" : {
                        "type" : "dir"
                    },
                    "dir_test_case2" : {
                        "type" : "dir",
                        "value" : "/path/to/dir"
                    }
                }
            }
        }
    }

    data = Data(**json)
    root = tk.Tk()
    o = Section(root, "section1", data)
    o.get_frame().pack()
    for c in o.components:
        if o.components[c].type == "bool":
            o.components[c].bool.set(True)
            o.components[c].handler()
        elif o.components[c].type == "dir":
            o.components[c].directory_entry.delete(0, END)
            o.components[c].directory_entry.insert(0, "hello")
            o.components[c].handler(None)
        else:
            raise RuntimeError("Unsupported type detected!")
    
    json = {
        "sections" : {
            "section1" : {
                "options" : {
                    "bool_test_case" : {
                        "type" : "bool",
                        "value" : "yes"
                    },
                    "dir_test_case1" : {
                        "type" : "dir",
                        "value" : "hello"
                    },
                    "dir_test_case2" : {
                        "type" : "dir",
                        "value" : "hello"
                    }
                }
            }
        }
    }

    assert data._dict_() == json


