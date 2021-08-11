#!/usr/bin/python3
from posixpath import basename, curdir
import tkinter

import tkinter as tk
from tkinter import PhotoImage, StringVar, Tk, ttk
from tkinter import BooleanVar, Toplevel, Text, Menu, Canvas
from tkinter.constants import ANCHOR, NONE, SUNKEN
from tkinter.ttk import Frame, Button, Entry, Label, Checkbutton, LabelFrame, Radiobutton, Scrollbar
from tkinter import ttk
import json
from tkinter import filedialog
from tkinter.ttk import Notebook
import subprocess
from tkinter import messagebox
import logging
from tkinter.scrolledtext import ScrolledText
import traceback
import os
import argparse
import shutil
import time
import errno
import ntpath
import glob
import sys
import datetime


sys.path.append(os.path.dirname(os.path.realpath(__file__)))
from default_trick_config import default_trick_config

def QuoteForPOSIX(string): #Adapted from https://code.activestate.com/recipes/498202-quote-python-strings-for-safe-use-in-posix-shells/
    '''quote a string so it can be used as an argument in a  posix shell

       According to: http://www.unix.org/single_unix_specification/
          2.2.1 Escape Character (Backslash)

          A backslash that is not quoted shall preserve the literal value
          of the following character, with the exception of a <newline>.

          2.2.2 Single-Quotes

          Enclosing characters in single-quotes ( '' ) shall preserve
          the literal value of each character within the single-quotes.
          A single-quote cannot occur within single-quotes.

    '''

    return "\\'".join("'" + p + "'" for p in string.split("'"))

def string_to_bool(string: str):
    if string.lower() in ("yes", "true"):
        return True
    else:
        return False

def bool_to_string(bool):
    if bool:
        return "yes"
    else:
        return "no"

def run(program, *args, **kargs):
    time = kargs.get("time", False)
    new_args = []
    for key in kargs:
        value = kargs[key]
        new_args.append(f"--{key}={value}")
    for value in args:
        new_args.append(f"--{value}")
    if time:
        program = "time " + program
    cmd = str(program + " " + " ".join(new_args))
    logging.info("Running: " + cmd)
    process = subprocess.run(cmd, stderr=subprocess.STDOUT, stdout=subprocess.PIPE, shell=True)
    return process.stdout.decode()

def textEvent(e):
    logging.debug(f"state: {e.state}")
    logging.debug(f"key: {e.keysym}")
    if (e.state == 20 and e.keysym == "c"): #TODO: Add other exceptions like Ctrl+a
        return
    else:
        return "break"

def set_widget_geometry(widget, width=None, height=None):
    """Set the geometry of a widget. Default to half of the screen size.  If width or height is greater then the screen size will use default value for that demension"""
    screen_width = widget.winfo_screenwidth()
    screen_height = widget.winfo_screenheight()
    if not (width and width <= screen_width):
        if width:
            logging.warning("Trying to set width larger than screen size. Defaulting to half of screen width")
        width =  screen_width/2
    if not (height and height <= screen_height):
        if height:
            logging.warning("Trying to set height larger than screen size. Defaulting to half of screen height")
        height =  screen_height/2
    widget.geometry(f"{int(width)}x{int(height)}")

#Adapted from https://stackoverflow.com/questions/4770993/how-can-i-make-silent-exceptions-louder-in-tkinter
class Stderr(object):
    def __init__(self, parent):
        self.txt = Text(parent)
        self.pack(self.txt, )
    def write(self, s):
        self.txt.insert('insert', s)
    def fileno(self):
        return 2

class Data:
    def __create_attribute_list(self):
        try:
            self._attrs_
        except:
            dict.__setattr__(self, "_attrs_", []) #We use this list to perserve order

    def __init__(self, **kargs) -> None:
        self.__create_attribute_list()
        for key, value in kargs.items():
            self._attrs_.append(key)
            if type(value) != dict:
                setattr(self, key, value)
            else:
                setattr(self, key, Data(**value))
    
    def _dict_(self):
        d = {}
        for attribute in self._attrs_:
        # for attribute in dir(self):
            if not attribute.startswith("_"):
                var = getattr(self, attribute)
                if type(var) == Data:
                    d[attribute] = var._dict_()
                else:
                    d[attribute] = var
        return d
    
    def __setattr__(self, name: str, value) -> None:
        self.__create_attribute_list()
        self._attrs_.append(name)
        dict.__setattr__(self, name, value)
        

class Component:
    def __init__(self, parent, name, source:Data, special_valid_params, special_required_params) -> None:
        self.source_attribute = "value"
        self.parent = parent
        self.frame = Frame(parent)
        self.name = name
        self.source = source

        self.params = [x for x in dir(self.source) if not x.startswith("_")]
        self.required_params = special_required_params
        self.valid_params = special_valid_params

        for p in self.required_params:
            if p not in self.params:
                raise RuntimeError(f"Parameter {p} is required and not found in object '{source}.{name}'")
        
        for key in self.params:
            if key not in self.valid_params:
                raise RuntimeError(f"Parameter '{key}' in '{name}' is not a valid param. Valid params are {self.valid_params}.")
            setattr(self, key, getattr(self.source, key))
        
        for key in list(set(self.params).symmetric_difference(set(self.valid_params))):
            setattr(self, key, "default")
            self.params.append(key)
    
    @property
    def value(self):
        return getattr(self.source, self.source_attribute)
    
    @value.setter
    def value(self, value):
        setattr(self.source, self.source_attribute, value)

    def get_hidden(self):
        if "hidden" in dir(self):
            if type(self.hidden) is bool:
                return self.hidden
            elif type(self.hidden) is str:
                return string_to_bool(self.hidden)
            else:
                raise RuntimeError("Hidden is not a String or a Boolean")
        else:
            return False

    def pack(self, tk, **kargs):        
        if not self.get_hidden():
            tk.pack(kargs)
    
    def grid(self, tk, **kargs):
        if not self.get_hidden():
            tk.grid(kargs)
        
    def get_frame(self):
        return self.frame

