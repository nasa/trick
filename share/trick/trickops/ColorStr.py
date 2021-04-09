# Simple color string utility class
class ColorStr:

    def __init__( self ):
        self.CODE={
            'ENDC':0,  # RESET COLOR
            'BOLD':1,
            'UNDERLINE':4,
            'BLINK':5,
            'INVERT':7,
            'CONCEALD':8,
            'STRIKE':9,
            'GREY30':90,
            'GREY40':2,
            'GREY65':37,
            'GREY70':97,
            'GREY20_BG':40,
            'GREY33_BG':100,
            'GREY80_BG':47,
            'GREY93_BG':107,
            'DARK_RED':31,
            'RED':91,
            'RED_BG':41,
            'LIGHT_RED_BG':101,
            'DARK_YELLOW':33,
            'YELLOW':93,
            'YELLOW_BG':43,
            'LIGHT_YELLOW_BG':103,
            'DARK_BLUE':34,
            'BLUE':94,
            'BLUE_BG':44,
            'LIGHT_BLUE_BG':104,
            'DARK_MAGENTA':35,
            'PURPLE':95,
            'MAGENTA_BG':45,
            'LIGHT_PURPLE_BG':105,
            'DARK_CYAN':36,
            'AQUA':96,
            'CYAN_BG':46,
            'LIGHT_AUQA_BG':106,
            'DARK_GREEN':32,
            'GREEN':92,
            'GREEN_BG':42,
            'LIGHT_GREEN_BG':102,
            'BLACK':30 \
        }

    def getCodes( self ):
        return self.CODE

    def termcode(self, num):
        return '\033[%sm'%num

    def colorstr(self, astr, color):
        return self.termcode(self.CODE[color])+astr+self.termcode(self.CODE['ENDC'])

    def showCodes( self ):
        astr='yippy skippy'
        codes = self.getCodes()
        for key in sorted(codes.keys()):
            print('%s: %s' % (key, self.colorstr( astr, key )))
    def cprint( self, astr, color ):
        print(self.colorstr( astr, color ))
