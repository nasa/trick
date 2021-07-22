from os import EX_CANTCREAT
import json
from pprint import pprint
import os

def write_help(program):
    os.system(f"{program} --help > help.txt")

def load():
    with open("help.txt", "r") as f:
        sections = {}
        section = None
        for line in f:
            if line.endswith(":\n"):
                section = line[:-1] 
                sections[section] = []
            elif section is not None and line != "\n":
                try:
                    argIndex = line.index("--") #TODO: Add env var
                    helpIndex = line.index(" ", argIndex)
                    sections[section].append((line[argIndex:helpIndex], line[helpIndex:].strip()))
                except:
                    print("Invalid line:", line)
    myJson = {
        "sections" : {
            
        }
    }
    for section in sections:
        myJson["sections"][section] = {
            "options" : {}
        }
        for t in sections[section]:
            arg = t[0]
            help = t[1]
            try:
                s = arg.split("=")
                arg = s[0]
                if "=" in arg:
                    continue
                type = s[1]
                if "DIR" in type:
                    type = "dir"
            except:
                type = "flag"
            # print(arg, "=", type, ":", help)
            if type in ("dir", "bool", "flag"):
                if arg[-1] == "[":
                    arg = arg[:-1]
                myJson["sections"][section]["options"][arg[2:]] = {
                    # "section": section,
                    "type": type,
                    "desc":help
                }
            else:
                print("unuported type:", type)

    with open("config.json", "w") as f:
        f.write(json.dumps(myJson, indent=4))