class Option(Component):
    def __init__(self, parent, section, name, data, special_valid_params = [], special_required_params=[]) -> None:
        self.source_attribute = "value"
        required_params = ["type"]
        valid_params = ["type", "value", "label", "desc", "hidden", "fill", "side", "expand"]
        super().__init__(parent, name, getattr(getattr(getattr(getattr(data, "sections"), section), "options"), name), special_required_params=special_required_params + required_params, special_valid_params=special_valid_params + valid_params)
        self.fill = "both" if self.fill == "default" else self.fill
        self.side = "top" if self.side == "default" else self.side
        self.expand = False if self.expand == "default" else self.expand

    @property
    def value(self):
        return getattr(self.source, self.source_attribute)
    
    @value.setter
    def value(self, value):
        setattr(self.source, self.source_attribute, value)
        
    def get_frame(self):
        return self.frame
    
    def get_option(self):
        if self.value not in ("no", ""):
            return f"--{self.name}={QuoteForPOSIX(self.value)}"
        else:
            return ""

class ToolTip(object): #Adapted from https://stackoverflow.com/questions/20399243/display-message-when-hovering-over-something-with-mouse-cursor-in-python

    def __init__(self, widget):
        self.widget = widget
        self.tipwindow = None
        self.id = None
        self.x = self.y = 0

    def showtip(self, text):
        "Display text in tooltip window"
        self.text = text
        if self.tipwindow or not self.text:
            return
        x, y, cx, cy = self.widget.bbox("insert")
        x = x + self.widget.winfo_rootx() + 57
        y = y + cy + self.widget.winfo_rooty() +27
        self.tipwindow = tw = Toplevel(self.widget)
        tw.wm_overrideredirect(1)
        tw.wm_geometry("+%d+%d" % (x, y))
        label = Label(tw, text=self.text, justify="left",
                      background="#ffffe0", relief="solid", borderwidth=1,
                      font=("tahoma", "8", "normal"))
        label.pack(ipadx=1)

    def hidetip(self):
        tw = self.tipwindow
        self.tipwindow = None
        if tw:
            tw.destroy()

def CreateToolTip(widget, text):
    toolTip = ToolTip(widget)
    def enter(event):
        toolTip.showtip(text)
    def leave(event):
        toolTip.hidetip()
    widget.bind('<Enter>', enter)
    widget.bind('<Leave>', leave)


class OptionDir(Option):
    def __init__(self, parent, section, name, data):
        super().__init__(parent, section, name, data, special_valid_params=["width"])
        # Setting defaults
        self.width = 20 if self.width == "default" else self.width
        self.label = self.name if self.label == "default" else self.label
        self.value = "" if self.value == "default" else self.value

        #Building GUI
        self.container = LabelFrame(self.get_frame(), text=f"{self.label}" + (f" - {self.desc}" if len(self.desc) != 0 else ""))
        self.pack(self.container, fill="both", expand=True)
        # self.label_tk = Label(self.container, text=self.label)
        # self.pack(self.label_tk, side="left")
        self.directory_entry = Entry(self.container, width=self.width)
        self.directory_entry.bind('<KeyRelease>', self.handler)
        self.directory_entry.insert(0, self.value)
        self.pack(self.directory_entry, side="left", fill="both", expand=True)
        self.browse_button = Button(self.container, text="browse", command=self.browse_dir)
        self.pack(self.browse_button, side="right")
        CreateToolTip(self.browse_button, "Browse for a directory.")
        # self.desc_label = Label(self.container, text = self.desc, font=("", 8)) #TODO: Make a hover-over pop up
        # CreateToolTip(self.desc_label, self.desc)
        # self.pack(self.desc_label, side="left")
    
    def handler(self, event):
        logging.debug(f"Setting value to {self.directory_entry.get()}")
        self.value = self.directory_entry.get()
    
    def browse_dir(self):
        initDir=self.value
        if initDir=="":
            initDir=os.getcwd()
        if not os.path.isdir(initDir):
            messagebox.showerror("Error", f'Specified directory not found.  Value was:{"(Empty)" if initDir=="" else initDir}')
            initDir=""
        dir = filedialog.askdirectory(initialdir=initDir)
        if not dir in ("", ()): #askdirectory can return an empty tuple(Escape pressed) or an empty string(Cancel pressed)
            self.directory_entry.delete(0, "end")
            self.directory_entry.insert(0, dir)
            self.handler(None)

