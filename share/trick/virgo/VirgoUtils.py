def cprint(text, color="white", bold=False):
    colors = {
        "red": "\033[91m",
        "green": "\033[92m",
        "yellow": "\033[93m",
        "blue": "\033[94m",
        "white": "\033[97m",
    }
    reset = "\033[0m"
    bold_code = "\033[1m" if bold else ""
    print(f"{bold_code}{colors.get(color, '')}{text}{reset}")