class OptionBool(Option):
    def __init__(self, parent, section, name, data):
        super().__init__(parent ,section, name, data)
        #Setting defaults
        self.value = "no" if self.value == "default" else self.value
        self.label = self.name if self.label == "default" else self.label
    
        #Building GUI
        self.container = Frame(self.get_frame())
        self.container.pack(expand=True, fill="both")
        self.bool = BooleanVar(value = self.value)
        self.check_button = Checkbutton(self.container, text=self.label, command=self.handler, variable=self.bool)
        self.pack(self.check_button, side="left")
        self.desc_label = Label(self.container, text = f": {self.desc}" if len(self.desc) != 0 else "") #TODO: Make a pop up
        self.pack(self.desc_label, side="left")
        # CreateToolTip(self.check_button, self.desc)
    
    def handler(self):
        logging.debug(f"Setting value to {self.bool.get()}.")
        self.value = "yes" if self.bool.get() else "no"
    
    def get_option(self):
        value = string_to_bool(str(self.value))
        return f"--{self.name}" + f"={QuoteForPOSIX(bool_to_string(value))}"

class OptionFlag(OptionBool):
    def __init__(self, parent, section, name, data) -> None:
        super().__init__(parent, section, name, data)
    def get_option(self):
        value = string_to_bool(str(self.value))
        if value:
            return f"--{self.name}"
        else:
            return ""

class OptionString(OptionDir):
    def __init__(self, parent, section, name, data):
        super().__init__(parent, section, name, data)

        self.container["text"] = self.container["text"]
        self.browse_button.pack_forget()

class OptionEnvVar(OptionDir):
    def __init__(self, parent, section, name, data):
        super().__init__(parent, section, name, data)

        self.container["text"] = "ENV: " + self.container["text"]
        self.browse_button.pack_forget()

    def get_option(self):
        value = str(self.value)
        if value == "":
            if self.name in os.environ:
                del os.environ[self.name]
            return ""
        os.environ[self.name] = value
        if os.environ[self.name] != value: #TODO: Find a reliable way to get the env var.  Example with spaces in name is not working.
            raise RuntimeError(f"Environment variable error. Cannont set {self.name} to {value}.")
        return f"{self.name} = {value}"


class OptionRadio(Option):
    def __init__(self, parent, section, name: str, data: Data):
        super().__init__(parent, section, name, data, special_valid_params=["options"], special_required_params=[])
        self.options = Data(**{}) if self.options == "default" else self.options
        self.value = "" if self.value == "default" else self.value

        self.box = LabelFrame(self.get_frame(), text=f"{self.name} - {self.desc}")
        self.pack(self.box, side="left")

        self.variable = StringVar(value=self.value)
        for key, obj in self.options._dict_().items():
            desc = obj.get("desc", "")
            value = obj.get("value", key)
            label = obj.get("label", "")
            if label == "":
                label = key
            if len(desc) > 0:
                desc = " - " + desc
            self.pack(Radiobutton(self.box, text=f"{label}{desc}", variable = self.variable, value=value, command=lambda: self.handler()), anchor="w")

    def handler(self):
        if self.variable.get() == self.value:
            self.variable.set("")
        logging.debug(f"Setting value to {self.variable.get()}")
        self.value = self.variable.get()
    
    def get_option(self):
        if self.value.lower() == "empty":
            return f"--{self.name}"
        elif self.value not in ("no", ""):
            return f"--{self.name}={QuoteForPOSIX(self.value)}"
        else:
            return ""            

class Section(Component):
    def __init__(self, parent, section, data:Data): #TODO: Figure out if I can pass in data instead of making it global
        valid_params = ["options", "size"] #TODO: Use size or take it out of valid params
        required_params = ["options"]
        super().__init__(parent, section, getattr(getattr(data, "sections"), section), special_valid_params=valid_params, special_required_params=required_params)

        self.scrollable = self.get_scrollable_frame(self.get_frame())

        self.components = {}
        if type(parent) == Notebook:
            parent.add(self.get_frame(), text=section)
        
        options = getattr(self.source, "options")._dict_()
        for option in options:
            obj = getattr(getattr(self.source, "options"), option)
            my_type = obj.type
            if my_type == "dir":
                self.components[option] = OptionDir(self.get_scrollable(), section, option, data)
            elif my_type == "bool":
                self.components[option] = OptionBool(self.get_scrollable(), section, option, data)
            elif my_type == "flag":
                self.components[option] = OptionFlag(self.get_scrollable(), section, option, data)
            elif my_type == "envvar":
                self.components[option] = OptionEnvVar(self.get_scrollable(), section, option, data)
            elif my_type == "radio":
                self.components[option] = OptionRadio(self.get_scrollable(), section, option, data)
            elif my_type == "string":
                self.components[option] = OptionString(self.get_scrollable(), section, option, data)
            else:
                raise RuntimeError(f"Option type '{my_type}' in {option} is not implemented yet.")
            
            # self.components[option].get_frame().pack(fill="both", expand=1, side="top")
            self.pack(self.components[option].get_frame(), fill = self.components[option].fill, expand = self.components[option].expand)
        
    def get_scrollable(self):
        if self.scrollable:
            return self.scrollable
        else:
            return self.get_frame()

    def get_frame(self):
        return self.frame
    
    def get_required_height(self):
        total = 0
        for component in self.components.values():
             total += component.get_frame().winfo_height()
        return total
    
    def update_scrollbar(self):
        if self.get_required_height() < self.main_frame.winfo_height():
            self.my_scrollbar.pack_forget()
            self.scrollable = False
        else:
            self.my_scrollbar.pack(side="right", fill="y")
            self.scrollable = True

    def get_scrollable_frame(self, parent):
        self.main_frame = Frame(parent)
        self.main_frame.pack(fill="both", expand=True)

        self.main_frame.bind("<Configure>", lambda e: self.update_scrollbar())

        self.my_canvas = Canvas(self.main_frame)
        self.my_canvas.pack(side="left", fill="both", expand=True)

        self.my_scrollbar = ttk.Scrollbar(master=self.main_frame, orient="vertical", command=self.my_canvas.yview)

        self.my_canvas.configure(yscrollcommand=self.my_scrollbar.set)

        second_frame = Frame(self.my_canvas)
        canvasFrame = self.my_canvas.create_window((0, 0), window=second_frame, anchor="nw")
        
        self.setIsInCanvas(False)
        second_frame.bind("<Configure>", lambda e: self.my_canvas.configure(scrollregion=self.my_canvas.bbox("all")))
        self.my_canvas.bind('<Configure>', lambda e: self.my_canvas.itemconfig(canvasFrame, width=e.width))
    
        self.my_canvas.bind('<Enter>', lambda e: self.setIsInCanvas(True))
        self.my_canvas.bind('<Leave>', lambda e: self.setIsInCanvas(False))

        return second_frame
    
    def setIsInCanvas(self, bool):
        self.isInCanvas = bool
    
    def _scroll(self, dir):
        if self.scrollable:
            if self.isInCanvas:
                if self.get_frame().winfo_ismapped():
                    speed = 1
                    self.my_canvas.yview_scroll(dir * speed, "units")
    
    def scroll_up(self):
        self._scroll(1)

    def scroll_down(self):
        self._scroll(-1)

def get_configure_command(command: str, sections: dict, include_vars=False):
    vars = []
    args = []
    for section in sections.values():
        for option in section.components.values():
            value = option.get_option()
            if value != "":
                if option.type != "envvar":
                    args.append(value)
                else:
                    vars.append(value)
    command = command + " " +  " ".join(args)
    if include_vars:
        command = "\n".join(vars) + "\n" + command
    return command.strip()

class App(Component):
    def __init__(self, my_json_or_filename, program: str = "/home/cherpin/git/trick/configure", resource_folder: str = f'{os.path.dirname(os.path.realpath(__file__))}/resources', back_up_filename=None) -> None:
        if type(my_json_or_filename) == str: #Handle a file name
            self.open(my_json_or_filename)
            self.filename = my_json_or_filename
        elif type(my_json_or_filename == dict): #Handle a dictionary object
            self.filename = back_up_filename #Allows us to still save to a file when we are passing in json as our config options.
            self.data = Data(**my_json_or_filename)
            self.my_json = my_json_or_filename
        else:
            raise RuntimeError(f"Invalid parameter my_json_or_file: {my_json_or_filename}.")

        self.trick_home = os.path.dirname(program)
        self.output_file = os.path.join(self.trick_home, "share", "trick", "makefiles", "config_user.mk")
        self._program = program
        self.resource_folder = resource_folder

        self.root = tkinter.Tk()

        set_widget_geometry(self.root)

        super().__init__(self.root, "app", self.data, special_required_params=["sections"], special_valid_params=["sections", "name", "landing", "time_stamp"])
        
        self.name = "app" if self.name == "default" else self.name
        # self.time_stamp = -2 if self.time_stamp == "default" else self.time_stamp
        if self.time_stamp == "default":
            self.time_stamp = Data(value=-2)#We use -2 here because -1 is used if the output file does not exist
            # setattr(self.source, "time_stamp", self.time_stamp)


        self.root.title(self.name)
        self.root.minsize(width=500, height=400)
        self.root.report_callback_exception = self.report_callback_exception
        
        self.header = Frame(self.root)
        self.header.pack(side = "top", fill="x")

        self.footer = Frame(self.root)
        self.footer.pack(side="bottom", fill="x")

        self.options_title = "Options for script"
        self.notebook_label_frame = LabelFrame(self.root, text=self.options_title)
        self.notebook_label_frame.pack(expand=True, fill="both")
        
        self.body = Frame(self.notebook_label_frame)
        self.body.pack(expand=True, fill="both")

        #Setting up navigation buttons
        def switch_tab(dir):
            total_number_of_tabs = len(self.showing["sections"])
            if total_number_of_tabs > 0:
                showing = list(self.showing["sections"])
                next_id = showing.index(self.notebook_name) + dir
                if total_number_of_tabs - 1 < next_id:
                    next_id = list(self.sections).index(showing[0])
                elif next_id < 0:
                    next_id = list(self.sections).index(showing[total_number_of_tabs - 1])
                else:
                    next_id = list(self.sections).index(showing[next_id])
                self.notebook.select(next_id)

        navigation_frame = Frame(self.body)
        navigation_frame.pack(anchor="e")

        tab_right_button = Button(navigation_frame, text="Next Tab", command=lambda: switch_tab(1)) #TODO: Make this a picture
        tab_right_button.pack(side="right")
        tab_left_button = Button(navigation_frame, text="Previous Tab", command=lambda: switch_tab(-1)) #TODO: Make this a picture
        tab_left_button.pack(side="right")


        self.add_shortcuts()
        self.build_menu(self.root)
        self.build_search_bar(self.header)
        self.build_current_script(self.footer)

        self.notebook_frame = Frame(self.body)
        self.build_notebook(self.body)
        self.build_current_command() #We can only run this after we build a notebook

        self._status = StringVar()
        self.status_label = Label(self.footer, textvariable=self._status)
        self.set_status()
        self.status_label.pack(side="left")
    
    @property
    def program(self):
        return self._program
    
    @program.setter
    def program(self, value):
        self._program = value
        self.update_status()
        self.build_current_command()
    
    def set_status(self, msg=None):
        if msg is None:
            msg = f"Config file: {self.filename}"
        self._status.set("Status - " + msg)
        self.status_label.pack(side="left")
        self.status_label.pack()

    def add_shortcuts(self):
        self.root.bind(f"<Alt-h>", lambda e: self.show_help())
        self.root.bind(f"<Alt-e>", lambda e: self.execute())
        self.root.bind(f"<Alt-o>", lambda e: self.focus_options())
        self.root.bind(f"<Alt-s>", lambda e: self.focus_search())

    def focus_options(self):
        self.notebook_label_frame.focus_set()
    
    def focus_search(self):
        self.search_entry.focus_set()

    def conf(self, e):
            self.body.update()
            height = self.body.winfo_height()
            width = self.body.winfo_width()
            self.notebook.configure(height=height, width=width)

    def build_notebook(self, parent):
        self.notebook = ttk.Notebook(parent)
        # self.body.bind("<Configure>", self.conf)
        self.notebook.pack(fill="both", expand=True) 
        self.sections = {}
        sections = getattr(self.source, "sections")._dict_()
        for section in sections:
            obj = getattr(getattr(self.source, "sections"), section)
            if len(getattr(obj, "options")._dict_()) > 0: #Note: not adding section if empty
                self.sections[section] = Section(self.notebook, section, self.source)
                # CreateToolTip(self.sections[section].get_frame(), section)
        
        self.previous_section_length = 0

        def call_func_on_obj(obj, func):
            if obj:
                getattr(obj, func)()
        
        self.get_frame().bind_all('<Button-4>', lambda e: call_func_on_obj(self.sections.get(self.notebook_name), "scroll_down"))
        self.get_frame().bind_all('<Button-5>', lambda e: call_func_on_obj(self.sections.get(self.notebook_name), "scroll_up"))
        self.call_search()
    

    @property
    def notebook_name(self):
        if len(self.showing["sections"]) > 0:
            return self.notebook.tab(self.notebook.select(), "text")

    def build_search_bar(self, parent):
        #Search box
        self.outer_search_box = LabelFrame(parent, text="Filter Options")
        self.outer_search_box.pack(side="left", anchor="n", fill="x", expand=1)

        self.img = PhotoImage(file=f'{self.resource_folder}/trick_small.gif')
        Label(self.outer_search_box, image=self.img).pack(side="right")

        self.search_box = Frame(self.outer_search_box)
        self.search_box.rowconfigure(0, weight=1)
        self.search_box.columnconfigure(0, weight=1)

        self.search_label = Label(self.search_box, text = "Search for options:", underline=0)
        # self.search_label.grid(row=0, column=0, sticky="ew")
        self.search_label.pack(expand=True, fill="x")

        self.search_entry = Entry(self.search_box)
        self.search_entry.bind("<KeyRelease>", self.call_search)
        CreateToolTip(self.search_entry, "Search for a specific option.")
        # self.search_entry.grid(row=0, column=1, sticky="ew")
        self.search_entry.pack(expand=True, fill="x")

        self.pack(self.search_box, side="top", anchor="e", expand=True, fill="x")

        self.only_checked = BooleanVar(False)
        self.checked_toggle = Checkbutton(self.outer_search_box, variable=self.only_checked, text="Show only used options", command=self.call_search)
        self.checked_toggle.pack(side="right", anchor="e", expand=True, fill="x")

        #End Search box
    
    def get_match_status(self):
        try:
            self.last_update_time = int(os.path.getmtime(self.output_file))
        except FileNotFoundError:
            self.last_update_time = -1
            logging.info(f"Output file, {self.output_file}, cannot be found.")
        
        try:
            logging.info(f"Comparing timestamps of output file, {self.output_file}, and config file, {self.filename}.")
            if self.filename != None:
                self.config_update_time = int(os.path.getmtime(os.path.join(self.filename)))
            else:
                self.config_update_time = None
        except FileNotFoundError:
            logging.critical(f"Config file, {self.filename}, cannot be found.")
            raise FileNotFoundError
        
        logging.info(f"Output file, {self.output_file}, timestamp={self.last_update_time}, and config file, {self.filename}=, timestamp={self.config_update_time}.")
        # if int(float(self.time_stamp.value)) == int(self.last_update_time):
        if self.config_update_time == self.last_update_time:
            color = "green"
            status = ""
        else:
            color = "yellow4"
            status = "Warning: Trick's current configuration may be different from the configuration shown here."
        return status, color


    def build_current_script(self, parent):
        #Current Script
        self.current_script = Frame(parent)
        self.current_script.pack(side="top", anchor="n", fill="x", expand=True)

        self.label_frame = LabelFrame(self.current_script, text="Current Script with Options", underline=21)
        self.label_frame.pack(side="top", expand=True, fill="x")

        self.current_command = ScrolledText(self.label_frame, height=4, state="normal")
        self.current_command.bind("<Key>", textEvent)
        self.current_command.bind("<Enter>", lambda e: self.setIsInCurrentCommand(True))
        self.current_command.bind("<Leave>", lambda e: self.setIsInCurrentCommand(False))
        self.current_command.pack(side="top", anchor="w", fill="x", expand=True)

        self.setIsInCurrentCommand(False)

        self.root.bind("<KeyRelease>", self.build_current_command)
        self.root.bind("<ButtonRelease-1>", self.build_current_command)

        self.status_frame = Frame(self.label_frame)
        self.status_frame.pack()

        status, color = self.get_status()
        self.label_status = Label(self.status_frame, text=f"Status: {status}", foreground=color)
        self.label_status.pack()

        status, color = self.get_match_status()
        if len(status) > 0:
            self.label_match_status = Label(self.status_frame, text=f"{status}", foreground=color)
            CreateToolTip(self.label_match_status, f"The timestamp of configure's output file, {self.output_file}, does not much that of our {self.filename} file.  \nThis usually means that \n  1. 'configure' was run without this program, \n  2. this program has not been run yet, or \n  3. the json has been manually changed since the last time this program was run.")
            self.label_match_status.pack()

        self.button_frame = Frame(self.label_frame)
        self.button_frame.pack()

        self.help_button = Button(self.button_frame, text=f"Help for script", command=self.show_help, underline=0)
        self.help_button.pack(side="left", anchor="w", expand=True, fill="both", padx=10)


        self.done_button = Button(self.button_frame, text="Execute command with options (will remember settings)", command=self.execute, underline=0)
        CreateToolTip(self.done_button, f"Execute command with options and save settings to {self.filename}.")
        self.done_button.pack(side="right", anchor="e", expand=True, fill="both", padx=5)
    
    def setIsInCurrentCommand(self, value):
        self.isInCurrentCommand = value

    def update_status(self):
        self.label_status["text"], self.label_status["foreground"] = self.get_status()

    def get_status(self):
        rvalue = ""
        color = "black"
        if os.access(self.program, os.X_OK):
            rvalue += "Valid"
            color = "green"
        else:
            rvalue += "Invalid"
            color = "red"
        return rvalue + " Executable File", color

    def show_help(self): #TODO: This code is being repeated where we a ScrolledText widget
        self.win = tk.Toplevel()
        self.win.title("General help for the configure script")
        set_widget_geometry(self.win)
        output = run(self.program, "help")
        self.output = ScrolledText(self.win, state="normal", height=8, width=50)
        self.output.bind("<Key>", textEvent)
        self.output.insert(1.0, output)
        self.pack(self.output, fill="both", expand=True, anchor="w")

    def build_current_command(self, e=None):
        # self.current_command["state"] = "normal"
        if not self.isInCurrentCommand:
            # text = get_configure_command(self.program, self.source._dict_(), include_vars=True)
            text = get_configure_command(self.program, self.sections, include_vars=True)
            self.current_command.delete(1.0, "end")
            self.current_command.insert(1.0, text)
        # self.current_command["state"] = "disabled"
        # self.current_command["text"] = text

    def build_menu(self, parent):
        menubar = Menu(parent)
        filemenu = Menu(menubar, tearoff=0)
        filemenu.add_command(label="Select command", command=self.select_command)
        filemenu.add_command(label="Save options", command=self.save)
        filemenu.add_separator()
        filemenu.add_command(label="Exit", command=parent.destroy) #TODO: This may not work for non root parents
        menubar.add_cascade(label="File", menu=filemenu, underline=0)

        parent.config(menu=menubar)

    def select_command(self):
        initDir = os.path.abspath(os.path.dirname(self.program))
        file = filedialog.askopenfilename(initialdir=initDir)
        if file not in ("", ()):
            self.program = os.path.abspath(file)
            

    def call_search(self, e=None):
        current = self.search_entry.get()
        self._search(current, self.sections)
        msg = " (filtered)"
        if current != "" or self.only_checked.get():
            self.notebook_label_frame["text"] = self.options_title + msg
        else:
            self.notebook_label_frame["text"] = self.options_title
    
    def _search(self, word, sections):
        section_id = 0
        self.current_section_length = 0
        self.showing = { "sections" : {} }
        for section in sections:
            options = sections[section].components
            count_hidden = 0
            self.showing["sections"][section] = {}
            self.showing["sections"][section]["options"] = {}
            for option in options: #TODO: Allow for double grouping
                if (word != '' and not App.is_match(word, option, options[option].desc, options[option].label)) or (self.only_checked.get() and options[option].value in ("no", "")):
                    options[option].get_frame().pack_forget()
                    count_hidden += 1
                else:
                    options[option].get_frame().pack(fill = options[option].fill, )
                    self.showing["sections"][section]["options"][option] = self.my_json["sections"][section]["options"][option]
            if count_hidden == len(sections[section].components):
                self.notebook.hide(section_id)
                del self.showing["sections"][section]
            else:
                if self.previous_section_length == 0:
                    self.notebook.select(0)
                self.notebook.add(sections[section].get_frame())
                self.current_section_length += 1
            section_id += 1
        self.previous_section_length = self.current_section_length
        return self.showing
    
    @staticmethod
    def is_match(search, *args): #Pass in args to see if search is a match with any of the arguments
        rvalue = False
        for a in args:
            if search.lower() in a.lower():
                rvalue = True
        return rvalue


    def get_frame(self):
        return self.root
    
    def execute(self, answer=None, on_done=None):
        self.set_status("Running script")
        if not answer:
            answer = messagebox.askyesno(title="Confirmation", message=f"Would you like to configure trick with your chosen options?")
        
        if answer:
            output = Output(self.program, self.sections, on_done=self.root.destroy)
            self.save()
            output.get_frame().mainloop()
        else:
            self.set_status()
        
            
    def save(self, filename=None):
        date = int(time.mktime(datetime.datetime.now().timetuple()))
        self.time_stamp.value = date
        if filename == None:
            if self.filename == None:
                raise RuntimeError(f"No file to save configuration to.")
            else:
                filename = self.filename
        with open(filename, "w") as f:
            f.write(json.dumps(self.source._dict_(), indent=4)) #TODO: What happens if there is an error on this line
            try:
                os.makedirs("archive")
            except OSError as exception:
                if exception.errno != errno.EEXIST:
                    raise
        timestr = time.strftime("%Y%m%d-%H%M%S")
        shutil.copyfile(filename, f"archive/{timestr}_{ntpath.basename(filename)}")

    
    def open(self, filename):
        with open(filename, "r") as f:
            new_json = json.load(f)
            self.data = Data(**new_json)
            self.my_json = new_json
    
    #Adapted from https://stackoverflow.com/questions/4770993/how-can-i-make-silent-exceptions-louder-in-tkinter
    def report_callback_exception(self, exc, val, tb):
        #Handles tkinter exceptions
        err_msg = {
            "No file to save configuration to." : "You cannot save you current options because Gsetup was run without a configuration file."
            }
        err = err_msg.get(str(val), f'Unknown Error:{val}')
        logging.error(traceback.format_exception(exc, val, tb))
        messagebox.showerror('Error Found', err)
    
    def is_saved(self):
        # return DeepDiff(self.original_dict, self.data._dict_())
        return self.original_dict == self.data._dict_()        

class Output():
    def __init__(self, program, sections, on_done=None):
        self.root = tk.Tk()
        self.on_done = on_done
        cmd = get_configure_command(program, sections)
        
        output = run(cmd)
        output = f"{cmd}\n\n" + output

        self.root.title("Script's output")
        set_widget_geometry(self.root)
        self.output = ScrolledText(self.root, state="normal", height=8, width=50)
        self.output.bind("<Key>", textEvent)
        self.output.insert(1.0, output)
        self.output.pack(fill="both", expand=True, anchor="w")
        self.finish_button = Button(self.root, text="Finished", command=self.quit)
        self.finish_button.pack(anchor="e")
    
    def get_frame(self):
        return self.root

    def quit(self):
        self.root.destroy()
        if self.on_done:
            self.on_done()
    
class LandingPage(Component):
    def __init__(self, parent=None, config_file="./config.json", program="configure", initial_dir=os.getcwd(), resource_folder = f'{os.path.dirname(os.path.realpath(__file__))}/resources') -> None:
        if parent:
            self.root = parent
        else:
            self.root = Tk()
        
        self.root.maxsize(width=531, height=292) #These numbers were found through trial and error
        self.root.minsize(width=531, height=292) #These numbers were found through trial and error
        set_widget_geometry(self.root, 531, 292)

        if type(config_file) is str:
            with open(config_file, "r") as f:
                app_json = json.load(f)
            self.filename = config_file
        elif type(config_file) is dict:
            app_json = config_file
        else:
            raise RuntimeError(f"Config_file is {type(config_file)}. It must be either a string or a dict.")
        self.data = Data(**(app_json.get("landing", {})))

        super().__init__(parent, app_json.get("name", "landing"), self.data, special_valid_params=["version", "desc"], special_required_params=[]) #Note: there should be no required params for Landing because landing itself is not required

        self.resource_folder = resource_folder
        self.program = program

        #Set default values
        self.version = "x.x" if self.version == "default" else self.version 
        self.desc = "This setup guide will allow you to easily see all the options that are available to configure Trick with." if self.desc == "default" else self.desc

        self.root.title(self.name)
        
        self.open_advanced = False
        self.to_close = True

        self.header = Frame(self.root)
        self.body = Frame(self.root)
        self.footer = Frame(self.root)

        self.header.pack()
        self.body.pack(expand=True, fill="both")
        self.footer.pack()


        self.release_label = Label(self.header, text=f"Release {self.version}")
        self.release_label.pack(anchor="w")
        
        
        self.title_frame = Frame(self.header)
        self.desc_label = Label(self.title_frame, text="Welcome to Trick.", font='Helvetica 15 bold')
        self.desc_label.pack(side="left")
        self.img = PhotoImage(file=f'{self.resource_folder}/trick_icon.gif')
        Label(self.title_frame, image=self.img).pack(side="left")
        self.title_frame.pack()


        self.desc_label2 = Label(self.header, wraplength=500, text=self.desc)
        self.desc_label2.pack(pady=10)
        
        self.labelScript = Label(self.body, text=f"Script filename: {os.path.basename(self.program)}")
        self.labelScript.pack(anchor="w", padx=50, pady=0)

        self.label = Label(self.body, text="Script location:")
        self.label.pack(anchor="w", padx=50, pady=0)

        self.folder_location = StringVar(value=initial_dir)

        self.browse_frame = Frame(self.body)

        self.folder_entry = Entry(self.browse_frame, textvariable=self.folder_location)
        self.folder_entry.pack(side="left", expand=True, fill="x", pady=0)

        self.change_button = Button(self.browse_frame, text="Browse", command=self.change_dir)
        CreateToolTip(self.change_button, "Click here to choose Trick's home directory.  Configure will run from within this directory.")
        self.change_button.pack(side="left", padx=(10, 0))

        self.browse_frame.pack(fill="x", expand=True, padx=50, anchor="n")

        self.configure_fast_button = Button(self.footer, text="Configure with defaults", command=self.configure)
        CreateToolTip(self.configure_fast_button, "Run configure with the default options.")
        self.configure_fast_button.pack(side="left", padx=10, pady=10)

        self.configure_button = Button(self.footer, text="Configure with advanced options", command=self.configure_with_options)
        CreateToolTip(self.configure_button, "Choose advanced options to configure trick with.")
        self.configure_button.pack(side="left", padx=10, pady=10)
        
        self.close_button = Button(self.footer, text="Close", command=self.close)
        self.close_button.pack(side="left", padx=10, pady=10)

    def change_dir(self):
        dir = filedialog.askdirectory(initialdir=self.folder_location.get())
        if not dir in ("", ()):
            self.folder_location.set(dir)
        else:
            logging.error("Invalid directory.")
    
    def set_program(self):
        currdir = os.path.abspath(os.getcwd())
        try:
            os.chdir(self.folder_location.get())
        except:
            messagebox.showerror(title="Invalid directory", message=f"{self.folder_location.get()} is not a valid directory")
            return False
        arr = glob.glob(self.program)
        if len(arr) > 0:
            self.program = os.path.abspath(arr[0])
            return True
        else:
            which = shutil.which(self.program)  #Note: if self.program contains a path, which will always be false (e.g. the default ./configure will never be found in the path)
            if which:
                self.program = which
                return True
            os.chdir(currdir)
            messagebox.showerror(title="Wrong home directory", message=f"No SCRIPT_FILE file found in location: {self.folder_location.get()} or in the PATH env variable. Please enter a valid location for this file (this is usually your TRICK_HOME directory).") #TODO: Don't hard code this message
            return False

    def configure(self):
        if self.set_program():
            self.open_advanced = False
            self.to_close = False
            self.close()
    
    def close(self):
        self.root.destroy()

    def configure_with_options(self):
            if self.set_program():
                self.open_advanced = True
                self.to_close = False
                self.close()
    def get_frame(self):
        return self.root
        
        
def main(argv=[]):
    parser = argparse.ArgumentParser()

    default = "(default: %(default)s)"
    parser.add_argument("-s", "--script-file", default="./configure", help=f"script to add args to and to execute.{default}.  If the filename exists in the current folder, it will be run, otherwise, the path env variable will be used to find the filename.  Note: ./configure will never be found in the path because in includes a path.")
    parser.add_argument("-c", "--config", default=f"{os.path.dirname(os.path.realpath(__file__))}/trick_config.json", help=f"json file with gui options and settings {default}")
    parser.add_argument("-b", "--build", action="store_true", default=False, help=f"guess the parameter choices from the scripts help output.  Use '-c config.json' to use these parameters. {default}")
    parser.add_argument('-l', '--logging', default="warning", choices=["critical", "error", "warning", "info", "debug", "notset"], help=f"Set logging level.  {default}")
    args = parser.parse_args(argv)
    
    # log_level = logging.getLevelName(args.logging)
    log_level = args.logging.upper()
    logging.basicConfig(
        format='%(asctime)s %(levelname)-8s %(message)s',
        level=log_level,
        datefmt='%Y-%m-%d %H:%M:%S')
    logging.info(f"Logging level is set to {log_level}")

    
    resource_folder = f'{os.path.dirname(os.path.realpath(__file__))}/resources'
    
    if args.build:
        from load import load, write_help
        write_help(args.script_file)
        load()
    
    config_file = args.config
    if not os.path.isfile(config_file):
        logging.info(f"The config file did not exist.  Creating a default config file at: {config_file}")
        #Config file does not exist so we create one with the default configuration
        with open(config_file, "w") as f:
            json.dump(default_trick_config, f, indent=4)
    else:
        logging.info(f"The config file, {config_file} exists.  If this file does not exist, a default would be created.")
    config_file = os.path.abspath(config_file) #Landing page will change cwd so we get abs path
    if os.path.exists(args.script_file):
        script_folder = os.path.dirname(os.path.abspath(args.script_file))
    else:
        script_folder = os.getcwd()
    landingPage = LandingPage(parent=None, config_file=config_file, program=args.script_file, initial_dir=script_folder, resource_folder=resource_folder)
    landingPage.get_frame().mainloop()
    
    if not landingPage.to_close:
        if landingPage.open_advanced:
            a = App(config_file, landingPage.program, resource_folder=resource_folder)
            a.get_frame().mainloop()
        else:
            output = Output(landingPage.program, {})
            output.get_frame().mainloop()

if __name__ == "__main__":
    main(sys.argv[1:])
